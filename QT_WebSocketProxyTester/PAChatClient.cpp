#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkProxy>
#include "PAChatClient.h"

QT_USE_NAMESPACE

PAChatClient::PAChatClient(QString proxy_host, ushort proxy_port, QObject *parent)
	: QObject(parent),
	process_timeout_(nullptr),
	netman_(nullptr),
	proxy_host(proxy_host),
	proxy_port(proxy_port)
{
	process_timeout_ = new QTimer(this);
	connect(process_timeout_, &QTimer::timeout, this, &PAChatClient::onTimeout);
	process_timeout_->start(1500);

	netman_ = new QNetworkAccessManager(this);
	connect(netman_, &QNetworkAccessManager::finished, this, &PAChatClient::onNetworkReply);

	QUrl url("http://ws.praatanoniem.nl/socket.io/?EIO=3&transport=polling&t=" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "-0");
	QNetworkRequest request(url);

	request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36");
	request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
	request.setRawHeader("Accept-Encoding", "gzip, deflate");
	request.setRawHeader("Referer", "http://www.praatanoniem.nl");
	request.setRawHeader("Origin", "http://www.praatanoniem.nl");
	request.setRawHeader("Cookie", "");
	request.setRawHeader("Connection", "keep-alive");

	if (proxy_host.length() && proxy_port)
	{
		QNetworkProxy proxy;
		proxy.setHostName(proxy_host);
		proxy.setPort(proxy_port);
		proxy.setType(QNetworkProxy::ProxyType::Socks5Proxy);
		netman_->setProxy(proxy);
	}

	reply_manager_ = netman_->get(request);
}

void PAChatClient::onNetworkReply(QNetworkReply* reply)
{
	process_timeout_->stop();
	bool done = false;

	QVariant cookieVar = reply->header(QNetworkRequest::SetCookieHeader);

	if(cookieVar.isValid())
	{
		QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie>>(cookieVar);
		QString SID;
		for (auto& cookie : cookies)
		{
			if (QString(cookie.name()) == QString("io"))
			{
				SID = QString(cookie.value());
				done = true;
			}
		}

		if (done)
		{
			emit onProxyWorking();
		}
	}

	if(!done)
	{
		onTimeout();
	}

	/*if (reply_manager_)
	{
		delete reply_manager_;
		reply_manager_ = nullptr;
	}*/
}

PAChatClient::~PAChatClient()
{
	delete netman_;
	netman_ = nullptr;
	delete process_timeout_;
	process_timeout_ = nullptr;

	if (reply_manager_)
	{
		delete reply_manager_;
		reply_manager_ = nullptr;
	}
	disconnect(this, 0, 0, 0);
}

std::pair<QString, ushort> PAChatClient::GetProxy()
{
	return std::pair<QString, ushort>(proxy_host, proxy_port);
}

void PAChatClient::onTimeout()
{
	/*if (reply_manager_)
	{
		delete reply_manager_;
		reply_manager_ = nullptr;
	}*/
	reply_manager_->deleteLater();
	
	emit onProxyNotWorking();
}
