#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtNetwork/QNetworkCookie>
#include "PAChatClient.h"

QT_USE_NAMESPACE

void PAChatClient::Initialize()
{
	if (webSocket_)
	{
		delete webSocket_;
		delete pinger_;
		delete online_count_update_;
		delete process_timeout_;
	}

	connected_ = false;
	searching_ = false;
	chatting_ = false;
	is_typing_ = false;
	is_other_typing_ = false;
	online_count_ = 0;
	sid_gen_retries_ = 0;

	webSocket_ = new QWebSocket("http://ws.praatanoniem.nl");
	pinger_ = new QTimer(this);
	online_count_update_ = new QTimer(this);
	process_timeout_ = new QTimer(this);

	connect(webSocket_, &QWebSocket::connected, this, &PAChatClient::onConnected);
	connect(webSocket_, &QWebSocket::disconnected, this, &PAChatClient::onDisconnected);
	connect(webSocket_, &QWebSocket::textMessageReceived, this, &PAChatClient::onTextMessageReceived);

	connect(pinger_, &QTimer::timeout, this, &PAChatClient::onPing);
	connect(online_count_update_, &QTimer::timeout, this, &PAChatClient::onOnlineCountUpdate);
	connect(process_timeout_, &QTimer::timeout, this, &PAChatClient::onDisconnected);

	StartGeneratingSID();
}

PAChatClient::PAChatClient(ProxyEntry* proxy, int max_sid_gen_attempts, QObject *parent)
	: QObject(parent),
	proxy_(proxy),
	max_sid_gen_retries_(max_sid_gen_attempts),
	webSocket_(nullptr),
	netman_(nullptr),
	pinger_(nullptr),
	online_count_update_(nullptr),
	process_timeout_(nullptr),
	reply_manager_(nullptr)
{
	Initialize();
}

void PAChatClient::StartGeneratingSID()
{
	// fix another memory leak? - or well connection/timeout leak
	if (netman_)
	{
		delete netman_;
		netman_ = new QNetworkAccessManager(this);
		connect(netman_, &QNetworkAccessManager::finished, this, &PAChatClient::onNetworkReply);
	}
	else if (!netman_)
	{
		netman_ = new QNetworkAccessManager(this);
		connect(netman_, &QNetworkAccessManager::finished, this, &PAChatClient::onNetworkReply);
	}

	if (sid_gen_retries_ <= max_sid_gen_retries_)
	{
		++sid_gen_retries_;
		//
		QUrl url("http://ws.praatanoniem.nl/socket.io/?EIO=3&transport=polling&t=" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "-0");
		QNetworkRequest request(url);

		request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
		request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:48.0) Gecko/20100101 Firefox/48.0");
		request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
		request.setRawHeader("Accept-Encoding", "gzip, deflate");
		request.setRawHeader("Referer", "http://www.praatanoniem.nl");
		request.setRawHeader("Origin", "http://www.praatanoniem.nl");
		request.setRawHeader("Cookie", "");
		request.setRawHeader("Connection", "keep-alive");

		if (proxy_->GetHost().length())
		{
			QNetworkProxy proxy;
			proxy.setHostName(proxy_->GetHost());
			proxy.setPort(proxy_->GetPort());
			proxy.setType(QNetworkProxy::ProxyType::Socks5Proxy);
			netman_->setProxy(proxy);
		}

		reply_manager_ = netman_->get(request);

#ifndef _DEBUG
		process_timeout_->start(30000);
#else
		process_timeout_->start(300000);
#endif
		state_ = PAChatClientState_GeneratingSID;
		emit onGeneratingSID();
	}
	else
	{
		emit onSidGenRetryCountExceeded(proxy_, max_sid_gen_retries_);
		// TODO remove this in release
		Reconnect(2500);
	}
}

void PAChatClient::onNetworkReply(QNetworkReply* reply)
{
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
			QUrl url("ws://ws.praatanoniem.nl/socket.io/?EIO=3&transport=websocket&sid=" + SID);

			if (proxy_->GetHost().length())
			{
				QNetworkProxy proxy;
				proxy.setHostName(proxy_->GetHost());
				proxy.setPort(proxy_->GetPort());
				proxy.setType(QNetworkProxy::ProxyType::Socks5Proxy);
				webSocket_->setProxy(proxy);
			}

			webSocket_->open(url);

			state_ = PAChatClientState_SocketConnecting;
			emit onSocketConnecting();
		}
	}

	if(!done)
	{
		emit onProxyNotWorking(proxy_, sid_gen_retries_, max_sid_gen_retries_);
		onDisconnected();
	}
	//reply_manager_->deleteLater();
}

PAChatClient::~PAChatClient()
{
	disconnect(webSocket_);
	if (reply_manager_)
	{
		delete reply_manager_;
		reply_manager_ = nullptr;
	}
	disconnect(this, 0, 0, 0);
}

void PAChatClient::onPing()
{
	webSocket_->sendTextMessage(QStringLiteral("2"));
}

void PAChatClient::onOnlineCountUpdate()
{
	webSocket_->sendTextMessage(QStringLiteral("42[\"count\"]"));
}

void PAChatClient::onConnected()
{
	qDebug() << "WebSocket connected";

	webSocket_->sendTextMessage(QStringLiteral("2probe"));

	state_ = PAChatClientState_ChatConnecting;

	emit onSocketConnected();
	//then you'll receive '3probe'
}

void PAChatClient::onDisconnected()
{
	state_ = PAChatClientState_Disconnected;
	connected_ = false;
	searching_ = false;
	chatting_ = false;
	is_typing_ = false;
	is_other_typing_ = false;
	online_count_ = 0;
	/*if (reply_manager_)
	{
		delete reply_manager_;
		reply_manager_ = nullptr;
	}*/
	reply_manager_->deleteLater();

	pinger_->stop();
	online_count_update_->stop();

	emit onSocketDisconnected();

	//Reconnect?
	//StartGeneratingSID();
	//QTimer::singleShot(0, this, &PAChatClient::Initialize);
}

void PAChatClient::onTextMessageReceived(QString incomming_message)
{
	qDebug() << "Message received:" << incomming_message;
	QString json = "";
	if (incomming_message.startsWith("42["))
	{
		incomming_message = incomming_message.mid(2);
		int start_brace = incomming_message.indexOf("\",{");
		if (start_brace != -1)
		{
			start_brace += 2;
			if (incomming_message.indexOf("}]", start_brace) != -1)
			{
				json = incomming_message.mid(start_brace, incomming_message.length() - (start_brace + 1));
			}
		}
	}

	if (json.length() == 0)
	{
		if (searching_ && incomming_message == "[\"start\"]")
		{
			searching_ = false;
			chatting_ = true;
			is_typing_ = false;
			is_other_typing_ = false;

			state_ = PAChatClientState_Chatting;
			//OnChatBegin
			emit onChatBegin();
		}
		else if (chatting_ && incomming_message == "[\"end\"]")
		{
			searching_ = false;
			chatting_ = false;
			is_typing_ = false;
			is_other_typing_ = false;

			state_ = PAChatClientState_Idle;
			//OnChatEnd
			emit onChatEnd();
		}
		else if (!connected_ && incomming_message == "3probe")
		{
			webSocket_->sendTextMessage(QStringLiteral("5"));

			pinger_->start(26000);
			online_count_update_->start(10000);

			onOnlineCountUpdate();

			connected_ = true;

			process_timeout_->stop();

			state_ = PAChatClientState_Idle;
			//OnConnected
			emit onChatConnected();
		}
	}
	else if (json.length() > 2)
	{
		switch (json.at(2).toLatin1())
		{
		case 't': //{"is_typing_":true}
			{
				bool temp = json[json.length() - 3] == 'u';
				if (temp != is_other_typing_)
				{
					is_other_typing_ = temp;
					//OtherIsTyping
					emit onChatTyping(false, is_other_typing_);
				}
			}
			break;
		case 'u': //{"userCount":123}
			{
				int integer_start = json.indexOf(':') + 1;
				QString num = json.mid(integer_start, json.length() - (integer_start + 1));
				bool ok;
				int temp_val = num.toInt(&ok);
				if (ok)
				{
					online_count_ = temp_val;
					emit onChatOnlineCount(online_count_);
				}
			}
			break;
		case 'm': //{"message":"hello world!","sender":"Onbekende"}
			{
				int start_message = json.indexOf("e\":\"") + 4;
				int end_message = json.lastIndexOf("\",\"s");

				QString recvd_message = json.mid(start_message, end_message - start_message).replace("\\\"", "\"").replace("\\\\", "\\");
				if (recvd_message[0] == 129)
				{
					recvd_message = recvd_message.mid(2);
				}
				//OnChatMessage
				emit onChatMessage(false, recvd_message, -1);
			}
			break;

		case 'i': //{"hash":"ad32a067f076eaf409c1776f22c0df9dc28a58fc372c4ea13a7b1205cca0a8a9f895433e21e33b490fbae07e50e953f719c60a966b3176d35f8f98e7","expire":1462710282}
			{
				int start_hash = json.indexOf("h\":\"") + 4;
				int end_hash = json.lastIndexOf("\",\"e");

				QString image_link = "http://www.praatanoniem.nl/image.php?hash=" + json.mid(start_hash, end_hash - start_hash);

				//OnChatMessage
				emit onChatImage(false, image_link, -1);
			}
			break;
		}
	}
}

bool PAChatClient::Search()
{
	if (!connected_ || searching_ || chatting_)
	{
		return false;
	}

	searching_ = true;
	webSocket_->sendTextMessage(QStringLiteral("42[\"start\"]"));
	state_ = PAChatClientState_Searching;
	emit onChatSearch();

	return true;
}

bool PAChatClient::Searching()
{
	return searching_;
}

bool PAChatClient::Connected()
{
	return connected_;
}

bool PAChatClient::Chatting()
{
	return chatting_;
}

bool PAChatClient::SendMessage(QString message, int sender_id)
{
	if (!connected_ || searching_ || !chatting_ || message.length() < 1)
	{
		return false;
	}

	QString format("42[\"message\",{\"message\":\"%1\"}]");

	if (sender_id != 1336 && sender_id != 1338)
	{
		webSocket_->sendTextMessage(format.arg(QString(message).replace("\\", "\\\\").replace("\"", "\\\"")));
	}

	emit onChatMessage(true, message, sender_id);

	SendTyping(false);

	return true;
}

int PAChatClient::OnlineCount()
{
	return online_count_;
}

PAChatClientState PAChatClient::State()
{
	return state_;
}

bool PAChatClient::SendTyping(bool typing)
{
	if (!connected_ || searching_ || !chatting_ || typing == is_typing_)
	{
		return false;
	}

	is_typing_ = typing;

	webSocket_->sendTextMessage("42[\"typing\",{\"typing\":" + QString(is_typing_ ? "true" : "false") +"}]");
	emit onChatTyping(true, is_typing_);

	return true;
}

bool PAChatClient::EndChat()
{
	if (!connected_ || searching_ || !chatting_)
	{
		return false;
	}

	searching_ = false;
	chatting_ = false;
	is_typing_ = false;
	is_other_typing_ = false;

	state_ = PAChatClientState_Idle;

	webSocket_->sendTextMessage("42[\"end\"]");
	emit onChatEnd();

	return true;	
}

//doesn't work...
void PAChatClient::Reconnect(int delay)
{
	QTimer::singleShot(delay, this, &PAChatClient::Initialize);
}

ProxyEntry* PAChatClient::GetProxy()
{
	return proxy_;
}

bool PAChatClient::SendImageHash(QString hash, int sender_id)
{
	if (!connected_ || searching_ || !chatting_ || hash.length() < 1)
	{
		return false;
	}

	//120
	//{"hash":"ad32a067f076eaf409c1776f22c0df9dc28a58fc372c4ea13a7b1205cca0a8a9f895433e21e33b490fbae07e50e953f719c60a966b3176d35f8f98e7","expire":1462710282}
	QString format("42[\"image\",{\"hash\":\"%1\",\"expire\":%2}]");

	webSocket_->sendTextMessage(format.arg(QString(hash)).arg((QDateTime::currentMSecsSinceEpoch()/1000)+120));

	emit onChatImage(true, hash, sender_id);

	return true;
}

bool PAChatClient::SendRaw(QString bytes)
{
	if (!connected_ || searching_ || !chatting_ || bytes.length() < 1)
	{
		return false;
	}

	webSocket_->sendTextMessage(bytes);

	return true;
}

bool PAChatClient::Report(QString reason, int sender_id)
{
	if (!connected_ || searching_ || !chatting_)
	{
		return false;
	}

	webSocket_->sendTextMessage("42[\"report\"]");

	return true;
}