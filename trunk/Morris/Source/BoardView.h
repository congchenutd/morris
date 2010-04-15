#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsItem>
#include <map>
#include <vector>
#include "ui_BoardView.h"
#include "Board.h"

class BoardView;
class Chessman;
class BoardModel : public QObject
{
	Q_OBJECT

public:
	void setView(BoardView* v) { view = v; }

	// mainwindow -> model -> view -> chessman
	void setStatus(const QString& newStatus);
	QString toString() const { return board.toString(); }

	// chessman -> model -> mainwindow
	void add   (const Chessman* man, const QPoint& to);
	void move  (const Chessman* man, const QPoint& from, const QPoint& to);
	void remove(const QPoint& from);

	bool isEmpty(const QPoint& pos) const;
	Board getBoard() const { return board; }

private:
	void updateView(const QString& newStatus, QChar color);

signals:
	void statusChanged(const QString& s);    // to mainwindow

private:
	Board      board;
	BoardView* view;
};


class BoardView : public QWidget
{
	Q_OBJECT
		
	typedef std::map<int, Chessman*> Chessmen;

public:
	BoardView(QWidget *parent = 0);
	void setModel(BoardModel* m);
	void createBoard();
	void initChessmen();

	void add   (int boardID, QChar color);
	void remove(int boardID);                // called by model
	void remove(const Chessman* man);        // called by chessman
	void move  (int from, int to);
	void removeAll(bool idleOnly = false);

	void setMovable  (QChar color, bool movable, bool onlyIdle = false);
	void setRemovable(QChar color, bool removable);
	int  countRemovable(QChar color) const;

	QPoint getClosestPosition(const Chessman& chessman) const;
	int    posToID(const QPointF& posF) const;
	QPoint idToPos(int id) const;

	void showCurrentColor(QChar color);

private:
	void initPositions();
	Chessman* findIdleMan(QChar color) const;

public:
	enum {GRID = 80, BOARD_LEFT = GRID*3, BOARD_WIDTH = 6*GRID, SCENE_WIDTH = 12*GRID};

private:
	Ui::BoardViewClass ui;
	std::vector<QPoint> boardPositions;

	QGraphicsScene scene;
	BoardModel* model;
	Chessmen chessmenWhite;
	Chessmen chessmenBlack;
	QGraphicsTextItem currentColorLabel;
};

class GameManager;
class Chessman : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	Chessman(int i, BoardModel* m, BoardView* v, QPoint pos, QChar c, QGraphicsItem* parent = 0);
	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

	int   getID()       const { return id;        }
	QChar getColor()    const { return color;     }
	bool  isIdle()      const { return idle;      }
	int   isRemovable() const { return removable; }

	int  distance(const QPoint& other) const;
	void setIdle();
	bool isMovable() const;
	void setMovable(bool movable) { setFlag(QGraphicsItem::ItemIsMovable, movable); }
	void setRemovable(bool enable);

	void add (const QPoint& target, bool byHuman = false);
	void move(const QPoint& target, bool byHuman = false);
	void remove(bool byHuman = false);
	void moveTo(const QPoint& target, int duration = 500);

	static void setManager(GameManager* m) { manager = m;    }
	static void resetIdleCount()           { idleCount = 18; }

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*) { unsetCursor(); }
	virtual void mouseMoveEvent   (QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	virtual void mousePressEvent  (QGraphicsSceneMouseEvent* event);

private slots:
	void onDead();
	void onRemove();

private:
	static bool onBoard(const QPoint& target);

public:
	const static QPoint WhiteHeaven, BlackHeaven;
	enum {DIAMETER = 50};

private:
	static GameManager* manager;
	static int idleCount;
	BoardModel* model;
	BoardView* view;

	QPoint lastPosition;
	QTimeLine timeLine;
	QGraphicsItemAnimation animation;
	QChar  color;
	int    id;           // 0~8
	bool   idle;
	bool   removable;
	QPixmap pixmap;
};


#endif // BOARDVIEW_H
