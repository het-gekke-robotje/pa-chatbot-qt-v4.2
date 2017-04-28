#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <QFile>
#include "PAChatClient.h"
#include "ProxyCheckerTask.h"

ProxyCheckerTask::ProxyCheckerTask(QObject *parent) 
	: QObject(parent) ,
	current_running(0),
	max_running(64)
{

}

bool remove_character(char c)
{
	if (c == ':' || c == '.' || (c >= '0' && c <= '9'))
	{
		return false;
	}
	return true;
}

void ProxyCheckerTask::checkNextProxy()
{
	if (proxies_vector.size())
	{
		std::pair<QString, ushort> proxy = proxies_vector.front();
		proxies_vector.pop();
		++current_running;
		PAChatClient* client = new PAChatClient(proxy.first, proxy.second, this);
		connect(client, &PAChatClient::onProxyNotWorking, this, &ProxyCheckerTask::onProxyNotWorking);
		connect(client, &PAChatClient::onProxyWorking, this, &ProxyCheckerTask::onProxyWorking);
	}
}

void ProxyCheckerTask::finishUp()
{
	for (auto& proxy : working_proxies)
	{
		std::cout << proxy.first.toStdString() << ":" << proxy.second << std::endl;
	}

	emit finished();
}

void ProxyCheckerTask::fillProxies()
{
	while (current_running < max_running && proxies_vector.size() > 0)
	{
		checkNextProxy();
	}
}

void ProxyCheckerTask::onProxyNotWorking()
{
	delete QObject::sender();
	--current_running;
	fillProxies();

	if (current_running == 0 && proxies_vector.size() == 0)
	{
		finishUp();
	}
}

void ProxyCheckerTask::onProxyWorking()
{
	PAChatClient* client = (PAChatClient*)QObject::sender();
	working_proxies.push_back(client->GetProxy());

	delete QObject::sender();
	--current_running;
	fillProxies();

	if (current_running == 0 && proxies_vector.size() == 0)
	{
		finishUp();
	}
}

void ProxyCheckerTask::run()
{
	QFile file("proxylist.txt");
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << file.errorString();
		emit finished();
		return;
	}

	std::map<ushort, std::set<QString>> proxies;

	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		std::string s = line.toStdString();
		s.erase(std::remove_if(s.begin(), s.end(), remove_character), s.end());
		auto proxy_entry = QString::fromStdString(s).split(':');
		if (proxy_entry.size() == 2)
		{
			bool succeeded = false;
			ushort port = proxy_entry[1].toInt(&succeeded);
			if (succeeded)
			{
				proxies[port].insert(proxy_entry[0]);
			}
		}		
	}

	for (auto& port : proxies)
	{
		for (auto& host : port.second)
		{
			proxies_vector.push(std::pair<QString, ushort>(host, port.first));
		}
	}

	fillProxies();
}
