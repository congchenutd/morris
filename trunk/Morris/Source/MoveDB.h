#ifndef MoveDB_h__
#define MoveDB_h__

#include <vector>
#include <QHash>
#include "Board.h"
#include "HashTable.h"

class Board;

struct MoveRecord
{
	MoveRecord(const Board& next = QString(), int s = NOT_FOUND, int d = 0)
		: nextMove(next), score(s), depth(d) {}

	bool operator < (const MoveRecord& other) const { return score < other.score; }
	bool operator > (const MoveRecord& other) const { return score > other.score; }

	Board nextMove;
	int   score;
	int   depth;

	enum {NOT_FOUND = 123456789};
};

typedef std::vector<MoveRecord> Moves;
typedef std::vector<std::vector<ulong> > MoveHistory;

class MoveDB
{
public:
	MoveDB();
	MoveRecord searchMove (const Board& board) const;
	void saveMove(const Board& current, const Board& next, int score);

	void saveEstimation(const Board& board, int score);
	int  searchEstimation(const Board& board) const;

	void  saveHistory  (const Board& current, const Board& next);
	ulong searchHistory(const Board& current, const Board& next) const;

	void clear();
	void setSize(int size);
	void load();
	void save();

private:
	HashTable<MoveRecord> dbWhite, dbBlack;
	HashTable<int> estimationDB;
	MoveHistory moveHistoryWhite, moveHistoryBlack;
};

QTextStream& operator << (QTextStream& os, const MoveRecord& record);
QTextStream& operator >> (QTextStream& is, MoveRecord& record);


#endif // MoveDB_h__