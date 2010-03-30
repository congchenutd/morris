#include "test2.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	test2 w;
	w.show();
	return a.exec();
}
