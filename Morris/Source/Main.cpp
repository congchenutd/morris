#include "MainWnd.h"
#include <QtGui>
#include "HashTable.h"

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
}
