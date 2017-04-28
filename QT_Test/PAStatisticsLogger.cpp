#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include "PAStatisticsLogger.h"

PAStatisticsLogger::PAStatisticsLogger(QObject *parent) : QObject(parent)
{
	filename_ = "stats_logs_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".csv";
	file_ = new QFile(filename_);
	file_->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
}

PAStatisticsLogger::~PAStatisticsLogger()
{
	file_->close();
}

void PAStatisticsLogger::Log(int people_online, int total_bots, int chatting_bots, int idle_bots, int encounter_chance, int total_online_count)
{
	if (people_online != people_online_ ||
		total_bots != total_bots_ ||
		chatting_bots != chatting_bots_ ||
		idle_bots != idle_bots_ ||
		encounter_chance != encounter_chance_ ||
		total_online_count != total_online_count_)
	{
		people_online_ = people_online;
		total_bots_ = total_bots;
		chatting_bots_ = chatting_bots;
		idle_bots_ = idle_bots;
		encounter_chance_ = encounter_chance;
		total_online_count_ = total_online_count;

		QString to_write(
			QString::number(QDateTime::currentMSecsSinceEpoch()) + "," +
			QString::number(people_online_) + "," +
			QString::number(total_bots_) + "," +
			QString::number(chatting_bots_) + "," +
			QString::number(idle_bots_) + "," +
			QString::number(encounter_chance_) + "," +
			QString::number(total_online_count_) + "\r\n"
		);

		QTextStream(file_) << to_write;
		file_->flush();
	}
}