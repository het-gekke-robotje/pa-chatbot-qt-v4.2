#pragma once
#include <set>
#include "PAChatClientGlue.h"

class PAChatClientStats
{
private:
	std::set<PAChatClientGlue*> clients_;
	std::set<PAChatClientGlue*> disconnected_;
	std::set<PAChatClientGlue*> connected_;
	std::set<PAChatClientGlue*> ready_;
	std::set<PAChatClientGlue*> chatting_;

public:
	PAChatClientStats();
	~PAChatClientStats();

	void Add(PAChatClientGlue* client);
	void Remove(PAChatClientGlue* client);

	void SetDisconnected(PAChatClientGlue* client);
	void SetConnected(PAChatClientGlue* client);
	void SetReady(PAChatClientGlue* client);
	void SetChatting(PAChatClientGlue* client);

};
