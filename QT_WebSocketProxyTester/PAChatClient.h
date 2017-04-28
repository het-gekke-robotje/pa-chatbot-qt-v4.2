#pragma once
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

class PAChatClient : public QObject
{
	Q_OBJECT
private:
	QPointer<QTimer> process_timeout_;
	QPointer<QNetworkAccessManager> netman_;
	QPointer<QNetworkReply> reply_manager_;
	QString proxy_host;
	ushort proxy_port;
public:
	PAChatClient(QString proxy_host, ushort proxy_port, QObject *parent = Q_NULLPTR);
	~PAChatClient();

	std::pair<QString, ushort> GetProxy();
Q_SIGNALS:
	void onProxyWorking();
	void onProxyNotWorking();

private Q_SLOTS:
	void onTimeout();
	void onNetworkReply(QNetworkReply* reply);
};
