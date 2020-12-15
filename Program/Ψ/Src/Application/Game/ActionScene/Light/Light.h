#pragma once

#include "./Application/Game/GameObject.h"

enum LIGHT_STATE
{
	LS_NONE,
	LS_NORMAL,
	LS_BREAKING,
	LS_SENSOR
};

class Light :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;
	virtual void DrawShadowMap()override {};

private:

	//光の色
	Vector3 m_lightColor = {0,0,0};

	//光の強さ
	UINT m_lightPower = 0.0f;

///ステート管理=====================================
	//基底アクションステート
	class BaseState
	{
	public:
		virtual void Deserialize(Light& rOwner,const json11::Json& jsonObj) = 0;
		virtual void Update(Light& rOwner) = 0;
	};

	//通常のライト
	class NormalState :public BaseState
	{
	public:
		virtual void Deserialize(Light& rOwner, const json11::Json& jsonObj)override;
		virtual void Update(Light& rOwner)override;
	};

	//壊れかけのライト
	class BreakingState :public BaseState
	{
	public:
		virtual void Deserialize(Light& rOwner, const json11::Json& jsonObj)override;
		virtual void Update(Light& rOwner)override;
	private:
		int m_count = 0;	//進みつ続けるカウント
		int m_interval = 0;	//ランダムで決まる間隔
	};

	//センサーライト
	class SensorState :public BaseState
	{
	public:
		virtual void Deserialize(Light& rOwner, const json11::Json& jsonObj)override;
		virtual void Update(Light& rOwner)override;
	private:
		bool m_isSensing = false;
		int m_range = 10;
	};


	//アクションステート
	std::shared_ptr<BaseState> m_spState;
	//ステートタグ
	UINT m_stateTag = LIGHT_STATE::LS_NONE;
};