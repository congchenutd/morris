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
	connect(ui.sliderDepth,     SIGNAL(valueChanged(int)), this, SLOT(onDepthChanged    (int)));
	connect(ui.sliderTimeLimit, SIGNAL(valueChanged(int)), this, SLOT(onTimeLimitChanged(int)));
	connect(ui.sliderTableSize, SIGNAL(valueChanged(int)), this, SLOT(onTableSizeChanged(int)));
	connect(ui.radioMinMax,            SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
	connect(ui.radioAlphaBeta,         SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
	connect(ui.radioAlphaBetaImproved, SIGNAL(clicked()), this, SLOT(onAlgorithmChanged()));
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

void DlgSetting::setPCEnabled(bool enable)
{
	ui.groupAlgorithm ->setEnabled(enable);
	ui.groupParameters->setEnabled(enable);
}

void DlgSetting::onDepthChanged(int depth) {
	ui.radioLimitByDepth->setText(tr("Depth Limit = %1").arg(depth));
}

void DlgSetting::onTimeLimitChanged(int seconds) {
	ui.radioLimitByTime->setText(tr("Time Limit = %1 s").arg(seconds));
}

void DlgSetting::onTableSizeChanged(int size) {
	ui.labelTableSize->setText(tr("Hashtable Size = %1 Million").arg(size));
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

QString DlgSetting::getLanguage() const {
	QString languages[] = {"English", "Chinese"};
	return languages[ui.cbLanguage->currentIndex()];
}

void DlgSetting::setLanguage(const QString& language) {
	ui.cbLanguage->setCurrentIndex(language == "Chinese" ? 1 : 0);
}

void DlgSetting::onAlgorithmChanged()
{
	ui.radioLimitByTime ->setEnabled(getAlgorithm() == ALPHA_BETA_IMPROVED);
	ui.sliderTimeLimit  ->setEnabled(getAlgorithm() == ALPHA_BETA_IMPROVED);
	ui.labelTableSize   ->setEnabled(getAlgorithm() == ALPHA_BETA_IMPROVED);
	ui.sliderTableSize  ->setEnabled(getAlgorithm() == ALPHA_BETA_IMPROVED);
	ui.radioLimitByDepth->setChecked(getAlgorithm() != ALPHA_BETA_IMPROVED);
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

int DlgSetting::getTableSize() const {
	return ui.sliderTableSize->value() * 1000000;
}

void DlgSetting::setTableSize(int size) 
{
	ui.sliderTableSize->setValue(size / 1000000);
	onTableSizeChanged(size / 1000000);
}

void DlgSetting::accept()
{
	UserSetting* setting = MySetting<UserSetting>::getInstance();
	setting->setValue("Language",  getLanguage());
	setting->setValue("TableSize", getTableSize());
	QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////
// UserSetting
void UserSetting::loadDefaults()
{
	setValue("Status", "xxxxxxxxxxxxxxxxxxxxxxx");
	setValue("Opening", true);
	setValue("Mode", "PC Human");
	setValue("StartColor", "W");
	setValue("CurrentColor", "W");
	setValue("Algorithm", "AlphaBetaImproved");
	setValue("Depth", 6);
	setValue("TimeLimit", 10);
	setValue("LimitBy", "Time");
	setValue("TableSize", 1000000);
}

void UserSetting::setMode(int mode) {
	setValue("Mode", modeNames[mode]);
}

void UserSetting::setAlgorithm(int alg) {
	setValue("Algorithm", algorithmNames[alg]);
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

UserSetting::UserSetting(const QString& userName) : MySetting<UserSetting>(userName)
{
	modeNames << "Single step" << "PC PC" << "PC Human" << "Human Human";
	algorithmNames << "MinMax" << "AlphaBeta" <<"AlphaBetaImproved";
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