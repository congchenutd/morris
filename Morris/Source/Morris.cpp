#include "Morris.h"
#include "Estimator.h"
#include <QtGui>
#include <algorithm>
#include <queue>

using namespace std;

MorrisAlgorithm::MorrisAlgorithm()
{
	generator = new MoveGenerator;
	estimator = 0;
}

MorrisAlgorithm::~MorrisAlgorithm() {
	delete generator;
}

QString MorrisAlgorithm::run(bool opening, const QString& input, QChar startColor, 
							 int by, int depth, int tl, int idleW, int idleB)
{
	limitBy = by;
	maxDepth = depth;
	timeLimit = tl * 1000;
	isOpening = opening;
	idleWhite = idleW;
	idleBlack = idleB;
	estimator->setStartColor(startColor);
	estimator->setOpening(opening);
	estimator->resetCounter();
	generator->setOpening(opening);
	runAlgorithm(Board(input, startColor, depth, idleWhite, idleBlack));
	return nextMove.nextMove.toString();
}

//////////////////////////////////////////////////////////////////////////
// MinMax
int MinMax::runAlgorithm(const Board& board) {
	return maxMin(board);
}

int MinMax::maxMin(const Board& board)
{
	if(isLeaf(board))
		return estimator->getEstimation(board);

	Moves moves = generator->generate(board);
	int value = Estimator::MIN_ESTIMATION;
	
	// no future move, definitely lose
	if(moves.empty())
	{
		nextMove = board;
		maxValue = value;
		return maxValue;
	}
	Moves::iterator maxMove = moves.begin();   // use pointer to avoid string copy
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = minMax(it->nextMove);
		if(temp > value)
		{
			value = temp;
			maxMove = it;
		}
	}

	nextMove = *maxMove;
	maxValue = value;
	return maxValue;
}

int MinMax::minMax(const Board& board)
{
	if(isLeaf(board))
		return estimator->getEstimation(board);

	Moves moves = generator->generate(board);
	int minValue = Estimator::MAX_ESTIMATION;
	if(moves.empty())
		return minValue;
	Moves::iterator minMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = maxMin(it->nextMove);
		if(temp < minValue)
		{
			minValue = temp;
			minMove = it;
		}
	}
	nextMove = *minMove;
	return minValue;
}


//////////////////////////////////////////////////////////////////////////
// AlphaBeta
int AlphaBeta::runAlgorithm(const Board& board) {
	return maxMin(board, -INT_MAX, INT_MAX);
}

int AlphaBeta::maxMin(const Board& board, int alpha, int beta)
{
	if(isLeaf(board))
		return estimator->getEstimation(board);

	Moves moves = generator->generate(board);
	int value = Estimator::MIN_ESTIMATION;

	// no future move, definitely lose
	if(moves.empty())
	{
		maxValue = value;
		nextMove = board;
		return maxValue;
	}

	Moves::iterator maxMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = minMax(it->nextMove, alpha, beta);
		if(temp > value)
		{
			value = temp;
			maxMove = it;
		}
		if(value >= beta)
			break;
		if(value > alpha)
			alpha = value;
	}

	nextMove = *maxMove;
	maxValue = value;
	return maxValue;
}

int AlphaBeta::minMax(const Board& board, int alpha, int beta)
{
	if(isLeaf(board))
		return estimator->getEstimation(board);

	Moves moves = generator->generate(board);
	int minValue = Estimator::MAX_ESTIMATION;
	if(moves.empty())
		return minValue;

	Moves::iterator minMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = maxMin(it->nextMove, alpha, beta);
		if(temp < minValue)
		{
			minValue = temp;
			minMove = it;
		}
		if(minValue <= alpha)
			break;
		if(minValue < beta)
			beta = minValue;
	}

	nextMove = *minMove;
	return minValue;
}


//////////////////////////////////////////////////////////////////////////
int NegaMax::runAlgorithm(const Board& b) 
{
	Board board(b);
	estimator->setDB(&db);
	int result = Estimator::MIN_ESTIMATION;

	// iterative deepening
	if(limitBy == DlgSetting::LIMIT_BY_DEPTH)
	{
		int maxDepthBackup = maxDepth;
		for(maxDepth = 1; maxDepth <= maxDepthBackup; maxDepth++)
		{
			board.setDepth(maxDepth);
			result = negaMax(board, -INT_MAX, INT_MAX, 1);
		}
		maxDepth = maxDepthBackup;
	}
	else
	{
		nextMove.nextMove = board;
		
		time.restart();
		for(maxDepth = 1; time.elapsed() < timeLimit; maxDepth ++)
		{
			board.setDepth(maxDepth);        // a new depth
			MoveRecord rollBack = nextMove;  // take a snapshot

			int temp = negaMax(board, -INT_MAX, INT_MAX, 1);  // run
			
			// already win / lose
			if(temp == Estimator::MAX_ESTIMATION)
				return temp;

			if(temp == TIME_OUT)
			{
				nextMove = rollBack;   // roll back on timeout
				break;
			}

			if(temp > result)          // accept better result only
			{
				result = temp;
				rollBack = nextMove;   // new snapshot
			}
			else
				nextMove = rollBack;   // roll back
		}
	}
	return result;
}

int NegaMax::negaMax(const Board& board, int alpha, int beta, int sign)
{
	// timeout and abort
	if(limitBy == DlgSetting::LIMIT_BY_TIME)
		if(time.elapsed() > timeLimit)
			return TIME_OUT;

	// Search db
	MoveRecord record = db.searchMove(board);
	if(record.score != MoveRecord::NOT_FOUND)
	{
		// if lower level iteration found winning move, do it
		// must be the same color/sign
		if(record.score == Estimator::MAX_ESTIMATION && record.nextMove.getSelfColor() == board.getOpponentColor())
		{
			nextMove = record.nextMove;
			maxValue = record.score;
			return maxValue;
		}
		if(record.depth >= board.getDepth())
		{
			nextMove = record.nextMove;
			maxValue = record.score;
			if(maxValue * sign > 0)   // same sign
				return maxValue;
		}
	}

	if(isLeaf(board))
		return sign * estimator->getEstimation(board);

	generator->setOpening(board.getIdleCount() > 0);   // switch to game phase
	Moves moves = generator->generate(board);

	int value = Estimator::MIN_ESTIMATION;
	if(moves.empty())  // no future move, definitely lose
		return value;

	sortMoves(board, moves);   // using history heuristic

	Moves::iterator maxMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = - negaMax(it->nextMove, -beta, -alpha, -sign);

		if(limitBy == DlgSetting::LIMIT_BY_TIME)   // timeout, abort
			if(temp == TIME_OUT || temp == -TIME_OUT)
				return TIME_OUT;

		if(temp == Estimator::MAX_ESTIMATION)
		{
			value = temp;
			maxMove = it;
			break;
		}

		if(temp > value)
		{
			value = temp;
			maxMove = it;
		}
		if(value >= beta)    // beta cut-off
			break;
		if(value > alpha)    // reduce window
			alpha = value;
	}

	nextMove = *maxMove;
	maxValue = value;

	// save to db
	db.saveMove(board, nextMove.nextMove, maxValue);
	db.saveHistory(board, nextMove.nextMove);

	return maxValue;
}

void NegaMax::setMemoryLimit(int size) {
	db.setSize(size);
}

void NegaMax::sortMoves(const Board& board, Moves& moves)
{
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
		it->score = db.searchHistory(board, it->nextMove);
	sort(moves.begin(), moves.end(), greater<MoveRecord>());
}

NegaMax::~NegaMax() {
//	db.save();
}

void NegaMax::loadDB() {
//	db.load();
}