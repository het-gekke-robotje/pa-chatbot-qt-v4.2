#include "PAChatGlobalId.h"

PAChatGlobalId::PAChatGlobalId()
	: current_id_(0)
{
}


PAChatGlobalId::~PAChatGlobalId()
{
}

int PAChatGlobalId::Get()
{
	static PAChatGlobalId* generator = nullptr;

	if (generator == nullptr)
	{
		generator = new PAChatGlobalId();
	}

	return generator->current_id_++;
}