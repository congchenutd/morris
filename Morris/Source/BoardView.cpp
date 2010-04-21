#include "BoardView.h"
#include "Board.h"
#include "Manager.h"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// Model
bool BoardModel::isEmpty(const QPoint& pos) const {
	return board.isEmpty(view->posToID(pos));
}

void BoardModel::add(const Chessman* man, const QPoint& to)
{
	int id = view->posToID(to);
	if(id > -1)
	{
		board.setManAt(view->posToID(to), man->getColor());
		emit statusChanged(board.toString());
	}
}

void BoardModel::move(const Chessman* man, const QPoint& from, const QPoint& to)
{
	remove(from);
	add(man, to);
}

void BoardModel::remove(const QPoint& from) 
{
	int id = view->posToID(from);
	if(id > -1)
	{
		board.removeManAt(id);
		emit statusChanged(board.toString());
	}
}

// mainwindow -> model -> view
void BoardModel::setStatus(const QString& s)
{
	QString newStatus = s;
	while(newStatus.length() < 23)
		newStatus.append('x');

	updateView(newStatus, 'W');
	updateView(newStatus, 'B');
	board.setString(newStatus);
}

void BoardModel::updateView(const QString& newStatus, QChar color)
{
	int oldCount = board.countNumber(color);
	int newCount = newStatus.count(color);

	if(newCount > oldCount)       // add
	{
		for(int i = board.findFirstAdded(newStatus, color, 0); i > -1; 
			    i = board.findFirstAdded(newStatus, color, i+1))
			view->add(i, color);
	}
	else if(newCount < oldCount)  // remove
	{
		for(int i = board.findFirstDeleted(newStatus, color, 0); i > -1; 
			    i = board.findFirstDeleted(newStatus, color, i+1))
			view->remove(i);
	}
	else                          // move, assuming only 1 move per color
	{
		int from = board.findFirstDeleted(newStatus, color);
		int to   = board.findFirstAdded  (newStatus, color);
		if(from != -1 && to != -1)
			view->move(from, to);
	}
}

//////////////////////////////////////////////////////////////////////////
// View
BoardView::BoardView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setMinimumSize(SCENE_WIDTH+4, 7*GRID+4);
	ui.graphicsView->setScene(&scene);
	ui.graphicsView->setRenderHints(QPainter::Antialiasing);
	initPositions();
}

void BoardView::setModel(BoardModel* m) {
	model = m;
	model->setView(this);
}

void BoardView::initPositions()
{
	boardPositions.push_back(QPoint(BOARD_LEFT,     6*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+3*GRID, 6*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+6*GRID, 6*GRID));	
	boardPositions.push_back(QPoint(BOARD_LEFT+1*GRID, 5*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+3*GRID, 5*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+5*GRID, 5*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+2*GRID, 4*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+4*GRID, 4*GRID));	
	boardPositions.push_back(QPoint(BOARD_LEFT,     3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+1*GRID, 3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+2*GRID, 3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+4*GRID, 3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+5*GRID, 3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+6*GRID, 3*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+2*GRID, 2*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+3*GRID, 2*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+4*GRID, 2*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+1*GRID, 1*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+3*GRID, 1*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT+5*GRID, 1*GRID));
	boardPositions.push_back(QPoint(BOARD_LEFT,     0));
	boardPositions.push_back(QPoint(BOARD_LEFT+3*GRID, 0));
	boardPositions.push_back(QPoint(BOARD_LEFT+6*GRID, 0));
}

QPoint BoardView::getClosestPosition(const Chessman& chessman) const
{
	vector<QPoint>::const_iterator it, closest;
	int minDistance = INT_MAX;
	for(it = boardPositions.begin(); it != boardPositions.end(); ++it)
	{
		int d = chessman.distance(*it);
		if(d < minDistance)
		{
			minDistance = d;
			closest = it;
		}
	}
	return *closest;
}

int BoardView::posToID(const QPointF& posF) const
{
	const QPoint pos = posF.toPoint();
	vector<QPoint>::const_iterator it;
	for(it = boardPositions.begin(); it != boardPositions.end(); ++it)
		if(pos == *it)
			return it - boardPositions.begin();
	return -1;
}

QPoint BoardView::idToPos(int id) const {
	return boardPositions[id];
}

void BoardView::createBoard()
{
	scene.setSceneRect(0, -GRID/2, SCENE_WIDTH, 7*GRID);

	QPixmap background("paper.jpg");
	QGraphicsPixmapItem* pix = new QGraphicsPixmapItem(background.scaled(SCENE_WIDTH, 7*GRID+10));
	pix->setPos(0, -GRID/2);
	scene.addItem(pix);

	scene.addRect(BOARD_LEFT,        0,      6*GRID, 6*GRID, QPen(Qt::black, 5));
	scene.addRect(BOARD_LEFT+1*GRID, 1*GRID, 4*GRID, 4*GRID, QPen(Qt::black, 3));
	scene.addRect(BOARD_LEFT+2*GRID, 2*GRID, 2*GRID, 2*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+0,      0,      BOARD_LEFT+2*GRID, 2*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+0,      6*GRID, BOARD_LEFT+2*GRID, 4*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+6*GRID, 0,      BOARD_LEFT+4*GRID, 2*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+6*GRID, 6*GRID, BOARD_LEFT+4*GRID, 4*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+0,      3*GRID, BOARD_LEFT+2*GRID, 3*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+4*GRID, 3*GRID, BOARD_LEFT+6*GRID, 3*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+3*GRID, 0,      BOARD_LEFT+3*GRID, 2*GRID, QPen(Qt::black, 3));
	scene.addLine(BOARD_LEFT+3*GRID, 5*GRID, BOARD_LEFT+3*GRID, 6*GRID, QPen(Qt::black, 3));
	
	currentColorLabel.setPos(0, -40);
	currentColorLabel.setFont(QFont(tr("Arial"), 16));
	currentColorLabel.setFlag(QGraphicsItem::ItemIsMovable, false);
	scene.addItem(&currentColorLabel);
}

void BoardView::initChessmen()
{
	removeAll();

	// create chessmen
	for(int i=0; i<9; ++i)
	{
		Chessman* man = new Chessman(i, model, this, Chessman::WhiteHeaven, 'W');
		scene.addItem(man);
		man->moveTo(QPoint(BOARD_LEFT-GRID, Chessman::DIAMETER/2+i*(GRID*6/9)), 1000);
		chessmenWhite.insert(make_pair(i, man));
	}
	for(int i=0; i<9; ++i)
	{
		Chessman* man = new Chessman(i, model, this, Chessman::BlackHeaven, 'B');
		scene.addItem(man);
		man->moveTo(QPoint(BOARD_LEFT+BOARD_WIDTH+GRID, Chessman::DIAMETER/2+i*(GRID*6/9)), 1000);
		chessmenBlack.insert(make_pair(i, man));
	}
	Chessman::resetIdleCount();
}

void BoardView::add(int boardID, QChar color)
{
	Chessman* man = findIdleMan(color);
	if(man != 0)
		man->add(idToPos(boardID));
}

Chessman* BoardView::findIdleMan(QChar color) const
{
	const Chessmen& chessmen = (color == 'W') ? chessmenWhite : chessmenBlack;
	for(Chessmen::const_iterator it = chessmen.begin(); it != chessmen.end(); ++it)
		if(it->second->isIdle())
			return it->second;
	return 0;
}

// called by model
void BoardView::remove(int boardID)
{
	Chessman* man = dynamic_cast<Chessman*>(scene.itemAt(idToPos(boardID)));
	if(man != 0)
		man->remove();
}

// called by chessman
void BoardView::remove(const Chessman* man)
{
	if(man->getColor() == 'W')
		chessmenWhite.erase(man->getID());
	else
		chessmenBlack.erase(man->getID());
}

void BoardView::move(int from, int to)
{
	Chessman* man = dynamic_cast<Chessman*>(scene.itemAt(idToPos(from)));
	if(man != 0)
		man->moveTo(idToPos(to));
}

void BoardView::setMovable(QChar color, bool movable, bool onlyIdle)
{
	const Chessmen& chessmen = (color == 'W') ? chessmenWhite : chessmenBlack;
	for(Chessmen::const_iterator it = chessmen.begin(); it != chessmen.end(); ++it)
	{
		if(onlyIdle && it->second != 0 && !it->second->isIdle())
			continue;
		it->second->setMovable(movable);
	}
}

void BoardView::setRemovable(QChar color, bool removable)
{
	const Chessmen& chessmen = (color == 'W') ? chessmenWhite : chessmenBlack;
	for(Chessmen::const_iterator it = chessmen.begin(); it != chessmen.end(); ++it)
		if(it->second != 0 && !it->second->isIdle())
			it->second->setRemovable(removable);
}

int BoardView::countRemovable(QChar color) const
{
	int result = 0;
	const Chessmen& chessmen = (color == 'W') ? chessmenWhite : chessmenBlack;
	for(Chessmen::const_iterator it = chessmen.begin(); it != chessmen.end(); ++it)
		if(it->second != 0 && !it->second->isIdle() && it->second->isRemovable())
			result ++;
	return result;
}

void BoardView::showCurrentColor(QChar color) {
	currentColorLabel.setPlainText(tr("%1's turn").arg(color == 'W' ? tr("White") : tr("Black")));
}

void BoardView::removeAll(bool idleOnly)
{
	for(int i=0; i<9; ++i)
	{
		Chessmen::iterator it = chessmenWhite.find(i);
		if(it == chessmenWhite.end())
			continue;
		if(idleOnly && !it->second->isIdle())
			continue;
		it->second->remove();
	}
	for(int i=0; i<9; ++i)
	{
		Chessmen::iterator it = chessmenBlack.find(i);
		if(it == chessmenBlack.end())
			continue;
		if(idleOnly && !it->second->isIdle())
			continue;
		it->second->remove();
	}
}


//////////////////////////////////////////////////////////////////////////
// Chessman
const QPoint Chessman::WhiteHeaven = QPoint(2000, 3*BoardView::GRID);
const QPoint Chessman::BlackHeaven = QPoint(-1000, 3*BoardView::GRID);
GameManager* Chessman::manager = 0;
int          Chessman::idleCount = 18;

Chessman::Chessman(int i, BoardModel* m, BoardView* v, QPoint pos, QChar c, QGraphicsItem* parent) 
: QGraphicsItem(parent)
{
	id        = i;
	model     = m;
	view      = v;
	idle      = true;
	removable = false;
	color     = c;
	pixmap = (color == 'W' ? QPixmap(":/MainWnd/Images/White.png").scaled(DIAMETER, DIAMETER) : 
							 QPixmap(":/MainWnd/Images/Black.png").scaled(DIAMETER, DIAMETER));
	setPos(pos);
	setMovable(true);
	setAcceptHoverEvents(true);

	timeLine.setFrameRange(0, 100);
	animation.setItem(this);
	animation.setTimeLine(&timeLine);
}

QRectF Chessman::boundingRect() const {
	return QRectF(-DIAMETER/2, -DIAMETER/2, DIAMETER, DIAMETER);
}

void Chessman::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->drawPixmap(-DIAMETER/2, -DIAMETER/2, DIAMETER, DIAMETER, pixmap);
	if(removable)
	{
		painter->setPen(Qt::red);
		const int w = painter->fontMetrics().width ('X');
		const int h = painter->fontMetrics().xHeight();
		painter->drawText(-w/2, h, "X");
	}
}

bool Chessman::isMovable() const {
	return flags() & QGraphicsItem::ItemIsMovable;
}

void Chessman::mousePressEvent(QGraphicsSceneMouseEvent*) {
	if(removable)
		onRemove();
}

void Chessman::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
	setCursor(removable ? Qt::PointingHandCursor : Qt::OpenHandCursor);
}

void Chessman::mouseMoveEvent(QGraphicsSceneMouseEvent* event) 
{
	if(removable)
		return;
	if(!isMovable())
		return;
	setCursor(Qt::ClosedHandCursor);
	QGraphicsItem::mouseMoveEvent(event);
}

void Chessman::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(removable)
		return;
	if(!isMovable())
		return;
	if(event->button() != Qt::LeftButton)
		return;
	setCursor(Qt::OpenHandCursor);

	QPoint target = view->getClosestPosition(*this);
	if(distance(target) < 10000 && model->isEmpty(target))  // closest valid position
	{
		if(!onBoard(lastPosition))  // outside -> inside, opening phase
			add(target, true);            
		else                        // inside -> inside, game phase
		{
			if(Board::isNeighbor(view->posToID(lastPosition), view->posToID(target)))
				move(target, true);
			else if(manager->hoppable())
				move(target, true);
			else
				moveTo(lastPosition);       // move back
		}
	}
	else
		moveTo(lastPosition);       // move back

	QGraphicsItem::mouseReleaseEvent(event);
}

void Chessman::add(const QPoint& target, bool byHuman) 
{
	setIdle();
	if(byHuman)
	{
		model->add(this, target);       // update model
		manager->onAdd(this, view->posToID(target));		
	}
	moveTo(target);
}

void Chessman::move(const QPoint& target, bool byHuman)
{
	if(byHuman)
	{
		model->move(this, lastPosition, target);       // update model
		manager->onMove(view->posToID(target));
	}
	moveTo(target);
}

void Chessman::remove(bool byHuman)
{
	view->remove(this);
	if(byHuman)
	{
		model->remove(pos().toPoint());
		manager->onRemove(this);
	}
	connect(&timeLine, SIGNAL(finished()), this, SLOT(onDead()));
	moveTo((getColor() == 'W') ? WhiteHeaven : BlackHeaven, 1000);
}

void Chessman::moveTo(const QPoint& target, int duration)
{
	timeLine.setDuration(duration);
	animation.clear();
	animation.setPosAt(0, pos());
	animation.setPosAt(1, target);
	timeLine.start();
	lastPosition = target;
}

void Chessman::onDead()
{
	scene()->removeItem(this);
	deleteLater();
}

void Chessman::onRemove() {
	remove(true);
}

int Chessman::distance(const QPoint& other) const {
	return (x()-other.x())*(x()-other.x()) + (y()-other.y())*(y()-other.y());
}

bool Chessman::onBoard(const QPoint& target) {
	return target.x() >= BoardView::BOARD_LEFT && 
		   target.x() <= BoardView::BOARD_LEFT+6*BoardView::GRID;
}

void Chessman::setIdle()
{
	idle = false;
	idleCount --;
	if(idleCount == 0)
		manager->endOpening();
}

void Chessman::setRemovable(bool enable) 
{
	bool m = model->getBoard().closeMill(view->posToID(pos()));
	removable = enable && !m;
	update();
}
