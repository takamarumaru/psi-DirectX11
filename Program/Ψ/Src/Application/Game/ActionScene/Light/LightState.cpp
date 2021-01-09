#include "Light.h"

#include "../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"
#include"./Application/Component/SoundComponent.h"


/// 通常のライト===========================================================

	//データ読み込み
	void Light::NormalState::Deserialize(Light& rOwner, const json11::Json& jsonObj)
	{
		//自己発光を有効化
		rOwner.m_spModelComponent->SetEmissive(true);
	}
	//更新
	void Light::NormalState::Update(Light& rOwner){}

/// 壊れかけのライト=======================================================

	//データ読み込み
	void Light::BreakingState::Deserialize(Light& rOwner, const json11::Json& jsonObj)
	{
		//ランダム初期化
		srand(timeGetTime());
		//interval決定
		m_interval = rand() % 10 + 30;

		rOwner.m_spModelComponent->SetEmissive(true);
	}
	//更新
	void Light::BreakingState::Update(Light& rOwner)
	{
		//ランダムの間隔で一瞬ライトを消す

		m_count++;
		if (!m_isbreak)
		{
			if (m_count > m_interval)
			{
				rOwner.m_spModelComponent->SetEmissive(false);
				m_interval = rand() % 30 + 25;
				m_count = 0;
				m_isbreak = true;
			}
		}
		else
		{
			if (m_count>2)
			{
				rOwner.m_spModelComponent->SetEmissive(true);
				m_interval = rand() % 150 + 10;
				m_count = 0;
				m_isbreak = false;
				rOwner.m_spSoundComponent->SoundPlay("Data/Sound/OnLight.wav");
			}
		}
	}

/// センサーに反応して起動するライト=======================================

	//データ読み込み
	void Light::SensorState::Deserialize(Light& rOwner, const json11::Json& jsonObj)
	{
		//自己発光を無効化
		rOwner.m_spModelComponent->SetEmissive(false);
	}
	//更新
	void Light::SensorState::Update(Light& rOwner)
	{
		//指定範囲での当たり判定処理

		//球情報の作成
		SphereInfo info;
		info.m_pos = rOwner.m_mWorld.GetTranslation();
		info.m_radius = m_range;
		m_isSensing = false;

		for (auto& obj : SCENE.GetObjects())
		{
			//自分自身は無視
			if (obj.get() == (&rOwner)) { continue; }
			//TAG_Playerと球判定を行う
			if (!(obj->GetTag() & (TAG_Player))) { continue; }

			//プレイヤーが範囲内に入ったらライトつける
			if (obj->HitCheckBySphere(info))
			{
				m_isSensing = true;
			}
		}

		if (m_isSensing)
		{
			//自己発光を有効化
			rOwner.m_spModelComponent->SetEmissive(true);
		}
		else
		{
			//自己発光を無効化
			rOwner.m_spModelComponent->SetEmissive(false);
		}
	}