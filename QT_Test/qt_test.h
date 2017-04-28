#ifndef QT_TEST_H
#define QT_TEST_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QPointer>
#include "ui_qt_test.h"
#include "PAChatManager.h"

class QT_Test : public QMainWindow
{
	Q_OBJECT

public:
	QT_Test(QWidget *parent = 0);
	~QT_Test();

private:
	Ui::QT_TestClass ui;
	QPointer<PAChatManager> manager;
};

#endif // QT_TEST_H
