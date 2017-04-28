#include "qt_test.h"

QT_Test::QT_Test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	manager = new PAChatManager(
		ui.chatProxyList, 
		ui.chatProxyInput, 
		ui.chatProxySubmit, 
		ui.chatBoxPeers,
		ui.chatBotTabs, 
		ui.chatManagerAddBot, 
		ui.chatManagerRequestNewChat, 
		ui.autoSearchCheckBox,
		ui.autoSendIntroductionMessage, 
		ui.autoSenderState, 
		ui.preparedTextsBox,
		ui.chatManagerFillBots,
		ui.onlineCountLabel,
		ui.autoAIModeCheckBox,
		ui.endFilteredChatsState,
		ui.unneededChatsHiderState,
		ui.reloadFiltersSubmit,
		ui.logginOptionCheckBox,
		ui.allowStopCheckBox,
		ui.cleanProxyListButton,
		this);
}

QT_Test::~QT_Test()
{

}
