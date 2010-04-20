#ifndef Board_h__
#define Board_h__

/************************************************************************/
/* Chessboard, represented by a string like "xBWxxWx..."                */
/* B for black, W for white, x for empty                                */
/************************************************************************/

#include <vector>
#include <QString>
#include <set>

typedef std::vector<int> Mill, Neighbors;

class Board
{
public:
	Board(const QString& p = "xxxxxxxxxxxxxxxxxxxxxxx", QChar c = 'W', int d = 0) 
		: chessmen(p), color(c), depth(d) {}

	QChar   getSelfColor()     const { return color; }
	QChar   getOpponentColor() const { return flipColor(color); }
	int     getDepth()         const { return depth; }
	QString toString()         const { return chessmen; }
	bool    isEmpty (int pos)  const { return chessmen[pos] == 'x'; }
	QChar   getManAt(int pos)  const { return chessmen[pos]; }
	void    setManAt   (int pos, QChar man) { chessmen[pos] = man; }
	void    removeManAt(int pos)            { setManAt(pos, 'x'); }
	void    setString(const QString& str)   { chessmen = str; }
	void    setDepth(int d)                 { depth = d; }
	void    move(int from, int to);
	
	bool closeMill(int pos) const;            // if pos is in a mill
	Board makeChild() const;  // a clone
	
	int countNumber      (QChar color) const;    // # of chessmen
	int countFreedom     (QChar color) const;    // movable positions
	int countOpenMills   (QChar color) const;    // *-O-*
	int countMills       (QChar color) const;    // *-*-*
	int countMorris      (QChar color) const;    // *-O-*
												 //   |
												 //   *
	int countDoubleMorris(QChar color) const;    //   |
	int countBlocked     (QChar color) const;    // *-O-*
	int countMoves(bool isOpening, QChar color) const;   // including hopping

	int findFirstAdded  (const Board& newBoard, QChar color, int start = 0);  // find difference
	int findFirstDeleted(const Board& newBoard, QChar color, int start = 0);

	static QChar     flipColor(QChar color) { return (color == 'W') ? 'B' : 'W'; }
	static Neighbors getNeighbors(int pos);
	static bool      isNeighbor(int lhs, int rhs);

	bool operator == (const Board& other) const { return chessmen == other.chessmen; }

private:
	bool isMill(const Mill& mill) const;
	std::set<Mill> findOpenMills(int pos, QChar color) const;
	std::set<Mill> findMills    (int pos) const;
	bool isBlocked(int pos) const;
	
	// pos moving to a neighbor makes a morris
	bool closeMorris(int pos) const;

private:
	QString chessmen;
	QChar   color;           // current color
	int     depth;           // current depth in the game tree

	static const int allMills[][3];
};

#endif // Board_h__
