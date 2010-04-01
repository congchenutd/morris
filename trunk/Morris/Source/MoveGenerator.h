#ifndef MoveGenerator_h__
#define MoveGenerator_h__

#include <vector>
#include "Morris.h"

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
