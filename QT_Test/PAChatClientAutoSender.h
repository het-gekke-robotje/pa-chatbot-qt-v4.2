#pragma once
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QString>
class PAChatClientAutoSender : public QObject
{
	Q_OBJECT
private:
	QTimer message_sender_;
	size_t current_index_;
	int current_story_;
	QString GetMessage(size_t index);

public:
	PAChatClientAutoSender(QObject *parent = Q_NULLPTR);
	~PAChatClientAutoSender();

	void Start(int current_story = -1);
	void Stop();
	bool Stopped();
	QString GetIntroMessage();
	int GetCurrentStory();

Q_SIGNALS:
	void onRequestNewMessage(QString message, bool last);

private Q_SLOTS:
	void processNextMessage();
};

void ReadAutoSenderData(bool reload = false);