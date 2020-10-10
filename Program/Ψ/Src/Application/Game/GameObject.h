﻿#pragma once

class ModelComponent;
class InputComponent;
class CameraComponent;

struct SphereInfo;
struct RayInfo;
struct SphereResult;

//タグ
enum OBJECT_TAG
{
	TAG_None			= 0x00000000,	//属性なし
	TAG_Character		= 0x00000001,	//キャラクター
	TAG_Player			= 0x00000002,	//プレイヤー
	TAG_StageObject		= 0x00000004,	//ステージ
	TAG_CanControlObject= 0x00000008,	//操作できるオブジェクト
};

class GameObject :public std::enable_shared_from_this<GameObject>
{
public:
	//コンストラクタ
	GameObject();
	//デストラクタ
	~GameObject();

/// 基本動作===============================================

	//データ読込・初期化
	virtual void Deserialize(const json11::Json& jsonObj);
	//データ保存
	virtual json11::Json::object Serialize();
	//更新
	virtual void Update();
	//描画
	virtual void Draw();
	//半透明物の描画
	virtual void DrawEffect();
	//ImGui更新
	virtual void ImGuiUpdate();

/// アクセサ===============================================

	//行列
	inline const Matrix& GetMatrix()const { return m_mWorld; }
	inline void SetMatrix(const Matrix& rMat) {m_mWorld = rMat; }
	//座標
	inline void SetPos(const Vector3& rPos) { m_pos = rPos; }
	//移動量
	inline void SetForce(const Vector3& rForce) { m_force = rForce; }
	//生死
	inline bool IsAlive() const { return m_isAlive; }
	inline void Destroy() { m_isAlive = false; }
	//重力の影響
	inline void OnFall() { m_isFall = true; }
	inline void OffFall() { m_isFall = false; }
	//タグ
	inline UINT SetTag(UINT tag){m_tag=tag; }
	inline UINT GetTag()const { return m_tag; }
	//名前
	inline const char* GetName() const { return m_name.c_str(); }
	//モデルコンポーネント
	std::shared_ptr<ModelComponent>GetModelComponent() { return m_spModelComponent; }
	//着地しているかどうか
	bool IsGround() { return m_isGround; }

///	当たり判定=============================================

	//レイによる当たり判定
	bool HitCheckByRay(const RayInfo& rInfo, RayResult& rResult);
	//球による当たり判定（mesh）
	bool HitCheckBySphereVsMesh(const SphereInfo& rInfo, SphereResult& rResult);

protected:
	//解放
	virtual void Release();

///	コンポーネント=========================================

	//インプットコンポーネント
	std::shared_ptr<InputComponent> m_spInputComponent = nullptr;
	//カメラコンポーネント
	std::shared_ptr<CameraComponent> m_spCameraComponent = nullptr;
	//モデルコンポーネント
	std::shared_ptr<ModelComponent> m_spModelComponent = std::make_shared<ModelComponent>(*this);

///	当たり判定=============================================

	//地面（下方向）とのレイ判定
	bool CheckGround(RayResult& downRayResult,float& rDstDistance, UINT rTag);
	//歩いて乗り越えられる段差の高さ
	static const float s_allowToStepHeight;
	//地面から足が離れていても着地していると判定する高さ（坂道などを登るときに宙に浮くのを防ぐ）
	static const float s_landingHeight;
	//着地しているかどうか
	bool m_isGround=false;

	//球面判定
	bool CheckBump(Vector3 rCenterOffset);

/// オブジェクトデータ=====================================

	//行列
	Matrix m_mWorld;
	//ひとつ前の行列
	Vector3 m_prevPos;
	//座標
	Vector3 m_pos;
	//移動量
	Vector3 m_force;
	//回転角度
	Vector3 m_rot;

	//タグ
	UINT		m_tag = OBJECT_TAG::TAG_None;
	//クラス名
	std::string m_className = "GameObject";
	//名前
	std::string m_name = "GameObject";
	//モデルパス名
	std::string m_modelFilePath;
	//プレハブjson
	std::string m_prefab;

	//生死フラグ
	bool		m_isAlive = true;
	//重力の影響を受けるか
	bool		m_isFall = true;

	//全体の拡縮
	float		m_allScale = 1.0f;
	//json読み込み時の拡縮
	Vector3		m_defScale = { 1.0f,1.0f,1.0f };

	//球の当たり判定半径
	float m_radius = 1.0f;
};

//クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

//球の情報
struct SphereInfo
{
	Vector3 m_pos;
	float m_radius = 0.0f;
};

//レイの情報
struct RayInfo
{
	Vector3	m_pos;				//レイ（光線）の発射場所
	Vector3	m_dir;				//レイの発射方法
	float	mMaxRange = 0.0f;	//レイが届く最大距離
};

//球面判定の結果データ
struct SphereResult
{
	Vector3 m_push;
	bool m_hit = false;
};