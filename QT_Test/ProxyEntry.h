#pragma once
#include <QtWidgets/QListWidget>
#include <QtCore/QObject>
#include <QtCore/QString>

class ProxyEntry : public QObject, public QListWidgetItem
{
	Q_OBJECT
private:
	QString host_;
	ushort port_;
	size_t use_count_;
public:
	ProxyEntry(const QString& host = "", ushort port = 1080, QObject *parent = Q_NULLPTR);
	~ProxyEntry();

	QString GetProxy();
	QString GetHost();
	ushort GetPort();
	QString ToString();

	size_t GetUseCount();
	void SetUseCount(size_t count);
	void PushUseCount();
	void PopUseCount();
};
