#pragma once
class PAChatGlobalId
{
private:
	int current_id_;

	PAChatGlobalId();
	~PAChatGlobalId();
public:
	static int Get();
};
