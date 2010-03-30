#include "Morris.h"
#include "Estimator.h"
#include "MoveGenerator.h"
#include <QtGui>

using namespace std;

MorrisAlgorithm::MorrisAlgorithm()
{
	generator = new MoveGenerator;
	estimator = 0;
}

MorrisAlgorithm::~MorrisAlgorithm() {
	delete generator;
}

QString MorrisAlgorithm::run(bool opening, const QString& input, QChar startColor, int depth)
{
	maxDepth = depth;
	estimator->setStartColor(startColor);
	estimator->setOpening(opening);
	estimator->resetCounter();
	generator->setOpening(opening);
	runAlgorithm(Board(input, startColor));   // really run it
	return nextMove.toString();
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
		int temp = minMax(*it);
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
		int temp = maxMin(*it);
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
		int temp = minMax(*it, alpha, beta);
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
		int temp = maxMin(*it, alpha, beta);
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
// Improved AlphaBeta
int AlphaBetaImproved::maxMin(const Board& board, int alpha, int beta)
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

	Moves::iterator maxMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = minMax(*it, alpha, beta);
		if(temp > value)
		{
			value = temp;
			maxMove = it;
		}

		// definitely to win, no need to go deeper
		if(temp == Estimator::MAX_ESTIMATION)
			break;

		if(value >= beta)
			break;
		if(value > alpha)
			alpha = value;
	}

	nextMove = *maxMove;
	maxValue = value;
	return maxValue;
}

int AlphaBetaImproved::minMax(const Board& board, int alpha, int beta)
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
		int temp = maxMin(*it, alpha, beta);
		if(temp < minValue)
		{
			minValue = temp;
			minMove = it;
		}

		// definitely to lose, no need to go deeper
		if(temp == Estimator::MIN_ESTIMATION)
			break;

		if(minValue <= alpha)
			break;
		if(minValue < beta)
			beta = minValue;
	}

	nextMove = *minMove;
	return minValue;
}