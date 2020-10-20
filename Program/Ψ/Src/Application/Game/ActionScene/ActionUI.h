#pragma once

#include"../GameObject.h"

class ActionUI :public GameObject
{
public:
	ActionUI() {}
	virtual ~ActionUI() {}

	//初期化
	void Deserialize(const json11::Json& jsonObj)override;
	//2D描画
	void Draw2D()override;
	//更新
	void Update()override;
private:
};