#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include "PAChatClientFilter.h"

QStringList disallowed_contents;
QStringList disallowed_contents_ai;
QStringList disallowed_begins;
QStringList disallowed_begins_ai;

void ReadFilterData(bool reload)
{
	static bool loaded = false;
	if (!loaded || reload)
	{
		loaded = true;

		QFile f1("berichten/forbidden_sentences.txt");
		QFile f2("berichten/forbidden_start.txt");
		QFile f3("berichten/forbidden_sentences_ai.txt");
		QFile f4("berichten/forbidden_start_ai.txt");

		f1.open(QIODevice::ReadOnly | QIODevice::Text);
		f2.open(QIODevice::ReadOnly | QIODevice::Text);
		f3.open(QIODevice::ReadOnly | QIODevice::Text);
		f4.open(QIODevice::ReadOnly | QIODevice::Text);

		QTextStream s1(&f1);
		QTextStream s2(&f2);
		QTextStream s3(&f3);
		QTextStream s4(&f4);

		disallowed_contents = s1.readAll().split('\n');
		disallowed_begins = s2.readAll().split('\n');
		disallowed_contents_ai = s3.readAll().split('\n');
		disallowed_begins_ai = s4.readAll().split('\n');
	}
}

PAChatClientFilter::PAChatClientFilter(QObject *parent)
	: QObject(parent)
{
	ReadFilterData();
}

PAChatClientFilter::~PAChatClientFilter()
{

}

bool PAChatClientFilter::IsMessageFiltered_Base(const QStringList& content, const QStringList& begins, QString& message)
{
	if (message.length() > 1536)
	{
		return true;
	}

	message = message.toLower();

	for (char i = '0'; i <= '9'; ++i)
	{
		message = message.replace(i, '@');
	}

	for (auto& forbidden_content : content)
	{
		if (message.indexOf(forbidden_content) != -1)
		{
			return true;
		}
	}

	for (int i = 0; i < message.length(); ++i)
	{
		if (message[i].isLetterOrNumber())
		{
			message = message.mid(i);
			break;
		}
	}
	
	for (auto& forbidden_content : begins)
	{
		int maxsize = forbidden_content.indexOf(">");
		QString forbidden_begin = maxsize == -1 ? forbidden_content : QString(forbidden_content).remove(">");
		if (message.startsWith(forbidden_begin))
		{	
			if (maxsize == -1 || message.length() == maxsize)
			{
				return true;
			}
		}
	}

	return false;
}

bool PAChatClientFilter::IsMessageFiltered(QString message)
{
	return IsMessageFiltered_Base(disallowed_contents, disallowed_begins, message);
}

bool PAChatClientFilter::IsMessageFilteredAI(QString message)
{
	return IsMessageFiltered_Base(disallowed_contents_ai, disallowed_begins_ai, message);
}
