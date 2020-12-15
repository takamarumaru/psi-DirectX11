#include "Target.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Target::Deserialize(const json11::Json& jsonObj)
{
	InputObject::Deserialize(jsonObj);

	//復帰時間取得
	if (jsonObj["ReturnTime"].is_null() == false)
	{
		m_returnTime = jsonObj["ReturnTime"].number_value();
	}
}

json11::Json::object Target::Serialize()
{
	json11::Json::object objectData = InputObject::Serialize();

	objectData["ReturnTime"] = m_returnTime;

	return objectData;
}

void Target::Update()
{
	GameObject::Update();

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//当たり判定
	UpdateCollision();

	//復帰処理
	if (m_isPush == true)
	{
		//現在時刻の取得
		m_nowTime = std::chrono::system_clock::now();
		//開始時刻との差を計測
		float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_nowTime - m_startTime).count();
		//設定時間を超えたら復帰
		if(elapsedTime / 1000.0f >= m_returnTime)
		{
			//フラグを降ろす
			m_isPush = false;
			//Offアニメーション開始
			SetAnimation("Off", false);
			//レールをOffに
			m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOff.png"));
		}
	}

	//回転
	m_mWorld.CreateRotation(m_rot);
	//座標
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}

void Target::UpdateCollision()
{
	//全てのオブジェクトと四角判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//TAG_Characterのみ
		if (!(obj->GetTag() & (TAG_Character))) { continue; }

		SphereInfo info;
		info.m_pos = m_mWorld.GetTranslation() + Vector3(0,4.0f,0);
		info.m_radius = m_radius;

		SCENE.AddDebugSphereLine(info.m_pos, info.m_radius);

		if (obj->HitCheckBySphere(info))
		{
			if (m_isPush == false)
			{
				if (m_animator.IsAnimationEnd())
				{
					//フラグを立てる
					m_isPush = true;
					//Onアニメーション開始
					SetAnimation("On", false);
					//レールをOnに
					m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOn.png"));
					//復帰までの時間計測開始
					m_startTime = std::chrono::system_clock::now();
				}
			}
			return;
		}
	}
}