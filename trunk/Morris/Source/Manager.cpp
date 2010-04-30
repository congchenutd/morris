#include "Manager.h"
#include "Board.h"
#include "Morris.h"
#include "Estimator.h"
#include "MoveGenerator.h"
#include <vector>
#include <QtGui>

using namespace std;

vector<GameManager*>     GameManager::managers;
vector<MorrisAlgorithm*> GameManager::algorithms;
vector<Estimator*>       GameManager::estimators;

GameManager*     GameManager::currentManager = 0;;
MainWnd*         GameManager::mainWnd = 0;
QChar            GameManager::currentColor = 'W';
Estimator*       GameManager::estimator = 0;
MorrisAlgorithm* GameManager::algorithm = 0;

void GameManager::initManagers(MainWnd* wnd)
{
	mainWnd = wnd;
	managers.push_back(new SingleStepModeManager);
	managers.push_back(new PCPCModeManager      );
	managers.push_back(new PCHumanModeManager   );
	managers.push_back(new HumanHumanModeManager);

	algorithms.push_back(new MinMax);
	algorithms.push_back(new AlphaBeta);
	algorithms.push_back(new NegaMax);

	estimators.push_back(new BasicEstimator);
	estimators.push_back(new ImprovedEstimator);
}

void GameManager::delManagers()
{
	for(vector<GameManager*>::iterator it = managers.begin(); it != managers.end(); ++it)
		delete *it;
	managers.clear();
	for(vector<MorrisAlgorithm*>::iterator it = algorithms.begin(); it != algorithms.end(); ++it)
		delete *it;
	algorithms.clear();
	for(vector<Estimator*>::iterator it = estimators.begin(); it != estimators.end(); ++it)
		delete *it;
	estimators.clear();
}

GameManager* GameManager::reset() 
{
	currentManager = managers[mainWnd->mode];
	currentManager->initMode();

	chooseAlgorithm();
	return currentManager;
}

// ugly simple factory
void GameManager::chooseAlgorithm()
{
	algorithm = algorithms[mainWnd->algorithm];
	if(mainWnd->algorithm == DlgSetting::ALPHA_BETA_IMPROVED)
	{
		estimator = estimators[1];
		algorithm->setMemoryLimit(mainWnd->memoryLimit);
		algorithm->loadDB();
	}
	else
		estimator = estimators[0];

	algorithm->setEstimator(estimator);
}

void GameManager::initMode()
{
	// title
	mainWnd->setWindowTitle(tr("Morris - ") + getModeName());

	// tool bar
	mainWnd->ui.mainToolBar->clear();
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionOpen);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionSave);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionRestart);
	initToolbar();
	mainWnd->ui.mainToolBar->addSeparator();
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionSetting);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionExit);
	mainWnd->ui.mainToolBar->addAction(mainWnd->ui.actionAbout);
	
	// status bar
	mainWnd->ui.leInput->clear();
	mainWnd->setStatus("xxxxxxxxxxxxxxxxxxxxxxx");
	mainWnd->ui.leEstimationCount->clear();
	mainWnd->ui.leEstimation->clear();
	mainWnd->ui.leDepth->clear();
	mainWnd->ui.sbIdleWhite->setValue(9);
	mainWnd->ui.sbIdleBlack->setValue(9);

	// chessmen, view
	Chessman::setManager(this);
	mainWnd->ui.boardView->initChessmen();
	mainWnd->ui.boardView->setRemovable('W', false);
	mainWnd->ui.boardView->setRemovable('B', false);

	currentColor = mainWnd->startColor;
	showCurrentColor();

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

void GameManager::endOpening()
{
	openingPhase = false;
	mainWnd->ui.boardView->removeAll(true);
	algorithm->endOpening();
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
	mainWnd->setEnabled(false);  // freeze GUI
	qApp->processEvents();
	QString output = algorithm->run(opening, mainWnd->model.toString(), currentColor, 
		mainWnd->limitBy, mainWnd->depth, mainWnd->timeLimit, 
		mainWnd->ui.sbIdleWhite->value(), mainWnd->ui.sbIdleBlack->value());
	mainWnd->setEnabled(true);
	mainWnd->downIdle(currentColor);

	mainWnd->setStatus(output);
	mainWnd->ui.leEstimationCount->setText(tr("%1").arg(estimator->getCounter()));
	mainWnd->ui.leEstimation     ->setText(tr("%1").arg(algorithm->getMaxValue()));
	mainWnd->ui.leDepth          ->setText(tr("%1").arg(algorithm->getMaxDepth()));
}

void GameManager::flipColor()
{
	currentColor = Board::flipColor(currentColor);
	showCurrentColor();
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
	mainWnd->downIdle(man->getColor());
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
	int whiteCount = mainWnd->model.getBoard().countNumber('W');
	int blackCount = mainWnd->model.getBoard().countNumber('B');
	int whiteMoveCount = mainWnd->model.getBoard().countMoves(isOpening(), 'W');
	int blackMoveCount = mainWnd->model.getBoard().countMoves(isOpening(), 'B');
	if(whiteCount == 2 || blackCount == 2 || whiteMoveCount == 0 || blackMoveCount == 0)
	{
		gameOver = true;
		mainWnd->ui.boardView->setMovable('W', false, false);  // freeze all
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
	return mainWnd->model.getBoard().countNumber(currentColor) == 3;
}

void GameManager::setCurrentColor(QChar color)
{
	currentColor = color;
	lock();
}

void GameManager::showStatusWidgets(bool input, bool status, bool estNum, 
									bool est,   bool depth)
{
	mainWnd->ui.labelInput          ->setVisible(input);
	mainWnd->ui.leInput             ->setVisible(input);
	mainWnd->ui.labelEstimationCount->setVisible(estNum);
	mainWnd->ui.leEstimationCount   ->setVisible(estNum);
	mainWnd->ui.labelEstimation     ->setVisible(est);
	mainWnd->ui.leEstimation        ->setVisible(est);
	mainWnd->ui.labelDepth          ->setVisible(depth);
	mainWnd->ui.leDepth             ->setVisible(depth);
	mainWnd->ui.labelStatus         ->setVisible(status);
	mainWnd->ui.leStatus            ->setVisible(status);
}

void GameManager::showCurrentColor() {
	mainWnd->ui.boardView->showCurrentColor(currentColor);
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

void SingleStepModeManager::enterThisMode() {
	showStatusWidgets(true, true, true, true, true);
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

void PCPCModeManager::enterThisMode() 
{
	showStatusWidgets(false, true, true, true, true);
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

void PCHumanModeManager::enterThisMode() {
	showStatusWidgets(false, true, true, true, true);
}

//////////////////////////////////////////////////////////////////////////
// Human - Human
void HumanHumanModeManager::initMovability()
{
	mainWnd->ui.boardView->setMovable('W', mainWnd->startColor == 'W');
	mainWnd->ui.boardView->setMovable('B', mainWnd->startColor == 'B');
}

void HumanHumanModeManager::enterThisMode() {
	showStatusWidgets(false, true, false, false, false);
}

void HumanHumanModeManager::runAlgorithm()
{
	lock();
	checkWinning();
}