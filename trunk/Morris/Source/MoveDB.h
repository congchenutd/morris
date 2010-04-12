#ifndef MoveDB_h__
#define MoveDB_h__

#include <QHash>
#include "Board.h"

class Board;

struct MoveRecord
{
	typedef enum {EXACT_VALUE, LOWER_BOUND, UPPER_BOUND} RecordType;

	MoveRecord(const Board& next = QString(), int s = NOT_FOUND, 
		int d = 0, RecordType t = EXACT_VALUE)
		: nextMove(next), score(s), depth(d), type(t) {}

	bool operator < (const MoveRecord& other) const { return score < other.score; }
	bool operator > (const MoveRecord& other) const { return score > other.score; }

	Board nextMove;
	int   score;
	int   depth;
	RecordType type;

	enum {NOT_FOUND = 123456789};
};

typedef std::vector<MoveRecord> Moves;


class MoveDB
{
public:
	MoveRecord searchMove      (const Board& board) const;
	int        searchEstimation(const Board& board) const;
	void saveMove      (const Board& current, const MoveRecord& next);
	void saveEstimation(const Board& board, int score);
	void clear() { dbWhite.clear(); dbBlack.clear(); estimationDB.clear(); }
	int getMoveDBSize()      const { return dbWhite.size() + dbBlack.size(); }
	int getEstimationDBSize() const { return estimationDB.size(); }
	int getMoveDBCapacity() const { return dbWhite.capacity() + dbBlack.capacity(); }
	int getEstimationDBCapacity() const { return estimationDB.capacity(); }

private:
	QHash<QString, MoveRecord> dbWhite, dbBlack;
	QHash<QString, int> estimationDB;
};


#endif // MoveDB_h__