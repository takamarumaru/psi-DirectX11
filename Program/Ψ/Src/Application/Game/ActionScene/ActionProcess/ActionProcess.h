#pragma once

#include"../../GameProcess.h"

class ActionProcess :public GameProcess
{
public:
	ActionProcess(){}
	virtual ~ActionProcess() {}

	//初期化
	void Deserialize(const json11::Json& jsonObj)override;
	//2D描画
	void Draw2D()override;
	//更新
	void Update()override;
};