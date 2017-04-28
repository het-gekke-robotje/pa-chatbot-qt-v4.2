#include "PAChatManager.h"

QT_USE_NAMESPACE

PAChatManager::PAChatManager(
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

	QObject *parent
)
	: QObject(parent),
	  proxy_list_(proxy_entry_item_list_widget, proxy_entries_textedit, proxy_entries_submit_button, this),
	  add_new_bot_button_(add_new_bot_button),
	  request_new_chat_button_(request_new_chat_button),
	  automatic_search_check_box_(automatic_search_check_box),
	  list_view_(chats_list_widget),
	  tabs_container_(tabs_container),
	  auto_search_enabled(true),
	  current_searching_(nullptr),
	  send_intro_message_check_box_(send_intro_message_check_box),
	  story_mode_check_box_(story_mode_check_box),
	  current_active_(nullptr),
	  macro_list_(macro_list),
	  fill_with_bots_button_(fill_with_bots_button),
	  online_count_label_(online_count_label),
	  chats_started_(0),
	  online_count_(0),
	  ai_mode_check_box_(ai_mode_check_box),
	  filtered_chat_end_mode_check_box_(filtered_chat_end_mode_check_box),
	  filter_unneeded_chat_entries_check_box_(filter_unneeded_chat_entries_check_box),
	  reload_filter_button_(reload_filter_button),
	  logging_checkbox_(logging_checkbox),
	  stats_logger_(this),
	  allow_stop_check_box_(allow_stop_check_box),
	  clean_proxy_list_button_(clean_proxy_list_button)
{

	connect(add_new_bot_button_, &QPushButton::clicked, this, &PAChatManager::PushClient); // god createh ,me,
	connect(automatic_search_check_box_, &QCheckBox::stateChanged, this, &PAChatManager::onAutoSearcherStateChange);
	connect(&search_timer, &QTimer::timeout, this, &PAChatManager::searchTimeout);
	connect(&search_watch_dog_, &QTimer::timeout, this, &PAChatManager::onSearchWatchDog);
	connect(list_view_, &QListWidget::itemClicked, this, &PAChatManager::onItemSelected);
	connect(list_view_, &QListWidget::itemPressed, this, &PAChatManager::onItemSelected);
	connect(list_view_, &QListWidget::itemActivated, this, &PAChatManager::onItemSelected);

	connect(request_new_chat_button_, &QPushButton::clicked, this, &PAChatManager::RequestNewChat);
	connect(fill_with_bots_button_, &QPushButton::clicked, this, &PAChatManager::PushClientsFull);

	chat_macros_ = new PAChatClientMacro(macro_list_, this);
	connect(chat_macros_, &PAChatClientMacro::onMacroRequested, this, &PAChatManager::onMacroRequested);

	connect(reload_filter_button_, &QPushButton::clicked, this, &PAChatManager::onFilterReload);

	connect(clean_proxy_list_button_, &QPushButton::clicked, this, &PAChatManager::onRequestCleanProxyList);

	QFile f1("cc.txt");

	f1.open(QIODevice::ReadOnly | QIODevice::Text);

	QTextStream s1(&f1);

	bool ok = false;
	int num_saved_chats_count = s1.readAll().toInt(&ok);
	if (ok && num_saved_chats_count > 0)
	{
		chats_started_ = num_saved_chats_count;
	}

	f1.close();

	search_watch_dog_.start(5000);
}

PAChatManager::~PAChatManager()
{
	QFile f1("cc.txt");

	f1.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);

	QTextStream s1(&f1);

	s1 << chats_started_;

	f1.close();
}

void PAChatManager::onItemSelected(QListWidgetItem* item)
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(item);
	if (glue)
	{
		QWidget* tab = glue->GetTab();
		if (tab && tabs_container_)
		{
			tabs_container_->setCurrentWidget(tab);
			tab->show();
			glue->FocusInputText();
			current_active_ = glue;
		} 
	}
}

void PAChatManager::PushClient()
{
	ProxyEntry* entry = proxy_list_.GetLowestCountEntry();
	if (proxy_list_.IsFull() || !entry)
	{
		return;
	}

	PAChatClientGlue* glue = new PAChatClientGlue(entry, tabs_container_, send_intro_message_check_box_, story_mode_check_box_, ai_mode_check_box_, filtered_chat_end_mode_check_box_, this, filter_unneeded_chat_entries_check_box_, logging_checkbox_, allow_stop_check_box_, this);
	clients.insert(glue);
	list_view_->addItem(glue);

	connect(glue, &PAChatClientGlue::onRequestRemove, this, &PAChatManager::PopClient);	//plz kill     `me`

	connect(glue, &PAChatClientGlue::onGlueChatConnected, this, &PAChatManager::onChatConnected);
	connect(glue, &PAChatClientGlue::onGlueChatSearch, this, &PAChatManager::onChatSearch);
	connect(glue, &PAChatClientGlue::onGlueChatBegin, this, &PAChatManager::onChatBegin);
	connect(glue, &PAChatClientGlue::onGlueChatEnd, this, &PAChatManager::onChatEnd);
	connect(glue, &PAChatClientGlue::onGlueSocketConnected, this, &PAChatManager::onSocketConnected);
	connect(glue, &PAChatClientGlue::onGlueSocketDisconnected, this, &PAChatManager::onSocketDisconnected);
	connect(glue, &PAChatClientGlue::onOnlineCountUpdate, this, &PAChatManager::onOnlineCountUpdate);

	if (!current_active_)
	{
		current_active_ = glue;
	}
}

void PAChatManager::PushClientsFull()
{
	while (!proxy_list_.IsFull())
	{
		PushClient();
	}
}

void PAChatManager::RequestNewChat()
{
	if (current_active_)
	{
		current_active_->KillWithText();
	}
}

void PAChatManager::onRequestCleanProxyList()
{
	QString good_proxies = "";
	std::set<ProxyEntry*> good_proxy_list;

	for (auto& client : clients)
	{
		switch (client->GetGlueState())
		{
		case PAChatClientGlue::PAChatClientGlueState_BotCreated:
		case PAChatClientGlue::PAChatClientGlueState_Disconnected:
		case PAChatClientGlue::PAChatClientGlueState_GeneratingSID:
		case PAChatClientGlue::PAChatClientGlueState_Connecting:
		case PAChatClientGlue::PAChatClientGlueState_OpeningChat:
			break;

		case PAChatClientGlue::PAChatClientGlueState_ChattingNoUnreadMessages:
		case PAChatClientGlue::PAChatClientGlueState_ChattingUnreadMessages:
		case PAChatClientGlue::PAChatClientGlueState_ChattingResponded:
		case PAChatClientGlue::PAChatClientGlueState_ReadyToChat:
		case PAChatClientGlue::PAChatClientGlueState_EndedReadyToChat:
		case PAChatClientGlue::PAChatClientGlueState_Searching:
			good_proxy_list.insert(client->Client()->GetProxy());
			break;
		}
	}

	for (auto& proxy_entry : good_proxy_list)
	{
		good_proxies += proxy_entry->GetProxy() + ";\n";
	}

	proxy_list_.SetText(good_proxies);
}

void PAChatManager::PopClient()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	PopClient2(glue);
}

void PAChatManager::PopClient2(PAChatClientGlue* glue)
{
	glue->Reconnect(2500);
	/*bool eq = current_active_ == glue;

	ready_to_search.erase(glue);
	clients.erase(glue);
	list_view_->removeItemWidget(glue);
	glue->GetTab()->setHidden(true);
	glue->GetTab()->hide();

	//tabs_container_-
	//delete glue->GetTab();
	//delete glue;

	if (eq && clients.size())
	{
		onItemSelected(*clients.begin());
	}*/
}

void PAChatManager::onSocketConnected()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	UpdateInfoLabel();
	RemoveFromSearch(glue);
}

void PAChatManager::onSocketDisconnected()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	UpdateInfoLabel();
	RemoveFromSearch(glue);
	PopClient();
}

void PAChatManager::onAutoSearcherStateChange(int state)
{
	auto_search_enabled = state > 0;
	if (auto_search_enabled && ready_to_search.size())
	{
		PrepareSearch(*ready_to_search.begin());
	}
}

void PAChatManager::searchTimeout()
{
	if (current_searching_)
	{
		PopClient2(current_searching_);
		current_searching_->Reconnect();
		if (ready_to_search.size())
		{
			PrepareSearch(*ready_to_search.begin());
		}
	}
}

void PAChatManager::PrepareSearch(PAChatClientGlue* glue)
{
	ready_to_search.insert(glue);
	if (!current_searching_ && auto_search_enabled)
	{
		current_searching_ = *ready_to_search.begin();
		ready_to_search.erase(current_searching_);
		if (current_searching_)
		{
			current_searching_->Search();
			search_timer.start(15000);
		}
	}
}

void PAChatManager::onChatBegin()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());

	++chats_started_;
	RemoveFromSearch(glue);
}

void PAChatManager::RemoveFromSearch(PAChatClientGlue* glue)
{
	UpdateInfoLabel();

	ready_to_search.erase(glue);
	if (current_searching_ == glue)
	{
		current_searching_ = nullptr;
		search_timer.stop();
		if (ready_to_search.size())
		{
			PrepareSearch(*ready_to_search.begin());
		}
	}
}

void PAChatManager::onSearchWatchDog()
{
	if (!current_searching_ && auto_search_enabled && ready_to_search.size())
	{
		PrepareSearch(*ready_to_search.begin());
	}
}

void PAChatManager::onChatConnected()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	UpdateInfoLabel();
	PrepareSearch(glue);
}

void PAChatManager::onChatSearch()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	UpdateInfoLabel();
	ready_to_search.erase(glue);
}


void PAChatManager::onChatEnd()
{
	PAChatClientGlue* glue = dynamic_cast<PAChatClientGlue*>(QObject::sender());
	UpdateInfoLabel();
	PrepareSearch(glue);
}

void PAChatManager::onMacroRequested(QString text)
{
	if (current_active_)
	{
		current_active_->SendMessage(text);
	}
}

void PAChatManager::onOnlineCountUpdate(int online_count)
{
	online_count_ = online_count;
	UpdateInfoLabel();
}

void PAChatManager::UpdateInfoLabel()
{
	//incase this should be "bad", finish class "PAChatClientStats" and replace the code below
	int online_bots = 0;
	int chatting_bots = 0;
	int total_bots = clients.size();
	int idle_bots = 0;
	int people_online = 0;
	float f_encounter_chance = 0.0f;
	int encounter_chance = 0;
	for (auto& client : clients)
	{
		switch (client->GetGlueState())
		{
		case PAChatClientGlue::PAChatClientGlueState_BotCreated:
		case PAChatClientGlue::PAChatClientGlueState_Disconnected:
		case PAChatClientGlue::PAChatClientGlueState_GeneratingSID:
		case PAChatClientGlue::PAChatClientGlueState_Connecting:
			break;

		case PAChatClientGlue::PAChatClientGlueState_ChattingNoUnreadMessages:
		case PAChatClientGlue::PAChatClientGlueState_ChattingUnreadMessages:
		case PAChatClientGlue::PAChatClientGlueState_ChattingResponded:
			++online_bots;
			++chatting_bots;
			break;

		case PAChatClientGlue::PAChatClientGlueState_ReadyToChat:
		case PAChatClientGlue::PAChatClientGlueState_EndedReadyToChat:
		case PAChatClientGlue::PAChatClientGlueState_Searching:
			++online_bots;
			++idle_bots;
			break;

		case PAChatClientGlue::PAChatClientGlueState_OpeningChat:
			++online_bots;
			break;
		}
	}

	people_online = online_count_ - online_bots;

	int chance_people = (people_online - (chatting_bots + 1));
	if (chance_people != 0)
	{
		f_encounter_chance = ((float)idle_bots / (float)chance_people) * 100.0f;
		encounter_chance = (int)f_encounter_chance;
	}

	online_count_label_->setText(
		QString::number(people_online) + " People | " + 
		QString::number(online_bots) + "/" + QString::number(total_bots) + " Online/Total Bots\n" +
		QString::number(chatting_bots) + " Chatting | " + 
		QString::number(idle_bots) + " Idle | " + 
		QString::number(encounter_chance) + "% Ratio\n" + 
		QString::number(online_count_) + " Online | " + 
		QString::number(chats_started_) + " Started"
	);

	if (logging_checkbox_->checkState() != 0)
	{
		stats_logger_.Log(people_online, online_bots, chatting_bots, idle_bots, encounter_chance, online_count_);
	}
}

size_t PAChatManager::GetReadyToSearchSize()
{
	return ready_to_search.size();
}

void PAChatManager::onFilterReload()
{
	ReadFilterData(true);
	ReadAutoSenderData(true);
	chat_macros_->Reload();
}