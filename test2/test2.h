#ifndef TEST2_H
#define TEST2_H

#include <QtGui/QMainWindow>
#include "ui_test2.h"

class test2 : public QMainWindow
{
	Q_OBJECT

public:
	test2(QWidget *parent = 0, Qt::WFlags flags = 0);
	~test2();

private:
	Ui::test2Class ui;
};

#endif // TEST2_H
