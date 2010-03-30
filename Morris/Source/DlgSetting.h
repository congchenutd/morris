#ifndef DLGSETTING_H
#define DLGSETTING_H

#include <QDialog>
#include "ui_DlgSetting.h"
#include "MySetting.h"

class DlgSetting : public QDialog
{
	Q_OBJECT

public:
	enum {STEP_MODE, PC_PC_MODE, PC_HUMAN_MODE, HUMAN_HUMAN_MODE};
	enum {MIN_MAX, ALPHA_BETA, ALPHA_BETA_IMPROVED};
	enum {BASIC_ESTIMATION, IMPROVED_ESTIMATION};

public:
	DlgSetting(QWidget *parent = 0);

	int  getMode() const;
	void setMode(int mode);

	QChar getStartColor() const;
	void  setStartColor(QChar color);

	int  getDepth() const    { return ui.sliderDepth->value();  }
	void setDepth(int depth);

	int getAlgorithm() const;
	void setAlgorithm(int alg);

	int getEstimation() const;
	void setEstimation(int estimation);

	void setLanguage(const QString& language);

private slots:
	void onSingleMode();
	void onPCPCMode();
	void onPCHumanMode();
	void onHumanHumanMode();
	void onDepthChanged   (int depth);
	void onLanguageChanged(int index);

private:
	void setPCEnabled(bool enable);

private:
	Ui::DlgSettingClass ui;
};

class UserSetting : public MySetting<UserSetting>
{
public:
	UserSetting(const QString& userName);

	void setMode      (int mode);
	void setAlgorithm (int alg);
	void setEstimation(int estimation);
	int getMode()       const;
	int getAlgorithm()  const;
	int getEstimation() const;

private:
	virtual void loadDefaults();

private:
	QStringList modeNames;
	QStringList algorithmNames;
	QStringList estimationNames;
};

#endif // DLGSETTING_H
