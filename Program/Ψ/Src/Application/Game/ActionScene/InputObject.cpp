#include "InputObject.h"

#include "../Scene.h"

#include"Application/Component/ModelComponent.h"


void InputObject::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//自己発光をオフに
	m_spModelComponent->SetEmissive(false);

	//レールのテクスチャ
	m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOff.png"));
	//レールのポイント
	auto& rRailsPos = jsonObj["RailsPoint"].array_items();
	for (auto&& point : rRailsPos)
	{
		//補正値を保存
		Vector3 offset =
		{
			point[3].int_value() * 0.01f,
			point[4].int_value() * 0.01f,
			point[5].int_value() * 0.01f
		};
		m_offsetList.push_back(offset);

		//座標を保存
		Matrix mPoint;
		mPoint.CreateTranslation
		(
			(float)point[0].int_value() + offset.x,
			(float)point[1].int_value() + offset.y,
			(float)point[2].int_value() + offset.z
		);
		m_rail.AddPointBack(mPoint);
	}
}

json11::Json::object InputObject::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	json11::Json::array points(m_rail.getNumPoints());

	//レールのポイント
	for (UINT i = 0; i < m_rail.getNumPoints(); i++)
	{
		json11::Json::array point(6);
		point[0] = (int)m_rail.GetPoints()[i].Translation().x;
		point[1] = (int)m_rail.GetPoints()[i].Translation().y;
		point[2] = (int)m_rail.GetPoints()[i].Translation().z;
		point[3] = (int)(m_offsetList[i].x * 100);
		point[4] = (int)(m_offsetList[i].y * 100);
		point[5] = (int)(m_offsetList[i].z * 100);

		points[i] = point;
	}

	objectData["RailsPoint"] = points;

	return objectData;
}

void InputObject::DrawEffect()
{
	//レールの描画
	SHADER.m_effectShader.SetWorldMatrix(Matrix());
	SHADER.m_effectShader.WriteToCB();
	m_rail.DrawDetached(0.5f);
}

void InputObject::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	//PointListを表示
	if (ImGui::TreeNodeEx("RailPointList", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < m_rail.GetPoints().size(); i++)
		{
			std::string pointNum = "point" + std::to_string(i);
			if (ImGui::TreeNodeEx(pointNum.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::LabelText
				(
					"",
					"x:%d y:%d z:%d",
					(int)m_rail.GetPoints()[i].Translation().x,
					(int)m_rail.GetPoints()[i].Translation().y,
					(int)m_rail.GetPoints()[i].Translation().z
				);
				ImGui::TreePop();
			}
		}

		//PointListの最後尾を削除
		if (ImGui::Button("PointDelete"))
		{
			if (m_rail.GetPoints().size() > 0)
			{
				m_rail.DelPoint_Back();
			}
		}

		if (ImGui::TreeNodeEx("CreatePoint", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static Vector3 newPointPos;
			static Vector3 newPointOffset;

			ImGui::DragFloat3("pos", &newPointPos.x, 1.0f);
			ImGui::DragFloat3("Offset", &newPointOffset.x, 1.0f);

			//ポイントにデバックスフィアを追加
			SCENE.AddDebugSphereLine(newPointPos, 0.2f, Math::Color(1, 0, 1, 1));

			if (ImGui::Button("PointCreate"))
			{
				Matrix mPoint;
				mPoint.CreateTranslation
				(
					(float)newPointPos.x + (newPointOffset.x * 0.01f),
					(float)newPointPos.y + (newPointOffset.y * 0.01f),
					(float)newPointPos.z + (newPointOffset.z * 0.01f)
				);
				m_rail.AddPointBack(mPoint);

				//補正値を保存
				m_offsetList.push_back
				(
					Vector3(
						newPointOffset.x * 0.01f,
						newPointOffset.y * 0.01f,
						newPointOffset.z * 0.01f
					)
				);
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
