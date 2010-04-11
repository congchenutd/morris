#ifndef MORRIS_H
#define MORRIS_H

/************************************************************************/
/* MinMax algorithms                                                    */
/************************************************************************/

#include "Board.h"
#include "MoveGenerator.h"
#include "MoveDB.h"
#include <QString>
#include <QTime>

class MoveGenerator;
class Estimator;
class MorrisAlgorithm
{
public:
	MorrisAlgorithm();
	virtual ~MorrisAlgorithm();

	void setEstimator(Estimator* est) { estimator = est; }
	QString run(bool opening, const QString& input, QChar startColor, int depth, int tl = 30);
	int getMaxValue() const { return maxValue; }
	int getMaxDepth() const { return maxDepth; }
	void clearDB() { db.clear(); }
	double getHitRatio() const { return (double)hit / node; }

protected:
	virtual int runAlgorithm(const Board& board) = 0;
	bool isLeaf(const Board& board) const { return board.getDepth() >= maxDepth; }

protected:
	int            maxDepth;
	MoveRecord     nextMove;   // run() returns it, as the actual output
	int            maxValue;   // runAlgorithm() returns it
	Estimator*     estimator;
	MoveGenerator* generator;
	MoveDB         db;
	long node;
	long hit;
	int timeLimit;
};

class MinMax : public MorrisAlgorithm
{
protected:
	virtual int runAlgorithm(const Board& board);

	int maxMin(const Board& board);
	int minMax(const Board& board);
};

class AlphaBeta : public MorrisAlgorithm
{
protected:
	virtual int runAlgorithm(const Board& board);
	virtual int maxMin(const Board& board, int alpha, int beta);
	virtual int minMax(const Board& board, int alpha, int beta);
};

class AlphaBetaImproved : public MorrisAlgorithm
{
protected:
	virtual int runAlgorithm(const Board& board);
	virtual int maxMin(const Board& board, int alpha, int beta);
	virtual int minMax(const Board& board, int alpha, int beta);

	Moves getSortedMoves(const Board& board, bool minMax);

private:
	enum {TIME_OUT = 123456789};
};

class NegaMax : public MorrisAlgorithm
{
protected:
	virtual int runAlgorithm(const Board& board);

	int negaMax(const Board& board, int alpha, int beta, int sign);
	Moves getSortedMoves(const Board& board, int sign);

private:
	enum {TIME_OUT = 123456789};
	QTime time;
};

#endif