#pragma once

#include"../GameProcess.h"

class MenuList;

class TitleProcess :public GameProcess
{
public:
	TitleProcess(){}
	virtual ~TitleProcess() {}

	void Deserialize(const json11::Json& jsonObj)override;
	void Draw2D()override;	//2D描画
	void Update()override;

private:
	//タイトルロゴ
	std::shared_ptr<Texture> m_spLogoTex;
};