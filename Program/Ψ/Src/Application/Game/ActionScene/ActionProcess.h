#pragma once

#include"../GameProcess.h"

class ActionProcess :public GameProcess
{
public:
	ActionProcess(){}
	virtual ~ActionProcess() {}

	void Deserialize(const json11::Json& jsonObj)override;
	void Update()override;
};