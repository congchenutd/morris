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
	virtual void setMemoryLimit(int) {}

	void setEstimator(Estimator* est) { estimator = est; }
	QString run(bool opening, const QString& input, QChar startColor, int by, int depth, int tl);
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
};

class MinMax : public MorrisAlgorithm
{
protected:
	virtual int runAlgorithm(const Board& board);

	int maxMin(const Board& board);
	int minMax(const Board& board);
};

struct TreeNode
{
	TreeNode(int d = 0, int s = 0) : depth(d), score(s) {}
	void addChild(const TreeNode& node) { children.push_back(node); }
	void toFile(const QString& fileName);

	std::vector<TreeNode> children;
	int depth;
	int score;
};

class AlphaBeta : public MorrisAlgorithm
{
public:
	AlphaBeta() {}

protected:
	virtual int runAlgorithm(const Board& board);
	int maxMin(const Board& board, int alpha, int beta, TreeNode& parent);
	int minMax(const Board& board, int alpha, int beta, TreeNode& parent);

	TreeNode root;
};

class NegaMax : public MorrisAlgorithm
{
public:
	virtual void endOpening() { db.clear(); }
	virtual void setMemoryLimit(int size);

protected:
	virtual int runAlgorithm(const Board& board);

	int negaMax(const Board& board, int alpha, int beta, int sign);
	void sortMoves(Moves& moves, int sign);

private:
	enum {TIME_OUT = 123456789};
	QTime  time;
	MoveDB db;
	HistoryHeuristc hh;
};


#endif
