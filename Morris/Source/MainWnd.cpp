#include "MainWnd.h"
#include "Morris.h"
#include "BoardView.h"
#include "Manager.h"
#include "MySetting.h"
#include <QtGui>

MainWnd::MainWnd(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	connect(ui.actionOpen,    SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSave,    SIGNAL(triggered()), this, SLOT(onSave()));
	connect(ui.actionRestart, SIGNAL(triggered()), this, SLOT(onRestart()));
	connect(ui.actionOpening, SIGNAL(triggered()), this, SLOT(onOpening()));
	connect(ui.actionGame,    SIGNAL(triggered()), this, SLOT(onGame()));
	connect(ui.actionStep,    SIGNAL(triggered()), this, SLOT(onStep()));
	connect(ui.actionRun,     SIGNAL(triggered()), this, SLOT(onRun()));
	connect(ui.actionSetting, SIGNAL(triggered()), this, SLOT(onSetting()));
	connect(ui.actionAbout,   SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui.leStatus,      SIGNAL(returnPressed()), this, SLOT(onStatusPressed()));
	connect(ui.leInput,       SIGNAL(returnPressed()), this, SLOT(onInputPressed()));
	connect(&model,  SIGNAL(statusChanged(QString)), this, SLOT(onModelChanged(QString)));

	ui.boardView->setModel(&model);
	ui.boardView->createBoard();

	GameManager::initManagers(this);
	loadSetting();
}

void MainWnd::onOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		".", tr("Text files (*.txt);;All files (*.*)"));
	if(fileName.isEmpty())
		return;

	loadSetting(fileName);
}

void MainWnd::onSave()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
		"./Output.txt",tr("Text files (*.txt);;All files (*.*)"));
	if(fileName.isEmpty())
		return;
	
	saveSetting(fileName);
}

void MainWnd::onSetting()
{
	DlgSetting dlg(this);
	dlg.setMode(mode);
	dlg.setStartColor(startColor);
	dlg.setAlgorithm(algorithm);
	dlg.setLimitBy(limitBy);
	dlg.setDepth(depth);
	dlg.setTimeLimit(timeLimit);
	dlg.setTableSize(memoryLimit);
	if(dlg.exec() == QDialog::Accepted)
	{
		mode        = dlg.getMode();
		startColor  = dlg.getStartColor();
		algorithm   = dlg.getAlgorithm();
		limitBy     = dlg.getLimitBy();
		depth       = dlg.getDepth();
		timeLimit   = dlg.getTimeLimit();
		memoryLimit = dlg.getTableSize();

		onRestart();
	}
}

MainWnd::~MainWnd() 
{
	GameManager::delManagers();
	MySetting<UserSetting>::destroySettingManager();
}

void MainWnd::onAbout() {
	QMessageBox::about(this, tr("About"), tr(
"<h2><b>Morris</b></h2> \
<p>Build 2010.4.26</p>   \
<p><a href=mailto:CongChenUTD@Gmail.com>CongChenUTD@Gmail.com</a></p> \
<h3>Rules</h3> \
<p>Opening phase: Players take turns placing pieces on any vacant board intersection spot until all pieces have been placed</p> \
<p>Midgame: Once all the pieces are placed, then take turns moving one piece along a board line to any adjacent vacant spot</p> \
<p>Endgame: When a player is down to only three game pieces, then he may move a piece to any open spot, not just an adjacent one (hopping)</p> \
<p>Mills: At any stage if you get three pieces in a row along the same straight board line, then you may remove one isolated opponent piece from play. An isolated piece is a piece that is not part of a mill.</p> \
The winner is the first player to reduce the opponent to only 2 tokens, or blocks the opponent from any further moves."
));
}

// model -> status
void MainWnd::onModelChanged(const QString& status) {
	ui.leStatus->setText(status);
}

// status -> model
void MainWnd::setStatus(const QString& status) 
{
	model.setStatus(status);
	ui.leStatus->setText(status);
}

void MainWnd::onStatusPressed() {
	setStatus(validateStatus(ui.leStatus->text()));
}

void MainWnd::onInputPressed() 
{
	ui.leInput->setText(validateStatus(ui.leInput->text()));
	setStatus(ui.leInput->text());
}

void MainWnd::onRestart() {
	manager = GameManager::reset();
}

void MainWnd::onOpening() 
{
	if(ui.leInput->text().isEmpty())
	{
		showNoInputError();
		return;
	}
	manager->opening();
}

void MainWnd::onGame() 
{
	if(ui.leInput->text().isEmpty())
	{
		showNoInputError();
		return;
	}
	manager->endOpening();
	manager->game();
}

void MainWnd::showNoInputError() {
	QMessageBox::critical(this, tr("No input"), tr("Load from file, or input directly and press enter"));
}

void MainWnd::onStep() {
	manager->step();
}

void MainWnd::onRun() {
	manager->run();
}

QString MainWnd::validateStatus(const QString& status) const
{
	QString result = status;
	if(result.length() > 23)
		result.truncate(23);
	while(result.length() < 23)
		result.append('x');	
	for(int i=0; i<23; ++i)
		if(result[i] != 'W' && result[i] != 'B' && result[i] != 'x')
			result[i] = 'x';
	return result;
}

void MainWnd::loadSetting(const QString& fileName)
{
	// load settings
	UserSetting* setting = MySetting<UserSetting>::getInstance(fileName);
	QString status = setting->value("Status").toString();
	bool opening = setting->value("Opening").toBool();
	mode = setting->getMode();
	startColor = setting->value("StartColor").toString().at(0);
	QChar currentColor = setting->value("CurrentColor").toString().at(0);
	algorithm = setting->getAlgorithm();
	limitBy = setting->getLimitBy();
	depth = setting->value("Depth").toInt();
	timeLimit = setting->value("TimeLimit").toInt();
	memoryLimit = setting->value("TableSize").toInt();

	// activate settings
	onRestart();
	if(fileName != "Global.ini")  // do not load last board
	{
		ui.leInput->setText(validateStatus(status));
		setStatus(status);
		if(!opening)
			manager->endOpening();	
	}
}

void MainWnd::saveSetting(const QString& fileName)
{
	UserSetting* setting = MySetting<UserSetting>::getInstance(fileName);
	setting->setValue("Status", ui.leStatus->text());
	setting->setValue("Opening", manager->isOpening());
	setting->setMode(mode);
	setting->setValue("StartColor", QString(startColor));
	setting->setValue("CurrentColor", QString(manager->getCurrentColor()));
	setting->setAlgorithm(algorithm);
	setting->setLimitBy(limitBy);
	setting->setValue("Depth", depth);
	setting->setValue("TimeLimit", timeLimit);
	setting->setValue("TableSize", memoryLimit);
}

void MainWnd::closeEvent(QCloseEvent*) {
	saveSetting();
}

void MainWnd::downIdle(QChar color)
{
	if(color == 'W')
		ui.sbIdleWhite->setValue(ui.sbIdleWhite->value() - 1);
	else
		ui.sbIdleBlack->setValue(ui.sbIdleBlack->value() - 1);
}
