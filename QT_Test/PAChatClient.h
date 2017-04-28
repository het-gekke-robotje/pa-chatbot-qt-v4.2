#pragma once
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtWidgets/QListWidget>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include <array>
#include "ProxyEntry.h"

enum PAChatClientState
{
	PAChatClientState_Disconnected,//= 0xDEAD
	PAChatClientState_GeneratingSID,
	PAChatClientState_SocketConnecting,
	PAChatClientState_ChatConnecting,
	PAChatClientState_Idle,
	PAChatClientState_Searching,
	PAChatClientState_Chatting
};

const std::array<unsigned int, 8> PAChatClientStateColors
{
	0xFFFFFF,
	0xC2C2C2,
	0xC2C2C2,
	0xC2C2C2,
	0xC2C2C2,
	0xD9FF00,
	0x00FF40,
	0x00FFE5,
};

constexpr unsigned int GetStateColor(PAChatClientState state, bool newMessageAvailable, bool force_red)
{
	return state == PAChatClientState_Chatting ? (force_red ? 0xFF5959 : PAChatClientStateColors[state + (int)newMessageAvailable]) : PAChatClientStateColors[state];
}

class PAChatClient : public QObject
{
	Q_OBJECT
private:
	bool searching_;
	bool connected_;
	bool chatting_;
	bool is_typing_;
	bool is_other_typing_;
	int online_count_;

	int sid_gen_retries_;
	int max_sid_gen_retries_;

	QPointer<QWebSocket> webSocket_;
	QPointer<QTimer> process_timeout_;
	QPointer<QTimer> pinger_;
	QPointer<QTimer> online_count_update_;
	QPointer<QNetworkAccessManager> netman_;
	QPointer<ProxyEntry> proxy_;
	QPointer<QNetworkReply> reply_manager_;

	PAChatClientState state_;

	void StartGeneratingSID();
	void Initialize();
public:
	PAChatClient(ProxyEntry* proxy = nullptr, int max_sid_gen_attempts = 5, QObject *parent = Q_NULLPTR);
	~PAChatClient();

	bool Search();
	bool Searching();
	bool Connected();
	bool Chatting();
	bool SendMessage(QString messge, int sender_id = 0);
	bool SendImageHash(QString hash, int sender_id = 0);
	bool Report(QString reason = "", int sender_id = 0);
	bool SendRaw(QString bytes);
	bool SendTyping(bool typing);
	bool EndChat();
	int OnlineCount();

	void Reconnect(int delay = 0);
	PAChatClientState State();
	ProxyEntry* GetProxy();
	void ResetRetries();

Q_SIGNALS:
	void onGeneratingSID();
	void onSocketConnecting();
	void onSocketConnected();
	void onChatConnected();
	void onChatSearch();
	void onChatBegin();
	void onChatTyping(bool me, bool typing);
	void onChatMessage(bool me, QString message, int sender_id);
	void onChatImage(bool me, QString image_link, int sender_id);
	void onChatEnd();
	void onChatOnlineCount(int online_count);
	void onSocketDisconnected();
	void onProxyNotWorking(ProxyEntry* proxy, int current_retry_count, int max_retry_count);
	void onSidGenRetryCountExceeded(ProxyEntry* proxy, int max_retry_count);

private Q_SLOTS:
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(QString message);
	void onPing();
	void onOnlineCountUpdate();
	void onNetworkReply(QNetworkReply* reply);
};
