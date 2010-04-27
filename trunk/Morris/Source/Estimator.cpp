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
	const int   opponentMoveNum = board.countMoves(isOpening, opponentColor);
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
	int result = 1000*numDiff + 1*freedomDiff + 1*blockedNum + 10*millNum + 50*openMillNum;
	return result;
}

int ImprovedEstimator::getGameEstimation(const Board& board) const
{
	QChar opponentColor = Board::flipColor(startColor);
	int selfNum         = board.countNumber(startColor);
	int opponentNum     = board.countNumber(opponentColor);
	int numDiff         = selfNum - opponentNum;
	int opponentMoveNum = board.countMoves(isOpening, opponentColor);
	int millNum         = board.countMills(startColor);
	int openMillNum     = board.countOpenMills(startColor);
	int morrisNum       = board.countMorris(startColor);
	int doubleMorrisNum = board.countDoubleMorris(startColor);

	if(opponentNum <= 2)
		return MAX_ESTIMATION;
	if(selfNum <= 2)
		return MIN_ESTIMATION;
	if(opponentMoveNum == 0)
		return MAX_ESTIMATION;

	return numDiff         * 1000 + 
		   opponentMoveNum * -1 +
		   millNum         * 10 + 
		   openMillNum     * 20 + 
		   morrisNum       * 30 +
		   doubleMorrisNum * 100;
}