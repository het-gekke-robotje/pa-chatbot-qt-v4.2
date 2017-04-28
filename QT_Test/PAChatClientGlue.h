#pragma once
#include <QtWidgets/QListWidget>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtWidgets/QCheckBox>
#include "PAChatClient.h"
#include "PAChatClientUI.h"
#include "PAChatClientAutoSender.h"
#include "PAChatClientFilter.h"
#include "PAChatAI.h"
#include "ProxyEntry.h"

class PAChatManager;
class PAChatClientGlue: public QObject, public QListWidgetItem
{
	Q_OBJECT
private:
	QPointer<PAChatClient> client;
	QPointer<PAChatClientUI> ui;
	QPointer<PAChatClientAutoSender> auto_sender;
	QPointer<PAChatClientFilter> message_filter;
	QPointer<PAChatAI> message_ai;

	QPointer<ProxyEntry> proxy_;
	QPointer<QTabWidget> tabs_container_;

	QPointer<QCheckBox> send_intro_message_check_box_;
	QPointer<QCheckBox> story_mode_check_box_;

	QPointer<QCheckBox> ai_mode_check_box_;
	QPointer<QCheckBox> filtered_chat_end_mode_check_box_;

	QPointer<QCheckBox> filter_unneeded_chat_entries_check_box_;

	QPointer<QCheckBox> logging_checkbox_;
	QPointer<QCheckBox> allow_stop_check_box_;

	QPointer<PAChatManager> chat_manager_;

public:
	enum PAChatClientGlueState
	{
		PAChatClientGlueState_BotCreated,
		PAChatClientGlueState_OpeningChat,
		PAChatClientGlueState_ReadyToChat,
		PAChatClientGlueState_Searching,
		PAChatClientGlueState_ChattingNoUnreadMessages,
		PAChatClientGlueState_ChattingUnreadMessages,
		PAChatClientGlueState_ChattingResponded,
		PAChatClientGlueState_EndedReadyToChat,
		PAChatClientGlueState_Disconnected,
		PAChatClientGlueState_GeneratingSID,
		PAChatClientGlueState_Connecting,
	};

private:
	PAChatClientGlueState glue_state_;

	bool force_red;
	bool chat_logging_;
	int int_id_;
	QString string_id_;
	int other_message_count_;

	QTimer silence_timer;
public:

	PAChatClientGlue(
		ProxyEntry* proxy, 
		QTabWidget* tabs_container_, 
		QCheckBox* send_intro_message_check_box, 
		QCheckBox* story_mode_check_box, 
		QCheckBox* ai_mode_check_box,
		QCheckBox* filtered_chat_end_mode_check_box,
		PAChatManager* chat_manager,
		QCheckBox* filter_unneeded_chat_entries_check_box,
		QCheckBox* logging_checkbox,
		QCheckBox* allow_stop_check_box,
		QObject *parent = Q_NULLPTR);
	~PAChatClientGlue();

private:
	void SetStateColor();

public:

	PAChatClientGlueState GetGlueState();
	bool ReadyForSearch();
	bool Search();
	QWidget* GetTab();
	void FocusInputText();
	void SendMessage(QString string, int sender_id = 0);
	void Reconnect(int delay = 0);
	PAChatClient* Client();
	void KillWithText();

Q_SIGNALS:
	void onGlueSocketConnected();
	void onGlueChatConnected();
	void onGlueChatSearch();
	void onGlueChatBegin();
	void onGlueChatEnd();
	void onGlueSocketDisconnected();

	void onRequestRemove();
	void onOnlineCountUpdate(int online_count);

private Q_SLOTS:
	void onGeneratingSID();
	void onSocketConnecting();
	void onSocketConnected();
	void onChatConnected();
	void onChatSearch();
	void onChatBegin();
	void onChatTyping(bool me, bool typing);
	void onChatMessage(bool me, QString message, int sender_id);
	void onChatEnd();
	void onChatOnlineCount(int online_count);
	void onSocketDisconnected();
	void onRequestRemoveBot();
	void onRequestChatSendMessage(QString message);
	void onRequestChatEnd();
	void onRequestChatKeep();
	void onTextInputChanged(QString text);
	void onSilenceTimerHit();
	void onRequestStopAutoSender();
	void onAutoSenderMessage(QString string, bool last_message);
	void showInfoStateChanged(int state);
};
 