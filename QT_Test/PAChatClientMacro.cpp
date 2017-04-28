#include <QtCore/QTextStream>
#include <QtCore/QFile>

#include "PAChatClientMacro.h"

QStringList macro_messages;

static bool ReadData(bool reload = false)
{
	static bool loaded = false;
	if (!loaded || reload)
	{
		loaded = true;

		QFile f1("berichten/info.txt");

		f1.open(QIODevice::ReadOnly | QIODevice::Text);

		QTextStream s1(&f1);

		macro_messages = s1.readAll().split('\n');

		return true;
	}
	return false;
}

PAChatClientMacro::PAChatClientMacro(QListWidget* macro_list, QObject *parent)
	: QObject(parent), macro_list_(macro_list)
{
	Load();

	if (macro_list_)
	{
		connect(macro_list_, &QListWidget::itemDoubleClicked, this, &PAChatClientMacro::onItemDoubleClicked);
	}
}

PAChatClientMacro::~PAChatClientMacro()
{

}

bool PAChatClientMacro::Load()
{
	if (ReadData() && macro_list_)
	{
		for (auto& message : macro_messages)
		{
			macro_list_->addItem(message);
		}
		return true;
	}
	return false;
}

bool PAChatClientMacro::Reload()
{
	if (ReadData(true) && macro_list_)
	{
		macro_list_->clear();
		for (auto& message : macro_messages)
		{
			macro_list_->addItem(message);
		}
		return true;
	}
	return false;
}

void PAChatClientMacro::onItemDoubleClicked(QListWidgetItem * item)
{
	if (item)
	{
		emit onMacroRequested(item->text());
	}
}