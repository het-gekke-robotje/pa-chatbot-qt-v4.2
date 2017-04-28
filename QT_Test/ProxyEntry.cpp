#include "ProxyEntry.h"

QT_USE_NAMESPACE

ProxyEntry::ProxyEntry(const QString& host, ushort port, QObject *parent)
	: QObject(parent), use_count_(0), host_(host), port_(port)
{
	QListWidgetItem::setText(ToString());
}

ProxyEntry::~ProxyEntry()
{

}

QString ProxyEntry::GetProxy()
{
	return host_ + ":" + QString::number(port_);
}

QString ProxyEntry::GetHost()
{
	return host_;
}

ushort ProxyEntry::GetPort()
{
	return port_;
}

QString ProxyEntry::ToString()
{
	return host_ + ":" + QString::number(port_) + "(" + QString::number(use_count_) + ")";
}

size_t ProxyEntry::GetUseCount()
{
	return use_count_;
}

void ProxyEntry::SetUseCount(size_t count)
{
	use_count_ = count;
	QListWidgetItem::setText(ToString());
}

void ProxyEntry::PushUseCount()
{
	++use_count_;
	QListWidgetItem::setText(ToString());
}

void ProxyEntry::PopUseCount()
{
	--use_count_;
	QListWidgetItem::setText(ToString());
}