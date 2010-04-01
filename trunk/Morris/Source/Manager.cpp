#include "Manager.h"
#include "Board.h"
#include "Morris.h"
#include "Estimator.h"
#include "MoveGenerator.h"
#include <vector>
#include <QtGui>

using namespace std;

vector<GameManager*> GameManager::managers;
GameManager*         GameManager::currentManager = 0;;
MainWnd*             GameManager::mainWnd = 0;
QChar                GameManager::currentColor = 'W';
Estimator*           GameManager::estimator = 0;
MorrisAlgorithm*     GameManager::algorithm = 0;

void GameManager::initManagers(MainWnd* wnd)
{
	mainWnd = wnd;
	managers.push_back(new SingleStepModeManager);
	managers.push_back(new PCPCModeManager      );
	managers.push_back(new PCHumanModeManager   );
	managers.push_back(new HumanHumanModeManager);
}

void GameManager::delManagers()
{
	for(vector<GameManager*>::iterator it = managers.begin(); it != managers.end(); ++it)
		delete *it;
	managers.clear();
}

GameManager* GameManager::resetManager() 
{
	if(currentManager != 0)
		currentManager->leaveThisMode();
	currentManager = managers[mainWnd->mode];
	currentManager->initMode();

	chooseAlgorithm();
	chooseEstimator();
	algorithm->setEstimator(estimator);

	return currentManager;
}

// 2 ugly simple factories
void GameManager::chooseAlgorithm()
{
	if(algorithm != 0)
		delete algorithm;
	if(mainWnd->algorithm == DlgSetting::MIN_MAX)
		algorithm = new MinMax;
	else if(mainWnd->algorithm == DlgSetting::ALPHA_BETA)
		algorithm = new AlphaBeta;
	else
		algorithm = new AlphaBetaImproved;
}

void GameManager::chooseEstimator()
{
	if(estimator != 0)
		delete estimator;
	if(mainWnd->estimation == DlgSetting::BASIC_ESTIMATION)
		estimator = new BasicEstimator;
	else
		estimator = new ImprovedEstimator;
}

void GameManager::initMode()
{
	mainWnd->setWindowTitle(tr("Morris - ") + getModeName());

	mainWnd->ui.mainToolBar->clear();
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionOpen);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionSave);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionRestart);
	initToolbar();
	mainWnd->ui.mainToolBar->addSeparator();
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionSetting);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionExit);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionAbout);

	Chessman::setManager(this);
	mainWnd->ui.leInput->clear();
	mainWnd->ui.leOutput->clear();
	mainWnd->ui.leEstimationCount->clear();
	mainWnd->ui.leEstimation->clear();
	mainWnd->ui.leDepth->clear();
	mainWnd->setStatus("xxxxxxxxxxxxxxxxxxxxxxx");
	mainWnd->ui.boardView->initChessmen();

	mainWnd->ui.boardView->setRemovable('W', false);
	mainWnd->ui.boardView->setRemovable('B', false);

	currentColor = mainWnd->startColor;
	mainWnd->ui.boardView->showCurrentColor(currentColor);

	initMovability();
	enterThisMode();

	openingPhase = true;
	gameOver = false;
}

void GameManager::initMovability()
{
	mainWnd->ui.boardView->setMovable('W', false);
	mainWnd->ui.boardView->setMovable('B', false);
}

void GameManager::setOpening(bool open)
{
	if(!open)
	{
		openingPhase = false;
		mainWnd->ui.boardView->removeAll(true);
		algorithm->clearDB();
	}
}

void GameManager::runAlgorithm()
{
	if(gameOver)
		return;
	runAlgorithm(isOpening());
	flipColor();
	checkWinning();
}

void GameManager::runAlgorithm(bool opening)
{
	QString input = mainWnd->ui.leStatus->text();
	QString output = algorithm->run(opening, input, currentColor, mainWnd->depth, mainWnd->timeLimit);
	mainWnd->ui.leOutput->setText(output);
	mainWnd->setStatus(output);
	mainWnd->ui.leEstimationCount->setText(tr("%1").arg(estimator->getCounter()));
	mainWnd->ui.leEstimation->setText(tr("%1").arg(algorithm->getMaxValue()));
	mainWnd->ui.leDepth->setText(tr("%1").arg(algorithm->getMaxDepth()));
}

void GameManager::flipColor()
{
	currentColor = Board::flipColor(currentColor);
	mainWnd->ui.boardView->showCurrentColor(currentColor);
}

void GameManager::lock(QChar color)
{
	if(currentColor == color)     // your turn, unlock idle for opening / all for game phase
		mainWnd->ui.boardView->setMovable(color, true, isOpening());
	else                          // otherwise, lock all of yours
		mainWnd->ui.boardView->setMovable(color, false, false);
}

void GameManager::lock()
{
	if(!gameOver)
	{
		lock('W');
		lock('B');
	}
}

void GameManager::onAdd(Chessman* man, int boardID)
{
	man->setMovable(false);
	onMove(boardID);		
}

void GameManager::onMove(int boardID)
{
	QString status = mainWnd->ui.leStatus->text();
	if(Board(status).closeMill(boardID))
	{
		flipColor();
		lock();
		mainWnd->ui.boardView->setRemovable(currentColor, true);
		if(mainWnd->ui.boardView->countRemovable(currentColor) == 0)
			runAlgorithm();
		return;
	}
	flipColor();
	runAlgorithm();
}

void GameManager::onRemove(Chessman* man) 
{
	if(checkWinning())
		return;
	mainWnd->ui.boardView->setRemovable(man->getColor(), false);
	runAlgorithm();
}

bool GameManager::checkWinning()
{
	if(isOpening())
		return false;
	int whiteCount = mainWnd->model.countNumber('W');
	int blackCount = mainWnd->model.countNumber('B');
	int whiteMoveCount = MoveGenerator::countMoves(mainWnd->model.toString(), isOpening(), 'W');
	int blackMoveCount = MoveGenerator::countMoves(mainWnd->model.toString(), isOpening(), 'B');
	if(whiteCount == 2 || blackCount == 2 || whiteMoveCount == 0 || blackMoveCount == 0)
	{
		gameOver = true;
		mainWnd->ui.boardView->setMovable('W', false, false);
		mainWnd->ui.boardView->setMovable('B', false, false);
		mainWnd->ui.boardView->setRemovable('W', false);
		mainWnd->ui.boardView->setRemovable('B', false);
		QMessageBox::information(mainWnd, tr("Game over"), 
			tr("%1 wins").arg(whiteCount == 2 || whiteMoveCount == 0 ? tr("Black") : tr("White")));
		return true;
	}
	return false;
}

bool GameManager::hoppable() const {
	return mainWnd->model.countNumber(currentColor) == 3;
}

void GameManager::setCurrentColor(QChar color)
{
	currentColor = color;
	lock();
}

void SingleStepModeManager::initToolbar()
{
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionOpening);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionGame);
}
void PCPCModeManager::initToolbar()
{
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionStep);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionRun);
}

void PCHumanModeManager::initToolbar() {
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionRun);
}


//////////////////////////////////////////////////////////////////////////
// Single Step
void SingleStepModeManager::opening() {
	runAlgorithm(true);
}

void SingleStepModeManager::game() {
	runAlgorithm(false);
}

void SingleStepModeManager::enterThisMode()
{
	mainWnd->ui.labelInput ->show();
	mainWnd->ui.leInput    ->show();
	mainWnd->ui.labelOutput->show();
	mainWnd->ui.leOutput   ->show();
	mainWnd->ui.labelStatus->hide();
	mainWnd->ui.leStatus   ->hide();
}

void SingleStepModeManager::leaveThisMode()
{
	mainWnd->ui.labelInput ->hide();
	mainWnd->ui.leInput    ->hide();
	mainWnd->ui.labelOutput->hide();
	mainWnd->ui.leOutput   ->hide();
	mainWnd->ui.labelStatus->show();
	mainWnd->ui.leStatus   ->show();
}

//////////////////////////////////////////////////////////////////////////
// PC - PC
void PCPCModeManager::run() 
{
	running = true;
	QTimer::singleShot(1000, this, SLOT(onTimer()));
}

void PCPCModeManager::onTimer() 
{
	if(!running)
		return;
	runAlgorithm();	
	QTimer::singleShot(1000, this, SLOT(onTimer()));
}

void PCPCModeManager::step() 
{
	running = false;
	runAlgorithm();
}

void PCPCModeManager::enterThisMode() {
	running = false;
}


//////////////////////////////////////////////////////////////////////////
// PC - Human
void PCHumanModeManager::initMovability()
{
	mainWnd->ui.boardView->setMovable('W', currentColor == 'W');
	mainWnd->ui.boardView->setMovable('B', false);
}

void PCHumanModeManager::run()
{
	if(mainWnd->startColor == 'B')
		runAlgorithm();
}

void PCHumanModeManager::runAlgorithm() {
	QTimer::singleShot(1000, this, SLOT(onTimer()));
}

void PCHumanModeManager::onTimer() 
{
	GameManager::runAlgorithm();
	lock();
}

//////////////////////////////////////////////////////////////////////////
// Human - Human
void HumanHumanModeManager::initMovability()
{
	mainWnd->ui.boardView->setMovable('W', mainWnd->startColor == 'W');
	mainWnd->ui.boardView->setMovable('B', mainWnd->startColor == 'B');
}

void HumanHumanModeManager::enterThisMode()
{
	mainWnd->ui.labelEstimationCount->hide();
	mainWnd->ui.leEstimationCount   ->hide();
	mainWnd->ui.labelEstimation     ->hide();
	mainWnd->ui.leEstimation        ->hide();
}

void HumanHumanModeManager::leaveThisMode()
{
	mainWnd->ui.labelEstimationCount->show();
	mainWnd->ui.leEstimationCount   ->show();
	mainWnd->ui.labelEstimation     ->show();
	mainWnd->ui.leEstimation        ->show();
}

void HumanHumanModeManager::runAlgorithm()
{
	lock();
	checkWinning();
}