#pragma once
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QFile>

class PAStatisticsLogger : public QObject
{
	Q_OBJECT
private:
	QString filename_;
	QPointer<QFile> file_;

	int people_online_;
	int total_bots_;
	int chatting_bots_;
	int idle_bots_;
	int encounter_chance_;
	int total_online_count_;
public:
	PAStatisticsLogger(QObject *parent);
	~PAStatisticsLogger();

	void Log(int people_online, int total_bots, int chatting_bots, int idle_bots, int encounter_chance, int total_online_count);
};

