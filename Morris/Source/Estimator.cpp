#include "Estimator.h"
#include "Morris.h"
#include "MoveGenerator.h"

// Attention: all estimations are based on startColor's view
// Do not use board's color, because it changes

int Estimator::getEstimation(const Board& board) 
{
	counter ++;
	return isOpening ? getOpeningEstimation(board) : getGameEstimation(board);
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

//////////////////////////////////////////////////////////////////////////
// Improved estimator
int ImprovedEstimator::getEstimation(const Board& board)
{
	counter ++;

	// search from database
	int score = db->searchEstimation(board);
	if(score != MoveRecord::NOT_FOUND)
		return score;

	// new search
	score = isOpening ? getOpeningEstimation(board) : getGameEstimation(board);
	db->saveEstimation(board, score);
	return score;
}

int ImprovedEstimator::getOpeningEstimation(const Board& board) const 
{
	//return BasicEstimator::getOpeningEstimation(board);
	QChar opponentColor = Board::flipColor(startColor);
	int selfNum         = board.countNumber(startColor);
	int opponentNum     = board.countNumber(opponentColor);
	int numDiff         = selfNum - opponentNum;
	int selfFreedom     = board.countFreedom(startColor);
	int opponentFreedom = board.countFreedom(opponentColor);
	int freedomDiff     = selfFreedom - opponentFreedom;
	int blockedNum      = board.countBlocked(opponentColor);
	int millNum         = board.countMills(startColor);
	int openMillNum     = board.countOpenMills(startColor);
	return 1000*numDiff + 10*freedomDiff + 10*blockedNum + 100*millNum + 200*openMillNum;
}

int ImprovedEstimator::getGameEstimation(const Board& board) const
{
	//return BasicEstimator::getGameEstimation(board);
	QChar opponentColor = Board::flipColor(startColor);
	int selfNum         = board.countNumber(startColor);
	int opponentNum     = board.countNumber(opponentColor);
	int numDiff         = selfNum - opponentNum;
	int blockedNum      = board.countBlocked(opponentColor);
	int millNum         = board.countMills(startColor);
	int openMillNum     = board.countOpenMills(startColor);
	int morrisNum = board.countMorris(startColor);
	return 1000*numDiff + 10*blockedNum + 100*millNum + 200*openMillNum + 300*morrisNum;
}