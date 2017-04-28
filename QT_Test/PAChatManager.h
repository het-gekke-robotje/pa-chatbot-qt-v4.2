#pragma once
#include <set>
#include <queue>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLabel>
#include "PAChatClientGlue.h"
#include "PAChatClientMacro.h"
#include "ProxyList.h"
#include "PAStatisticsLogger.h"

class PAChatManager : public QObject
{
	Q_OBJECT
private:
	ProxyList proxy_list_;

	QPointer<QListWidget> macro_list_;
	QPointer<PAChatClientMacro> chat_macros_;

	std::set<PAChatClientGlue*> clients;
	QPointer<PAChatClientGlue> current_searching_;
	QPointer<PAChatClientGlue> current_active_;

	QPointer<QListWidget> list_view_;

	QPointer<QPushButton> add_new_bot_button_;
	QPointer<QPushButton> request_new_chat_button_;
	QPointer<QCheckBox> automatic_search_check_box_;
	QPointer<QPushButton> fill_with_bots_button_;

	QPointer<QCheckBox> send_intro_message_check_box_;
	QPointer<QCheckBox> story_mode_check_box_;

	QPointer<QCheckBox> ai_mode_check_box_;
	QPointer<QCheckBox> filtered_chat_end_mode_check_box_;

	QPointer<QCheckBox> filter_unneeded_chat_entries_check_box_;

	QPointer<QLabel> online_count_label_;

	QPointer<QTabWidget> tabs_container_;

	QPointer<QPushButton> reload_filter_button_;

	QPointer<QCheckBox> logging_checkbox_;

	QPointer<QCheckBox> allow_stop_check_box_;

	QPointer<QPushButton> clean_proxy_list_button_;

	PAStatisticsLogger stats_logger_;

	bool auto_search_enabled;

	void PopClient2(PAChatClientGlue*);

	QTimer search_timer;
	QTimer search_watch_dog_;

	std::set<PAChatClientGlue*> ready_to_search;

	void PrepareSearch(PAChatClientGlue* glue);

	int chats_started_;
	int online_count_;

	void UpdateInfoLabel();

	void RemoveFromSearch(PAChatClientGlue* glue);
public:
	PAChatManager(
		QListWidget* proxy_entry_item_list_widget, 
		QLineEdit* proxy_entries_textedit, 
		QPushButton* proxy_entries_submit_button, 

		QListWidget* chats_list_widget,
		QTabWidget* tabs_container,

		QPushButton* add_new_bot_button,
		QPushButton* request_new_chat_button,
		QCheckBox* automatic_search_check_box,

		QCheckBox* send_intro_message_check_box,
		QCheckBox* story_mode_check_box,

		QListWidget* macro_list,

		QPushButton* fill_with_bots_button,

		QLabel* online_count_label,

		QCheckBox* ai_mode_check_box,
		QCheckBox* filtered_chat_end_mode_check_box,

		QCheckBox* filter_unneeded_chat_entries_check_box,

		QPushButton* reload_filter_button,

		QCheckBox* logging_checkbox,

		QCheckBox* allow_stop_check_box,

		QPushButton* clean_proxy_list_button,

		QObject *parent = Q_NULLPTR);
	~PAChatManager();

	size_t GetReadyToSearchSize();

private Q_SLOTS:
	void onAutoSearcherStateChange(int state);
	void onItemSelected(QListWidgetItem * item);
	void onMacroRequested(QString text);
	void onChatConnected();
	void onChatSearch();
	void onChatBegin();
	void onChatEnd();
	void onSocketConnected();
	void onSocketDisconnected();
	void searchTimeout();
	void PushClient();
	void PopClient();
	void PushClientsFull();
	void onOnlineCountUpdate(int online_count);
	void onFilterReload();
	void onSearchWatchDog();
	void onRequestCleanProxyList();
	void RequestNewChat();
};
