#ifndef MoveGenerator_h__
#define MoveGenerator_h__

#include <vector>
#include "Board.h"

struct MoveRecord
{
	typedef enum {EXACT_VALUE, LOWER_BOUND, HIGHER_BOUND} RecordType;

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

class MoveGenerator
{
public:
	MoveGenerator() : isOpening(true) {}
	virtual Moves generate(const Board& board);
	void setOpening(bool opening) { isOpening = opening; }
	static int countMoves(const Board& board, bool isOpening, QChar color);

private:
	Moves generateOpening(const Board& board) const;
	Moves generateGame   (const Board& board) const;
	Moves generateRemove (const Board& old, QChar color) const;
	Moves generateHopping(const Board& board) const;
	Moves generateMove   (const Board& board) const;
private:
	bool isOpening;
};

#endif // MoveGenerator_h__
