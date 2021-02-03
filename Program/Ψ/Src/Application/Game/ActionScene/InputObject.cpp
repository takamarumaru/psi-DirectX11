#include "InputObject.h"

#include "../Scene.h"

#include"Application/Component/ModelComponent.h"


void InputObject::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_rot = m_mWorld.GetAngles();

	//自己発光をオフに
	m_spModelComponent->SetEmissive(false);

	//レールのテクスチャ
	m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOff.png"));
	//レールのポイント
	auto& rRailsPos = jsonObj["RailsPoint"].array_items();
	for (auto&& point : rRailsPos)
	{
		//座標を保存
		Point3 pos =
		{
			point[0].int_value(),
			point[1].int_value(),
			point[2].int_value()
		};
		//補正値を保存
		Point3 offset =
		{
			point[3].int_value(),
			point[4].int_value(),
			point[5].int_value()
		};

		RailInfo info = { pos,offset };
		m_RailInfoList.push_back(info);

		//座標を保存
		Matrix mPoint;
		mPoint.CreateTranslation
		(
			(float)(point[0].int_value() + offset.x * 0.01f),
			(float)(point[1].int_value() + offset.y * 0.01f),
			(float)(point[2].int_value() + offset.z * 0.01f)
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
		point[0] = m_RailInfoList[i].pos.x;
		point[1] = m_RailInfoList[i].pos.y;
		point[2] = m_RailInfoList[i].pos.z;
		point[3] = m_RailInfoList[i].offset.x;
		point[4] = m_RailInfoList[i].offset.y;
		point[5] = m_RailInfoList[i].offset.z;

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
	m_rail.DrawDetached(0.5f,m_mWorld.GetAxisY());
}

void InputObject::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	//PointListを表示
	if (ImGui::CollapsingHeader("PointList", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < m_rail.GetPoints().size(); i++)
		{
			std::string pointNum = "point" + std::to_string(i);
			if (ImGui::TreeNodeEx(pointNum.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::LabelText("Pos","x:%d y:%d z:%d",m_RailInfoList[i].pos.x,m_RailInfoList[i].pos.y,m_RailInfoList[i].pos.z);
				ImGui::LabelText("Offset","x:%d y:%d z:%d",m_RailInfoList[i].offset.x,m_RailInfoList[i].offset.y,m_RailInfoList[i].offset.z);
				ImGui::TreePop();
			}
		}

		//PointListの最後尾を削除
		if (ImGui::Button("PointDelete"))
		{
			if (m_rail.GetPoints().size() > 0)
			{
				m_rail.DelPoint_Back();
				m_RailInfoList.pop_back();
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
				//座標を保存
				Point3 pos =
				{
					(int)newPointPos.x,
					(int)newPointPos.y,
					(int)newPointPos.z
				};
				//補正値を保存
				Point3 offset =
				{
					(int)newPointOffset.x,
					(int)newPointOffset.y,
					(int)newPointOffset.z
				};

				RailInfo info = { pos,offset };
				m_RailInfoList.push_back(info);

				Matrix mPoint;
				mPoint.CreateTranslation
				(
					(float)newPointPos.x + (newPointOffset.x * 0.01f),
					(float)newPointPos.y + (newPointOffset.y * 0.01f),
					(float)newPointPos.z + (newPointOffset.z * 0.01f)
				);
				m_rail.AddPointBack(mPoint);
			}

			ImGui::TreePop();
		}
	}
}
