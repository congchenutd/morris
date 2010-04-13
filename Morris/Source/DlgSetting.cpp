#include "DlgSetting.h"
#include <QTranslator>
#include <QMessageBox>

DlgSetting::DlgSetting(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	UserSetting* setting = MySetting<UserSetting>::getInstance();
	setLanguage(setting->value("Language").toString());

	connect(ui.radioStep,        SIGNAL(clicked()), this, SLOT(onSingleMode()));
	connect(ui.radioPC2PC,       SIGNAL(clicked()), this, SLOT(onPCPCMode()));
	connect(ui.radioPC2Human,    SIGNAL(clicked()), this, SLOT(onPCHumanMode()));
	connect(ui.radioHuman2Human, SIGNAL(clicked()), this, SLOT(onHumanHumanMode()));
	connect(ui.sliderDepth,     SIGNAL(valueChanged       (int)), this, SLOT(onDepthChanged    (int)));
	connect(ui.sliderTimeLimit, SIGNAL(valueChanged       (int)), this, SLOT(onTimeLimitChanged(int)));
	connect(ui.cbLanguage,      SIGNAL(currentIndexChanged(int)), this, SLOT(onLanguageChanged (int)));
	connect(ui.radioMinMax,            SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
	connect(ui.radioAlphaBeta,         SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
	connect(ui.radioAlphaBetaImproved, SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
	connect(ui.radioBasicEstimation,    SIGNAL(clicked()), this, SLOT(onEstimationChanged()));
	connect(ui.radioImprovedEstimation, SIGNAL(clicked()), this, SLOT(onEstimationChanged()));
}

int DlgSetting::getMode() const
{
	if(ui.radioStep->isChecked())
		return STEP_MODE;
	if(ui.radioPC2PC->isChecked())
		return PC_PC_MODE;
	if(ui.radioPC2Human->isChecked())
		return PC_HUMAN_MODE;
	return HUMAN_HUMAN_MODE;
}

void DlgSetting::setMode(int mode)
{
	switch(mode)
	{
	case PC_PC_MODE:
		ui.radioPC2PC->setChecked(true);
		onPCPCMode();
		break;
	case PC_HUMAN_MODE:
		ui.radioPC2Human->setChecked(true);
		onPCHumanMode();
		break;
	case HUMAN_HUMAN_MODE:
		ui.radioHuman2Human->setChecked(true);
		onHumanHumanMode();
		break;
	default:
		ui.radioStep->setChecked(true);
		onSingleMode();
		break;
	}
}

QChar DlgSetting::getStartColor() const {
	return ui.radioWhite->isChecked() ? 'W' : 'B';
}

void DlgSetting::setStartColor(QChar color)
{
	if(color == 'W')
		ui.radioWhite->setChecked(true);
	else
		ui.radioBlack->setChecked(true);
}

void DlgSetting::onSingleMode()
{
	ui.radioWhite->setText(tr("White"));
	ui.radioBlack->setText(tr("Black"));
	setPCEnabled(true);
}

void DlgSetting::onPCPCMode()
{
	ui.radioWhite->setText(tr("White - PC1"));
	ui.radioBlack->setText(tr("Black - PC2"));
	setPCEnabled(true);
}

void DlgSetting::onPCHumanMode()
{
	ui.radioWhite->setText(tr("White - Human"));
	ui.radioBlack->setText(tr("Black - PC"));
	setPCEnabled(true);
}

void DlgSetting::onHumanHumanMode()
{
	ui.radioWhite->setText(tr("White - Human1"));
	ui.radioBlack->setText(tr("Black - Human2"));
	setPCEnabled(false);
}

int DlgSetting::getAlgorithm() const {
	return	ui.radioMinMax->isChecked()    ? MIN_MAX : 
			ui.radioAlphaBeta->isChecked() ? ALPHA_BETA : 
											 ALPHA_BETA_IMPROVED;
}

void DlgSetting::setAlgorithm(int alg)
{
	if(alg == MIN_MAX)
		ui.radioMinMax->setChecked(true);
	else if(alg == ALPHA_BETA)
		ui.radioAlphaBeta->setChecked(true);
	else
		ui.radioAlphaBetaImproved->setChecked(true);
	onAlgorithmChanged();
}

int DlgSetting::getEstimation() const {
	return ui.radioBasicEstimation->isChecked() ? BASIC_ESTIMATION : IMPROVED_ESTIMATION;
}

void DlgSetting::setEstimation(int estimation)
{
	if(estimation == BASIC_ESTIMATION)
		ui.radioBasicEstimation->setChecked(true);
	else
		ui.radioImprovedEstimation->setChecked(true);
}

void DlgSetting::setPCEnabled(bool enable)
{
	ui.gbAlgorithm ->setVisible(enable);
	ui.gbEstimation->setVisible(enable);
	ui.gbDepth     ->setVisible(enable);
}

void DlgSetting::onDepthChanged(int depth) 
{
	ui.labelDepth->setText(tr("Depth = %1 (%2)")
		.arg(depth)
		.arg(tr("The greater the smarter, but slower")));
}

void DlgSetting::onTimeLimitChanged(int seconds) {
	ui.labelTimeLimit->setText(tr("Time limit = %1s (%2)")
		.arg(seconds).arg(tr("The longer the smarter")));
}

void DlgSetting::setDepth(int depth)
{
	ui.sliderDepth->setValue(depth);
	onDepthChanged(depth);
}

void DlgSetting::setTimeLimit(int seconds)
{
	ui.sliderTimeLimit->setValue(seconds);
	onTimeLimitChanged(seconds);
}

void DlgSetting::onLanguageChanged(int index)
{
	QString languages[] = {"English", "Chinese"};
	UserSetting* setting = MySetting<UserSetting>::getInstance();
	setting->setValue("Language", languages[index]);
}

void DlgSetting::setLanguage(const QString& language) {
	ui.cbLanguage->setCurrentIndex(language == "Chinese" ? 1 : 0);
}

void DlgSetting::onAlgorithmChanged()
{
	ui.radioBasicEstimation   ->setChecked(getAlgorithm() != ALPHA_BETA_IMPROVED);
	ui.radioImprovedEstimation->setChecked(getAlgorithm() == ALPHA_BETA_IMPROVED);
}

void DlgSetting::onEstimationChanged()
{
	ui.radioAlphaBeta        ->setChecked(getEstimation() != IMPROVED_ESTIMATION);
	ui.radioAlphaBetaImproved->setChecked(getEstimation() == IMPROVED_ESTIMATION);
}

int DlgSetting::getLimitBy() const {
	return ui.radioLimitByDepth->isChecked() ? LIMIT_BY_DEPTH : LIMIT_BY_TIME;
}

void DlgSetting::setLimitBy(int limitBy)
{
	if(limitBy == LIMIT_BY_DEPTH)
		ui.radioLimitByDepth->setChecked(true);
	else
		ui.radioLimitByTime->setChecked(true);
}

//////////////////////////////////////////////////////////////////////////
// UserSetting
void UserSetting::loadDefaults()
{
	setValue("Status", "xxxxxxxxxxxxxxxxxxxxxxx");
	setValue("Opening", true);
	setValue("Mode", "Single step");
	setValue("StartColor", "W");
	setValue("CurrentColor", "W");
	setValue("Algorithm", "MinMax");
	setValue("Estimation", "Basic");
	setValue("Depth", 3);
	setValue("TimeLimit", 30);
	setValue("LimitBy", "Time");
}

void UserSetting::setMode(int mode) {
	setValue("Mode", modeNames[mode]);
}

void UserSetting::setAlgorithm(int alg) {
	setValue("Algorithm", algorithmNames[alg]);
}

void UserSetting::setEstimation(int estimation) {
	setValue("Estimation", estimationNames[estimation]);
}

int UserSetting::getMode() const
{
	int result = modeNames.indexOf(value("Mode").toString());
	return result > -1 ? result : 0;
}

int UserSetting::getAlgorithm() const
{
	int result = algorithmNames.indexOf(value("Algorithm").toString());
	return result > -1 ? result : 0;
}

int UserSetting::getEstimation() const
{
	int result = estimationNames.indexOf(value("Estimation").toString());
	return result > -1 ? result : 0;
}

UserSetting::UserSetting(const QString& userName) : MySetting<UserSetting>(userName)
{
	modeNames << "Single step" << "PC PC" << "PC Human" << "Human Human";
	algorithmNames << "MinMax" << "AlphaBeta" <<"AlphaBetaImproved";
	estimationNames << "Basic" << "Improved";
	limitByNames << "Depth" << "Time";

	if(QFile(userName).size() == 0)   // no setting
		loadDefaults();
}

void UserSetting::setLimitBy(int limitBy) {
	setValue("LimitBy", limitByNames[limitBy]);
}

int UserSetting::getLimitBy() const
{
	int result = limitByNames.indexOf(value("LimitBy").toString());
	return result > -1 ? result : 0;
}