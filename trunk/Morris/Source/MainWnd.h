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

private:
	Ui::MainWndClass ui;

	int mode;
	QChar startColor;
	int depth;
	int algorithm;
	int estimation;

	BoardModel model;
	GameManager* manager;
};


#endif // MAINWND_H
