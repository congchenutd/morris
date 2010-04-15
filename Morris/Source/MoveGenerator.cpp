#include "MoveGenerator.h"
#include <algorithm>

using namespace std;

Moves MoveGenerator::generate(const Board& board) 
{
	if(!isOpening && board.countNumber(board.getSelfColor()) < 3)  // game over
		return Moves(); 
	return isOpening ? generateOpening(board) : generateGame(board);
}

Moves MoveGenerator::generateOpening(const Board& board) const
{
	Moves result;
	for(int i=0; i<23; ++i)
	{
		if(board.isEmpty(i))
		{
			Board next = board.makeChild();  // a clone
			next.setManAt(i, board.getSelfColor());
			if(next.closeMill(i))
			{
				Moves removeMoves = generateRemove(next, board.getOpponentColor());  // remove 1 opponent chessman
				copy(removeMoves.begin(), removeMoves.end(), back_inserter(result));
			}
			else
				result.push_back(next);
		}
	}
	return result;
}

Moves MoveGenerator::generateGame(const Board& board) const
{
	if(board.countNumber(board.getSelfColor()) == 3)
		return generateHopping(board);
	return generateMove(board);
}

Moves MoveGenerator::generateRemove(const Board& old, QChar color) const
{
	Moves result;
	for(int i=0; i<23; ++i)
	{
		if(old.getManAt(i) == color)
		{
			if(!old.closeMill(i))
			{
				// DO NOT use old.makeChild
				// next is a semi-finished product of a move
				Board next = old;
				next.setManAt(i, 'x');
				result.push_back(next);
			}
		}
	}
	if(result.empty())      // no possible remove
		result.push_back(old);
	return result;
}

Moves MoveGenerator::generateHopping(const Board& board) const
{
	Moves result;
	for(int from=0; from<23; ++from)
		if(board.getManAt(from) == board.getSelfColor())
		{
			for(int to=0; to<23; ++to)
				if(board.isEmpty(to))
				{
					Board next = board.makeChild();
					next.setManAt(from, 'x');
					next.setManAt(to, board.getSelfColor());
					if(next.closeMill(to))
					{
						Moves removeMoves = generateRemove(next, board.getOpponentColor());
						copy(removeMoves.begin(), removeMoves.end(), back_inserter(result));
					}
					else
						result.push_back(next);
				}
		}
	return result;
}

Moves MoveGenerator::generateMove(const Board& board) const
{
	Moves result;
	for(int from=0; from<23; ++from)
		if(board.getManAt(from) == board.getSelfColor())
		{
			Neighbors neighbors = Board::getNeighbors(from);
			for(Neighbors::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
				if(board.isEmpty(*it))
				{
					Board next = board.makeChild();
					next.setManAt(from, 'x');
					next.setManAt(*it, board.getSelfColor());
					if(next.closeMill(*it))
					{
						Moves removeMoves = generateRemove(next, board.getOpponentColor());
						copy(removeMoves.begin(), removeMoves.end(), back_inserter(result));
					}
					else
						result.push_back(next);
				}
		}
		return result;
}