#ifndef MORRIS_H
#define MORRIS_H

/************************************************************************/
/* MinMax algorithms                                                    */
/************************************************************************/

#include "Board.h"
#include <QString>
#include <QHash>

struct MoveRecord
{
	Board nextMove;
	int   score;
	int   depth;
};

class MoveDB
{
public:
	MoveRecord search(const Board& board) const;
	void save (const Board& current, const MoveRecord& next);
	void clear() { dbWhite.clear(); dbBlack.clear(); }

public:
	enum {NOT_FOUND = 123456789};

private:
	QHash<QString, MoveRecord> dbWhite, dbBlack;
};

class MoveGenerator;
class Estimator;
class MorrisAlgorithm
{
public:
	MorrisAlgorithm();
	~MorrisAlgorithm();
	void setEstimator(Estimator* est) { estimator = est; }
	QString run(bool opening, const QString& input, QChar startColor, int depth);
	int getMaxValue() const { return maxValue; }
	void clearDB() { db.clear(); }

protected:
	virtual int runAlgorithm(const Board& board) = 0;
	bool isLeaf(const Board& board) const { return board.getDepth() == maxDepth; }

protected:
	int            maxDepth;
	Board          nextMove;   // run() returns it, as the actual output
	int            maxValue;   // runAlgorithm() returns it
	Estimator*     estimator;
	MoveGenerator* generator;
	MoveDB         db;
	long node;
	long hit;
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

class AlphaBetaImproved : public AlphaBeta
{
protected:
	virtual int maxMin(const Board& board, int alpha, int beta);
	virtual int minMax(const Board& board, int alpha, int beta);
};

#endif