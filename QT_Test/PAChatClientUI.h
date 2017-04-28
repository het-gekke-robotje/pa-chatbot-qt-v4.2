#pragma once
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtCore/QPointer>

class PAChatClientUI : public QObject
{
	Q_OBJECT
private:
	QPointer<QWidget> tab_;
	QPointer<QGridLayout> tab_grid_layout_;
	QPointer<QGridLayout> chat_container_grid_;
	QPointer<QHBoxLayout> chat_container_button_grid_;
	QPointer<QPushButton> chat_manager_bot_remove_;
	QPointer<QPushButton> chat_manager_keep_chat_;
	QPointer<QPushButton> chat_manager_end_chat_;
	QPointer<QPushButton> chat_manager_send_;
	QPointer<QPushButton> chat_manager_stop_auto_sender_;
	QPointer<QPlainTextEdit> chat_box_text_messages_;
	QPointer<QLineEdit> chat_box_text_input_message_;

	QPointer<QTabWidget> tabs_container_;
	int tab_number_;
public:
	PAChatClientUI(QTabWidget* tabs_container, int show_id = -1, QObject *parent = Q_NULLPTR);
	~PAChatClientUI();

	void ScrollToBottom();
	void ScrollToTop();

	void AddMessage(bool me, const QString& message);
	void AddChatSeparator();
	void RemoveMessages();
	void ClearMessageInput();
	QWidget* GetTab();
	void FocusInputText();

Q_SIGNALS:
	void onRequestRemoveBot();
	void onRequestChatSendMessage(QString message);
	void onRequestChatEnd();
	void onRequestChatKeep();
	void onTextInputChanged(QString text);
	void onRequestStopAutoSender();

private Q_SLOTS:
	void onChatManagerBotRemoveButtonClicked();
	void onChatManagerKeepChatButtonClicked();
	void onChatManagerEndChatButtonClicked();
	void onChatManagerSendChatButtonClicked();
	void onChatManagerTextEdited(QString text);
	void onChatManagerAutoSenderStop();
};
