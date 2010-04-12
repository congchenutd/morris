#ifndef Board_h__
#define Board_h__

/************************************************************************/
/* Chessboard, represented by a string like "xBWxxWx..."                */
/* B for black, W for white, x for empty                                */
/************************************************************************/

#include <vector>
#include <QString>

typedef std::vector<int> Mill, Neighbors;

class Board
{
public:
	Board(const QString& p = "xxxxxxxxxxxxxxxxxxxxxxx", QChar c = 'W', int d = 0) 
		: chessmen(p), color(c), depth(d) {}

	static void init();

	QChar   getSelfColor()     const { return color; }
	QChar   getOpponentColor() const { return flipColor(color); }
	int     getDepth()         const { return depth; }
	QString toString()         const { return chessmen; }
	bool    isEmpty (int pos)  const { return chessmen[pos] == 'x'; }
	QChar   getManAt(int pos)  const { return chessmen[pos]; }
	void    setManAt   (int pos, QChar man) { chessmen[pos] = man; }
	void    removeManAt(int pos)            { setManAt(pos, 'x'); }
	void    setString(const QString& str)   { chessmen = str; }
	
	bool closeMill(int pos) const;
	Board makeChild() const;  // a clone
	
	int countNumber(QChar color) const;    // # of chessmen
	int countJoints(QChar color) const;    // i.e. a cross has 4 joints
	int countTwoItemMills(QChar color) const;

	int findFirstAdded  (const Board& newBoard, QChar color, int start = 0);  // find difference
	int findFirstDeleted(const Board& newBoard, QChar color, int start = 0);

	static QChar     flipColor(QChar color) { return (color == 'W') ? 'B' : 'W'; }
	static Neighbors getNeighbors(int pos);
	static bool      isNeighbor(int lhs, int rhs);

private:
	bool isMill(const Mill& mill) const;
	int inTwoItemMill(int pos, QChar color) const;

private:
	QString chessmen;
	QChar   color;           // current color
	int     depth;           // current depth in the game tree

	static const int allMills[][3];
};

#endif // Board_h__
