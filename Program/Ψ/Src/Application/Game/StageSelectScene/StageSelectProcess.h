#pragma once

#include"../GameProcess.h"

class MenuList;

class StageSelectProcess :public GameProcess
{
public:
	StageSelectProcess(){}
	virtual ~StageSelectProcess() {}

	void Deserialize(const json11::Json& jsonObj)override;
	void Draw2D()override;	//2D描画
	void Update()override;

private:
	//ステージセレクトテキスト
	std::shared_ptr<Texture> m_spStageSelectTex;
};