#ifndef MAINWND_H
#define MAINWND_H

#include <QtGui/QMainWindow>
#include "ui_MainWnd.h"
#include "DlgSetting.h"
#include "BoardView.h"

class MorrisAlgorithm;
class GameManager;

class MainWnd : public QMainWindow
{
	Q_OBJECT

	friend class GameManager;
	friend class SingleStepModeManager;
	friend class PCPCModeManager;
	friend class PCHumanModeManager;
	friend class HumanHumanModeManager;

public:
	MainWnd(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWnd();
	void setStatus(const QString& status);
	void downIdle(QChar color);

protected:
	virtual void closeEvent(QCloseEvent*);

private slots:
	void onOpen();
	void onSave();
	void onOpening();
	void onGame();
	void onSetting();
	void onAbout();
	void onStatusPressed();
	void onInputPressed();
	void onModelChanged(const QString& status);
	void onRestart();
	void onStep();
	void onRun();

private:
	QString validateStatus(const QString& status) const;
	void showNoInputError();
	void loadSetting(const QString& fileName = "Global.ini");
	void saveSetting(const QString& fileName = "Global.ini");

private:
	Ui::MainWndClass ui;

	int mode;
	QChar startColor;
	int algorithm;
	int limitBy;
	int depth;
	int timeLimit;
	int memoryLimit;

	BoardModel model;
	GameManager* manager;
};


#endif // MAINWND_H
