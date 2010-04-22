#include "MainWnd.h"
#include <QtGui>
#include "HashTable.h"
#include "Morris.h"
#include "Estimator.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	UserSetting* setting = MySetting<UserSetting>::getInstance();
	setting->value("Language").toString();
	QTranslator translator;
	translator.load(setting->value("Language").toString());
	qApp->installTranslator(&translator);

	MainWnd wnd;
	wnd.show();
	return app.exec();

/*
	MorrisAlgorithm* algorithm = new NegaMax;
	Estimator* estimator = new ImprovedEstimator;
	algorithm->setEstimator(estimator);
	algorithm->setMemoryLimit(1000000);
	algorithm->loadDB();
	
	for(int depth = 4; depth <=6; ++depth)
	{
		QTime time;
		time.start();

		QFile output("time.txt");
		output.open(QFile::WriteOnly | QFile::Append);
		QTextStream os2(&output);
		os2 << "depth = " << depth << "\r\n";

		QFile file("opening.txt");
		file.open(QFile::ReadOnly);
		QTextStream os(&file);
		while(!os.atEnd())
			algorithm->run(true, os.readLine(), 'W', 0, depth, 0);

		file.setFileName("game.txt");
		file.open(QFile::ReadOnly);
		while(!os.atEnd())
			algorithm->run(false, os.readLine(), 'W', 0, depth, 0);

		os2 << QObject::tr("%1").arg(time.elapsed()) << "\r\n";
	}
	delete algorithm;
	return 0;*/
}
