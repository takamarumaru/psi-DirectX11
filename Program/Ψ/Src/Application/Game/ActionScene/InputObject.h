#pragma once

#include "../GameObject.h"

class InputObject :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void DrawEffect()override;
	virtual void ImGuiUpdate()override;//ImGui更新

	inline bool GetIsPush() { return m_isPush; }

protected:

	//起動判定
	bool m_isPush = false;

	//出力オブジェクトまでのレール
	TrailPolygon	m_rail;

	//各ポイントの補正値
	std::vector<Vector3> m_offsetList;
};