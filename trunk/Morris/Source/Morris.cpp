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
							 int by, int depth, int tl)
{
	limitBy = by;
	maxDepth = depth;
	timeLimit = tl * 1000;
	estimator->setStartColor(startColor);
	estimator->setOpening(opening);
	estimator->resetCounter();
	generator->setOpening(opening);
	runAlgorithm(Board(input, startColor, depth));
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
int AlphaBeta::runAlgorithm(const Board& board) 
{
	int score = maxMin(board, -INT_MAX, INT_MAX, root);
	root.depth = board.getDepth();
	root.score = score;
	root.toFile("tree.txt");
	return score;
}

int AlphaBeta::maxMin(const Board& board, int alpha, int beta, TreeNode& parent)
{
	TreeNode node(board.getDepth(), 0);
	if(isLeaf(board))
	{
		int temp = estimator->getEstimation(board);
		node.score = temp;
		parent.addChild(node);
		return temp;
	}

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
		int temp = minMax(it->nextMove, alpha, beta, node);
//		if(temp == Estimator::MAX_ESTIMATION)
//			break;
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
	node.score = maxValue;
	parent.addChild(node);
	return maxValue;
}

int AlphaBeta::minMax(const Board& board, int alpha, int beta, TreeNode& parent)
{
	TreeNode node(board.getDepth(), 0);
	if(isLeaf(board))
	{
		int temp = estimator->getEstimation(board);
		node.score = temp;
		parent.addChild(node);
		return temp;
	}

	Moves moves = generator->generate(board);
	int minValue = Estimator::MAX_ESTIMATION;
	if(moves.empty())
		return minValue;

	Moves::iterator minMove = moves.begin();
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		int temp = maxMin(it->nextMove, alpha, beta, node);
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
	node.score = minValue;
	parent.addChild(node);
	return minValue;
}

void TreeNode::toFile(const QString& fileName)
{
	QFile file(fileName);
	file.open(QFile::WriteOnly);
	QTextStream os(&file);
	queue<TreeNode*> q;
	q.push(this);
	q.push(0);
	int d = depth;
	while(!q.empty())
	{
		TreeNode* node = q.front();
		q.pop();
		if(node == 0)
		{
			os << "|";
			continue;
		}
		if(node->depth != d)
		{
			d = node->depth;
			os << "\r\ndepth=" << d << "\r\n";
		}
		os << node->score << " ";

		for(vector<TreeNode>::iterator it=node->children.begin(); it!=node->children.end(); ++it)
			q.push(&(*it));
		q.push(0);
	}
}

//////////////////////////////////////////////////////////////////////////
int NegaMax::runAlgorithm(const Board& b) 
{
	Board board(b);
	estimator->setDB(&db);
	int result = 0;

	// iterative deepening
	if(limitBy == DlgSetting::LIMIT_BY_DEPTH)
	{
		int maxDepthBackup = maxDepth;
		for(maxDepth = 1; maxDepth <= maxDepthBackup; maxDepth++)
		{
			board.setDepth(maxDepth);
			result = negaMax(board, -INT_MAX, INT_MAX, 1);
		}
		maxDepth --;
	}
	else
	{
		nextMove.nextMove = board;
		maxDepth = 0;
		time.restart();
		while(time.elapsed() < timeLimit)
		{
			maxDepth ++;
			board.setDepth(maxDepth);
			MoveRecord nextMoveRollback = nextMove;
			int temp = negaMax(board, -INT_MAX, INT_MAX, 1);
			if(temp != TIME_OUT)
			{
				result = temp;
				nextMoveRollback = nextMove;
			}
			else
			{
				nextMove = nextMoveRollback;   // rollback on failure
				maxDepth --;
			}
		}
	}
	return result;
}

int NegaMax::negaMax(const Board& board, int alpha, int beta, int sign)
{
	if(limitBy == DlgSetting::LIMIT_BY_TIME)
		if(time.elapsed() > timeLimit)
			return TIME_OUT;

	// Search db
	MoveRecord record = db.searchMove(board);
	if(record.score != MoveRecord::NOT_FOUND)
	{
		// if lower level iteration found killer move, do it
		if(record.score == Estimator::MAX_ESTIMATION && record.type == MoveRecord::EXACT_VALUE)
		{
			nextMove = record.nextMove;
			maxValue = record.score;
			return maxValue;
		}
		if(record.depth >= board.getDepth())
		{
			switch(record.type)
			{
			case MoveRecord::EXACT_VALUE:
				nextMove = record.nextMove;
				maxValue = record.score;
				return maxValue;
			case MoveRecord::LOWER_BOUND:
				alpha = max(alpha, record.score);
				break;
			case MoveRecord::UPPER_BOUND:
				beta = min(beta, record.score);
				break;
			}
			if(alpha > beta)
			{
				nextMove = record.nextMove;
				maxValue = record.score;
				return maxValue;
			}
		}
	}

	if(isLeaf(board))
		return sign * estimator->getEstimation(board);

	//Moves moves = getSortedMoves(board, sign);
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
		int temp = - negaMax(it->nextMove, -beta, -alpha, -sign);

		if(limitBy == DlgSetting::LIMIT_BY_TIME)
			if(temp == TIME_OUT || temp == -TIME_OUT)   // abort
				return TIME_OUT;

		// found killer move
		//if(temp == Estimator::MAX_ESTIMATION)
		//{
		//	value = temp;
		//	maxMove = it;
		//	break;
		//}

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

	// save to db
	db.saveMove(board, nextMove.nextMove, maxValue, alpha, beta);

	return maxValue;
}

Moves NegaMax::getSortedMoves(const Board& board, int sign)
{
	Moves moves = generator->generate(board);
	for(Moves::iterator it = moves.begin(); it != moves.end(); ++it)
		it->score = estimator->getEstimation(it->nextMove);
	if(sign == 1)
		sort(moves.begin(), moves.end(), greater<MoveRecord>());
	else
		sort(moves.begin(), moves.end());
	return moves;
}

void NegaMax::setMemoryLimit(int size) {
	db.setSize(size);
}