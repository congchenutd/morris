#include "Board.h"
#include <algorithm>
#include <set>
#include <QtGui>

using namespace std;

bool Board::closeMill(int pos) const {
	return !findMills(pos).empty();
}

bool Board::isMill(const Mill& mill) const {
	return chessmen[mill[0]] == chessmen[mill[1]] && 
		   chessmen[mill[1]] == chessmen[mill[2]] && 
		   chessmen[mill[0]] != 'x';
}

int Board::findFirstAdded(const Board& newBoard, QChar color, int start)
{
	for(int i=start; i<23; ++i)
		if(chessmen[i] != color && newBoard.chessmen[i] == color)
			return i;
	return -1;
}

int Board::findFirstDeleted(const Board& newBoard, QChar color, int start)
{
	for(int i=start; i<23; ++i)
		if(chessmen[i] == color && newBoard.chessmen[i] != color)
			return i;
	return -1;
}

int Board::countNumber(QChar color) const {
	return chessmen.count(color);
}

Neighbors Board::getNeighbors(int pos)
{
	// row is current position, elements are its neighbors, at most 4, -1 means null
	static const int rawData[][4] =
	{
		{1,3,8,-1},
		{0,2,4,-1},
		{1,5,13,-1},
		{0,4,6,9},
		{1,3,5,-1},
		{2,4,7,12},
		{3,7,10,-1},
		{5,6,11,-1},
		{0,9,20,-1},
		{3,8,10,17},
		{6,9,14,-1},
		{7,12,16,-1},
		{5,11,13,19},
		{2,12,22,-1},
		{10,15,17,-1},
		{14,16,18,-1},
		{11,15,19,-1},
		{9,14,18,20},
		{15,17,19,21},
		{12,16,18,22},
		{8,17,21,-1},
		{18,20,22,-1},
		{13,19,21,-1}
	};
	Neighbors result;
	for(int j=0; j<4; ++j)
		if(rawData[pos][j] > -1)
			result.push_back(rawData[pos][j]);
	return result;
}

bool Board::isNeighbor(int lhs, int rhs)
{
	const Neighbors neighbors = getNeighbors(lhs);
	return find(neighbors.begin(), neighbors.end(), rhs) != neighbors.end();
}

int Board::countJoints(QChar color) const
{
	const static int threeJoints[16] = {0, 1, 2, 4, 6, 7, 8, 10, 11, 13, 14, 15, 16, 20, 21, 22};
	const static int fourJoints [7]  = {3, 5, 9, 12, 17, 18, 19};
	int result = 0;
	for(int i=0; i<16; ++i)
		if(chessmen[ threeJoints[i] ] == color)
			result += 3;
	for(int i=0; i<7; ++i)
		if(chessmen[ fourJoints[i] ] == color)
			result += 4;
	return result;
}

Board Board::makeChild() const {
	return Board(toString(), getOpponentColor(), depth - 1);
}

const int Board::allMills[][3] = 
{
	{0,1,2},
	{0,3,6},
	{0,8,20},
	{1,0,2},
	{2,0,1},
	{2,5,7},
	{2,13,22},
	{3,0,6},
	{3,9,17},
	{3,4,5},
	{4,3,5},
	{5,2,7},
	{5,3,4},
	{5,12,19},
	{6,0,3},
	{6,10,14},
	{7,2,5},
	{7,11,16},
	{8,0,20},
	{8,9,10},
	{9,3,17},
	{9,8,10},
	{10,6,14},
	{10,8,9},
	{11,7,16},
	{11,12,13},
	{12,5,19},
	{12,11,13},
	{13,2,22},
	{13,11,12},
	{14,6,10},
	{14,17,20},
	{14,15,16},
	{15,14,16},
	{15,18,21},
	{16,7,11},
	{16,14,15},
	{16,19,22},
	{17,3,9},
	{17,14,20},
	{17,18,19},
	{18,15,21},
	{18,17,19},
	{19,5,12},
	{19,16,22},
	{19,17,18},
	{20,0,8},
	{20,14,17},
	{20,21,22},
	{21,15,18},
	{21,20,22},
	{22,2,13},
	{22,16,19},
	{22,20,21},
};

int Board::countOpenMills(QChar c) const
{
	std::set<Mill> openMills;
	for(int i=0; i<23; ++i)
		if(getManAt(i) == c)
		{
			std::set<Mill> temp = findOpenMills(i, c);
			copy(temp.begin(), temp.end(), inserter(openMills, openMills.end()));
		}

	return openMills.size();
}

std::set<Mill> Board::findOpenMills(int pos, QChar color) const
{
	std::set<Mill> result;
	for(int i=0; i<54; ++i)
	{
		if(allMills[i][0] == pos)
		{
			if( (chessmen[allMills[i][1]] == color && chessmen[allMills[i][2]] == 'x') ||
				(chessmen[allMills[i][2]] == color && chessmen[allMills[i][1]] == 'x'))
			{
				Mill mill(allMills[i], allMills[i]+3);
				sort(mill.begin(), mill.end());
				result.insert(mill);
			}
		}
		else if(allMills[i][0] > pos)   // ignore remaining rows
			break;
	}
	return result;
}

int Board::countMills(QChar color) const
{
	std::set<Mill> mills;
	for(int i=0; i<23; ++i)
		if(getManAt(i) == color)
		{
			std::set<Mill> temp = findMills(i);
			copy(temp.begin(), temp.end(), inserter(mills, mills.end()));
		}

	return mills.size();
}

std::set<Mill> Board::findMills(int pos) const
{
	std::set<Mill> result;
	for(int i=0; i<54; ++i)
	{
		if(allMills[i][0] == pos)
		{
			Mill mill(allMills[i], allMills[i]+3);
			if(isMill(mill))
			{
				sort(mill.begin(), mill.end());
				result.insert(mill);
			}
		}
		else if(allMills[i][0] > pos)   // ignore remaining rows
			break;
	}
	return result;
}

int Board::countMorris(QChar color) const
{
	int result = 0;
	for(int i=0; i<23; ++i)
		if(chessmen[i] == color)   // find color
		{
			Neighbors neighbors = getNeighbors(i);
			for(Neighbors::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			{
				if(chessmen[*it] == 'x')        // for all it's empty neighbor
				{
					Board temp(*this);
					temp.setManAt(i, 'x');
					temp.setManAt(*it, color);  // try to move to this neighbor
					if(temp.closeMill(*it))
						result ++;
				}
			}
		}
	return result;
}

int Board::countBlocked(QChar color) const
{
	int result = 0;
	for(int i=0; i<23; ++i)
		if(chessmen[i] == color)   // find color
			if(isBlocked(i))
				result ++;
	return result;
}

bool Board::isBlocked(int pos) const
{
	Neighbors neighbors = getNeighbors(pos);
	for(Neighbors::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		if(chessmen[*it] == 'x')        // try to find an empty neighbor
			return false;
	return true;
}