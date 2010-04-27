#ifndef MORRIS_H
#define MORRIS_H

/************************************************************************/
/* MinMax algorithms                                                    */
/************************************************************************/

#include "Board.h"
#include "MoveGenerator.h"
#include "MoveDB.h"
#include "DlgSetting.h"
#include <QString>
#include <QTime>
#include <vector>

class MoveGenerator;
class Estimator;
class MorrisAlgorithm
{
public:
	MorrisAlgorithm();
	virtual ~MorrisAlgorithm();
	virtual void endOpening() {}
	virtual void setMemoryLimit(int) {}  // for negaMax
	virtual void loadDB() {}

	void setEstimator(Estimator* est) { estimator = est; }
	QString run(bool opening, const QString& input, QChar startColor, 
				int by, int depth, int tl, int idleW, int idleB);
	int getMaxValue() const { return maxValue; }
	int getMaxDepth() const { return maxDepth; }

protected:
	virtual int runAlgorithm(const Board& board) = 0;
	bool isLeaf(const Board& board) const { return board.getDepth() <= 0; }

protected:
	Estimator*     estimator;
	MoveGenerator* generator;

	MoveRecord nextMove;   // run() returns it, as the actual output
	int        maxValue;   // runAlgorithm() returns it
	int        limitBy;
	int        maxDepth;
	int        timeLimit;
	bool       isOpening;
	int        idleWhite;
	int        idleBlack;
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
	int maxMin(const Board& board, int alpha, int beta);
	int minMax(const Board& board, int alpha, int beta);
};

class NegaMax : public MorrisAlgorithm
{
public:
	~NegaMax();
	virtual void endOpening() { db.clear(); }
	virtual void setMemoryLimit(int size);
	virtual void loadDB();

protected:
	virtual int runAlgorithm(const Board& board);

	int negaMax(const Board& board, int alpha, int beta, int sign);
	void sortMoves(const Board& board, Moves& moves);

private:
	enum {TIME_OUT = 123456789};
	QTime  time;
	MoveDB db;
};


#endif
