#pragma once
#include <queue>
#include <vector>
#include <string>
#include <QString>
#include <QObject>

class ProxyCheckerTask : public QObject
{
	Q_OBJECT
private:
	std::queue<std::pair<QString, ushort>> proxies_vector;
	std::vector<std::pair<QString, ushort>> working_proxies;
	size_t current_running;
	size_t max_running;

	void checkNextProxy();
	void fillProxies();
	void finishUp();

public:
	ProxyCheckerTask(QObject *parent = 0);

	public slots :
	void run();
	void onProxyWorking();
	void onProxyNotWorking();

signals:
	void finished();
};
