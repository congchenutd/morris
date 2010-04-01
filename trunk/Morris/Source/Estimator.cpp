#include "Estimator.h"
#include "Morris.h"
#include "MoveGenerator.h"

// Attention: all estimations are based on startColor's view
// Do not use board's color, because it changes

int Estimator::getEstimation(const Board& board) 
{
	counter ++;

	// search from database
	int score = db.search(board, isOpening);
	if(score != EstimationDB::NOT_FOUND)
		return score;

	// new search
	score = isOpening ? getOpeningEstimation(board) : getGameEstimation(board);
	db.save(board, score, isOpening);
	return score;
}

int BasicEstimator::getOpeningEstimation(const Board& board) const {
	return board.countNumber(startColor) - board.countNumber(Board::flipColor(startColor));
}

int BasicEstimator::getGameEstimation(const Board& board) const
{
	const int   selfNum         = board.countNumber(startColor);
	const QChar opponentColor   = Board::flipColor(startColor);
	const int   opponentNum     = board.countNumber(opponentColor);
	const int   opponentMoveNum = MoveGenerator::countMoves(board, isOpening, opponentColor);
	if(opponentNum <= 2)
		return MAX_ESTIMATION;
	if(selfNum <= 2)
		return MIN_ESTIMATION;
	if(opponentMoveNum == 0)
		return MAX_ESTIMATION;
	return 1000 * (selfNum - opponentNum) - opponentMoveNum;
}

// Improved estimator
// The only improvement I make this time is that improved estimator counts 
// joins instead of positions: the more joins a chessman takes, the better
// i.e.   |
//      __a__  __b__  a takes 4 joins while b takes 3
//        |      |
//     a is considered better than b because it has more mobility
//     Chessman with less mobility is more likely to be trapped and less powerful

int ImprovedEstimator::getOpeningEstimation(const Board& board) const {
	return board.countJoints(startColor) - board.countJoints(Board::flipColor(startColor));
}

int ImprovedEstimator::getGameEstimation(const Board& board) const
{
	const int   selfNum         = board.countNumber(startColor);
	const int   selfMoveNum     = MoveGenerator::countMoves(board, isOpening, startColor);
	const QChar opponentColor   = Board::flipColor(startColor);
	const int   opponentNum     = board.countNumber(opponentColor);
	const int   opponentMoveNum = MoveGenerator::countMoves(board, isOpening, opponentColor);
	if(opponentNum <= 2 || opponentMoveNum == 0)
		return MAX_ESTIMATION;
	if(selfNum <= 2 || selfMoveNum == 0)
		return MIN_ESTIMATION;
	return 1000 * (selfNum - opponentNum) +
			(selfMoveNum - opponentMoveNum);
}

int EstimationDB::search(const Board& board, bool opening) const
{
	const QHash<QString, int>& db = opening ? dbOpen : dbGame;
	QHash<QString, int>::const_iterator it = db.find(board.toString());
	if(it == db.end())
		return NOT_FOUND;
	return it.value();
}

void EstimationDB::save(const Board& board, int score, bool opening) 
{
	QHash<QString, int>& db = opening ? dbOpen : dbGame;
	db.insert(board.toString(), score);
}