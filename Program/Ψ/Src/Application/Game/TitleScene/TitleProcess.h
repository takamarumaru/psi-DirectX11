#pragma once

#include"../GameProcess.h"

class TitleProcess :public GameProcess
{
public:
	TitleProcess(){}
	virtual ~TitleProcess() {}

	void Deserialize(const json11::Json& jsonObj)override;
	void Update()override;
};