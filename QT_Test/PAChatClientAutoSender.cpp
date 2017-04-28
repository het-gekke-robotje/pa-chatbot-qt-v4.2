#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <utility>
#include "PAChatClientAutoSender.h"

using story_message = std::pair<int, QStringList>;
QList<story_message> story_messages;

void ReadAutoSenderData(bool reload)
{
	static bool loaded = false;
	if (!loaded || reload)
	{
		loaded = true;

		QFile f1("berichten/story_mode.txt");

		f1.open(QIODevice::ReadOnly | QIODevice::Text);

		QTextStream s1(&f1);
		s1.setCodec("UTF-8");

		story_messages.clear();
		QStringList story_messages_premade = s1.readAll().split('\n');
		for (auto &elem : story_messages_premade)
		{
			QStringList list = elem.split(" | ");
			if (list.size() >= 2)
			{
				int milliseconds = list[0].toInt();
				if (milliseconds < 1)
				{
					milliseconds = 1;
				}
				list.removeAt(0);
				story_messages.append(story_message(milliseconds, list));
			}
		}
	}
}

PAChatClientAutoSender::PAChatClientAutoSender(QObject *parent)
	: QObject(parent)
{
	ReadAutoSenderData();
	connect(&message_sender_, &QTimer::timeout, this, &PAChatClientAutoSender::processNextMessage);
}

PAChatClientAutoSender::~PAChatClientAutoSender()
{

}

QString PAChatClientAutoSender::GetIntroMessage()
{
	if (story_messages.size())
	{
		return GetMessage(0);
	}
	return "";
}

void PAChatClientAutoSender::Start(int current_story)
{
	current_story_ = current_story;
	current_index_ = 1;
	int start_time = 3500;
	if (story_messages.size())
	{
		start_time = story_messages[current_index_].first;
	}
	message_sender_.start(start_time);
}

void PAChatClientAutoSender::Stop()
{
	message_sender_.stop();
}

bool PAChatClientAutoSender::Stopped()
{
	return !message_sender_.isActive();
}

void PAChatClientAutoSender::processNextMessage()
{
	if (current_index_ < story_messages.size())
	{
		size_t l_index = current_index_++;
		size_t n_index = l_index + 1;

		bool last = !(current_index_ < story_messages.size());
		emit onRequestNewMessage(GetMessage(l_index), last);

		if (!last && n_index < story_messages.size())
		{
			message_sender_.setInterval(story_messages[n_index].first);
		}
	}
	else
	{
		Stop();
	}
}

int PAChatClientAutoSender::GetCurrentStory()
{
	return current_story_;
}

QString PAChatClientAutoSender::GetMessage(size_t index)
{
	if (index >= story_messages.size())
	{
		return "";
	}

	QStringList* list = &story_messages[index].second;

	if (list->size() == 0)
	{
		return "";
	}

	if (list->size() == 1)
	{
		return list->at(0);
	}

	if (current_story_ != -1 && current_story_ < list->size())
	{
		return list->at(current_story_);	
	}

	return list->at(qrand() % list->size());
}