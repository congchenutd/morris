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
	enum {LIMIT_BY_DEPTH, LIMIT_BY_TIME};

public:
	DlgSetting(QWidget *parent = 0);

	int  getMode() const;
	void setMode(int mode);

	QChar getStartColor() const;
	void  setStartColor(QChar color);

	int  getDepth() const    { return ui.sliderDepth->value();  }
	void setDepth(int depth);

	int getTimeLimit() const { return ui.sliderTimeLimit->value(); }
	void setTimeLimit(int seconds);

	int getAlgorithm() const;
	void setAlgorithm(int alg);

	int getEstimation() const;
	void setEstimation(int estimation);

	int getLimitBy() const;
	void setLimitBy(int limitBy);

	void setLanguage(const QString& language);

private slots:
	void onSingleMode();
	void onPCPCMode();
	void onPCHumanMode();
	void onHumanHumanMode();
	void onDepthChanged    (int depth);
	void onTimeLimitChanged(int seconds);
	void onLanguageChanged (int index);
	void onAlgorithmChanged();
	void onEstimationChanged();

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
	void setLimitBy   (int limitBy);
	int getMode()       const;
	int getAlgorithm()  const;
	int getEstimation() const;
	int getLimitBy()    const;

private:
	virtual void loadDefaults();

private:
	QStringList modeNames;
	QStringList algorithmNames;
	QStringList estimationNames;
	QStringList limitByNames;
};

#endif // DLGSETTING_H
