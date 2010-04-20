#ifndef Manager_h__
#define Manager_h__

#include "MainWnd.h"
#include <QObject>
#include <QTimer>
#include <QTime>

class MorrisAlgorithm;
class Estimator;
class GameManager : public QObject
{
public:
	static void initManagers(MainWnd* wnd);
	static void delManagers();
	static GameManager* reset();

	virtual ~GameManager() {}
	virtual void opening() {}                            // for single step only
	virtual void game()    {}                            // for single step only
	
	virtual void run()     {}                            // for PCPC only
	virtual void step()    {}                            // for PCPC only
	
	virtual void onAdd   (Chessman* man, int boardID);   // for human mode
	virtual void onMove  (int boardID);
	virtual void onRemove(Chessman* man);

	bool hoppable() const;
	bool isOpening() const { return openingPhase; }
	void endOpening();
	void setCurrentColor(QChar color);
	QChar getCurrentColor() const { return currentColor; }

protected:
	virtual QString getModeName() const = 0;
	virtual void initToolbar  () {}
	virtual void enterThisMode() {}
	virtual void initMovability();
	virtual void runAlgorithm();

	void showStatusWidgets(bool input, bool output, bool estNum, bool est, bool depth, bool status);
	void runAlgorithm(bool opening);
	void flipColor();
	bool checkWinning();
	void lock();
	void lock(QChar color);
	void showCurrentColor();

private:
	void initMode();
	static void chooseAlgorithm();

protected:
	static MainWnd*         mainWnd;
	static MorrisAlgorithm* algorithm;
	static Estimator*       estimator;
	static QChar            currentColor;
	bool openingPhase;
	bool gameOver;
	QTime time;

private:
	static std::vector<GameManager*> managers;
	static GameManager*              currentManager;
	static std::vector<MorrisAlgorithm*> algorithms;
	static std::vector<Estimator*>       estimators;
};

class SingleStepModeManager : public GameManager
{
protected:
	virtual QString getModeName() const { return tr("Single Step Mode"); }
	virtual void initToolbar();
	virtual void opening();
	virtual void game();
	virtual void enterThisMode();
};

class PCPCModeManager : public GameManager
{
	Q_OBJECT

public:
	virtual void run();

protected:
	virtual QString getModeName() const { return tr("PC-PC Mode"); }
	virtual void initToolbar();
	virtual void step();
	virtual void enterThisMode();

private slots:
	void onTimer();

private:
	bool running;
};

class PCHumanModeManager : public GameManager
{
	Q_OBJECT

public:
	virtual void run();

protected:
	virtual QString getModeName() const { return tr("PC-Human Mode"); }
	virtual void initToolbar();
	virtual void initMovability();
	virtual void enterThisMode();
	virtual void runAlgorithm();

private slots:
	void onTimer();
};

class HumanHumanModeManager : public GameManager
{
protected:
	virtual QString getModeName() const { return tr("Human-Human Mode"); }
	virtual void initMovability();
	virtual void enterThisMode();
	virtual void runAlgorithm();
};

#endif // Manager_h__
