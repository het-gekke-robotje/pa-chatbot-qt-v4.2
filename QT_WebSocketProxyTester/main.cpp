#include <QtCore>
#include "PAChatClient.h"
#include "ProxyCheckerTask.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	// Task parented to the application so that it
	// will be deleted by the application.
	ProxyCheckerTask *task = new ProxyCheckerTask(&a);

	// This will cause the application to exit when
	// the task signals finished.    
	QObject::connect(task, &ProxyCheckerTask::finished, &a, &QCoreApplication::quit);

	// This will run the task from the application event loop.
	QTimer::singleShot(0, task, &ProxyCheckerTask::run);

	return a.exec();
}
