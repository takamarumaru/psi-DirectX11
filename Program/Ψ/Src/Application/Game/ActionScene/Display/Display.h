#pragma once

#include "Application/Game/GameObject.h"

struct DisplayTexInfo
{
	SquarePolygon	m_texture;
	Math::Vector2	m_size;
	Math::Vector2	m_pos;
	float			m_angleZ;
};

class Display :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;;
	virtual void Update()override;
	virtual void DrawEffect()override;
	virtual void ImGuiUpdate()override;

private:
	//テクスチャ生成ウィンドウの更新
	void ImGuiTextureFactoryUpdate();
	//表示画像のリスト
	std::unordered_map<std::string, DisplayTexInfo> m_spDisplayTexList;

};