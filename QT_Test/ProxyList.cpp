#include "ProxyList.h"

QT_USE_NAMESPACE

ProxyList::ProxyList(QListWidget* proxy_entry_item_list_widget, QLineEdit* proxy_entries_textedit, QPushButton* proxy_entries_submit_button, QObject *parent)
	: QObject(parent), list_view_(proxy_entry_item_list_widget), input_area_(proxy_entries_textedit), input_submit_(proxy_entries_submit_button)
{
	connect(input_submit_, &QPushButton::clicked, this, &ProxyList::InputSubmit);
}

ProxyList::~ProxyList()
{

}

bool ProxyList::Add(ProxyEntry* entry)
{
	auto exists = entries_.find(entry);

	if (exists == entries_.end())
	{
		for (auto& i : entries_)
		{
			if (entry->GetPort() == i->GetPort() && entry->GetHost().toLower() == i->GetHost().toLower())
			{
				return false;
			}
		}

		entries_.insert(entry);
		list_view_->addItem(entry);
		return true;
	}

	return false;
}

bool ProxyList::Remove(ProxyEntry* entry)
{
	auto exists = entries_.find(entry);

	if (exists != entries_.end())
	{
		entries_.erase(exists);
		list_view_->removeItemWidget(entry);
		return true;
	}

	return false;
}

ProxyEntry* ProxyList::GetLowestCountEntry()
{
	if (entries_.size() == 0)
	{
		return nullptr;
	}

	ProxyEntry* lowest = *entries_.begin();
	if (!lowest)
	{
		return nullptr;
	}

	size_t lowest_count = lowest->GetUseCount();

	for (auto& i : entries_)
	{
		if (i && i->GetUseCount() < lowest_count)
		{
			lowest_count = i->GetUseCount();
			lowest = i;
		}
	}

	return lowest;
}

bool ProxyList::IsFull(size_t max_count)
{
	size_t entries_full = 0;
	for (auto& i : entries_)
	{
		if (i && i->GetUseCount() >= max_count)
		{
			++entries_full;
		}
	}

	return entries_full == entries_.size();
}

void ProxyList::InputSubmit(bool checked)
{
	QString data(input_area_->text());
	QStringList proxy_list = data.split(";");
	for (auto& entry : proxy_list)
	{
		auto proxy_entry = entry.split(':');
		bool succeeded = false;
		if (proxy_entry.size() == 2)
		{
			QString host = proxy_entry[0];
			ushort port = proxy_entry[1].toInt(&succeeded);
			if (succeeded)
			{
				Add(new ProxyEntry(host, port, this));
			}
		}
	}
	proxy_list = data.split("\n");
	for (auto& entry : proxy_list)
	{
		auto proxy_entry = entry.split(':');
		bool succeeded = false;
		if (proxy_entry.size() == 2)
		{
			QString host = proxy_entry[0];
			ushort port = proxy_entry[1].toInt(&succeeded);
			if (succeeded)
			{
				Add(new ProxyEntry(host, port, this));
			}
		}
	}
	proxy_list = data.split(" ");
	for (auto& entry : proxy_list)
	{
		auto proxy_entry = entry.split(':');
		bool succeeded = false;
		if (proxy_entry.size() == 2)
		{
			QString host = proxy_entry[0];
			ushort port = proxy_entry[1].toInt(&succeeded);
			if (succeeded)
			{
				Add(new ProxyEntry(host, port, this));
			}
		}
	}
}

void ProxyList::SetText(const QString& text)
{
	input_area_->setText(text);
}