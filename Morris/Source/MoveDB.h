#ifndef MoveDB_h__
#define MoveDB_h__

#include <vector>
#include <QHash>
#include "Board.h"
#include "HashTable.h"

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
	void saveMove(const Board& current, const Board& next, int score, int alpha, int beta);
	void saveEstimation(const Board& board, int score);
	void clear() { dbWhite.clear(); dbBlack.clear(), estimationDB.clear(); }
	void setSize(int size);
	void load();
	void save();

private:
	HashTable<MoveRecord> dbWhite, dbBlack;
	HashTable<int> estimationDB;
};

QTextStream& operator << (QTextStream& os, const MoveRecord& record);
QTextStream& operator >> (QTextStream& is, MoveRecord& record);

#endif // MoveDB_h__