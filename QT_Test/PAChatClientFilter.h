#pragma once
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QString>

class PAChatClientFilter: public QObject
{
	Q_OBJECT
private:
	bool IsMessageFiltered_Base(const QStringList& content, const QStringList& begins, QString& message);
public:
	PAChatClientFilter(QObject *parent = Q_NULLPTR);
	~PAChatClientFilter();

	bool IsMessageFiltered(QString message);
	bool IsMessageFilteredAI(QString message);
};

void ReadFilterData(bool reload = false);