#include "GameObject.h"

#include"./Scene.h"

#include "../Component/ModelComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/SoundComponent.h"


#include "TitleScene/TitleProcess.h"

#include "ActionScene/ActionProcess/ActionProcess.h"
#include "ActionScene/Box/Box.h"
#include "ActionScene/Ball/Ball.h"
#include "ActionScene/Button/Button.h"
#include "ActionScene/Spring/Spring.h"
#include "ActionScene/Door/ManualDoor/ManualDoor.h"
#include "ActionScene/Door/AutomaticDoor/AutomaticDoor.h"
#include "ActionScene/Target/Target.h"
#include "ActionScene/Goal/Goal.h"
#include "ActionScene/Light/Light.h"
#include "ActionScene/Player/Player.h"

#include "StageSelectScene/StageSelectProcess.h"

#include "UI/MENU/MenuList.h"

const float GameObject::s_allowToStepHeight = 0.8f;
const float GameObject::s_landingHeight = 0.1f;

GameObject::GameObject(){}

GameObject::~GameObject()
{
	Release();
}

//初期化
void GameObject::Deserialize(const json11::Json& jsonObj)
{
	if (jsonObj.is_null()) { return; }

	//タグ
	if (jsonObj["Tag"].is_null() == false)
	{
		m_tag = jsonObj["Tag"].int_value();
	}
	//クラス名
	if (jsonObj["ClassName"].is_null() == false)
	{
		m_className = jsonObj["ClassName"].string_value();
	}
	//名前
	if (jsonObj["Name"].is_null() == false)
	{
		m_name = jsonObj["Name"].string_value();
	}
	//モデルのファイルパス
	if (jsonObj["ModelFilePath"].is_null() == false)
	{
		m_modelFilePath = jsonObj["ModelFilePath"].string_value();
	}
	//プレハブファイルパス
	if (jsonObj["Prefab"].is_null() == false)
	{
		m_prefab = jsonObj["Prefab"].string_value();
	}
	//弾性力
	if (jsonObj["Elastic"].is_null() == false)
	{
		m_elastic = jsonObj["Elastic"].number_value();
	}


	//モデル--------------------------------------
	if (m_spModelComponent)
	{
		//モデルをセット
		m_spModelComponent->SetModel(ResFac.GetModel(m_modelFilePath));
	}

	//行列----------------------------------------
	Matrix mTrans, mRotate, mScale;

	//座標
	const std::vector<json11::Json>& rPos = jsonObj["Pos"].array_items();
	if (rPos.size() == 3)
	{
		mTrans.CreateTranslation
		(
			(float)rPos[0].number_value(),
			(float)rPos[1].number_value(),
			(float)rPos[2].number_value()
		);
	}

	//回転
	const std::vector<json11::Json>& rRot = jsonObj["Rot"].array_items();
	if (rRot.size() == 3)
	{

		m_rot.x = rRot[0].int_value() * ToRadians;
		m_rot.y = rRot[1].int_value() * ToRadians;
		m_rot.z = rRot[2].int_value() * ToRadians;

		mRotate.CreateRotation(m_rot);
	}

	//拡縮
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	const std::vector<json11::Json>& rScaleOffset = jsonObj["ScaleOffset"].array_items();
	Vector3 scale;
	if (rScale.size() == 3)
	{
		//拡縮度を代入
		scale.x = (float)rScale[0].number_value();
		scale.y = (float)rScale[1].number_value();
		scale.z = (float)rScale[2].number_value();
		//補正値があった場合
		if (rScaleOffset.size() == 3)
		{
			scale.x += (float)rScaleOffset[0].number_value() * 0.01f;
			scale.y += (float)rScaleOffset[1].number_value() * 0.01f;
			scale.z += (float)rScaleOffset[2].number_value() * 0.01f;

			mTrans.Move
			(
				(float)rScaleOffset[0].number_value() * 0.01f / 2,
				(float)rScaleOffset[1].number_value() * 0.01f / 2,
				(float)rScaleOffset[2].number_value() * 0.01f / 2
			);
		}
		mScale.CreateScalling(scale);
	}

	m_mWorld = mScale * mRotate * mTrans;

	//中心座標への調整値
	const std::vector<json11::Json>& rCenterOffset = jsonObj["CenterOffset"].array_items();
	if (rCenterOffset.size() == 3)
	{
		m_centerOffset.x = (float)rCenterOffset[0].number_value();
		m_centerOffset.y = (float)rCenterOffset[1].number_value();
		m_centerOffset.z = (float)rCenterOffset[2].number_value();
	}

	//AudioEngineの初期化
	m_spSoundComponent->Init();
	// 音声のリストを取得
	auto& soundList = jsonObj["SoundList"].array_items();
	for (auto&& sound: soundList)
	{
		m_spSoundComponent->SoundLoad(sound[0].string_value().c_str(), sound[1].int_value() * 0.01f);
	}

	//当たり判定半径
	if (jsonObj["Radius"].is_null() == false)
	{
		m_radius = (float)jsonObj["Radius"].number_value();
	}

	//情報を格納
	m_pos = mTrans.GetTranslation();
	m_scale = mScale.GetScale();
}

//データ保存
json11::Json::object GameObject::Serialize()
{
	json11::Json::object objectData;

	objectData["ClassName"] = m_className;			//クラス名
	objectData["Name"] = m_name;					//名前
	if (!m_modelFilePath.empty())
	{
		objectData["ModelFilePath"] = m_modelFilePath;	//モデルパス
	}
	objectData["Tag"] = (int)m_tag;					//タグ

	//座標
	json11::Json::array mat(3);
	mat[0] = (int)m_pos.x;
	mat[1] = (int)m_pos.y;
	mat[2] = (int)m_pos.z;

	objectData["Pos"] = mat;

	//回転
	mat[0] = (int)(m_rot.x * ToDegrees);
	mat[1] = (int)(m_rot.y * ToDegrees);
	mat[2] = (int)(m_rot.z * ToDegrees);

	objectData["Rot"] = mat;

	//拡縮
	mat[0] = (int)m_scale.x;
	mat[1] = (int)m_scale.y;
	mat[2] = (int)m_scale.z;

	objectData["Scale"] = mat;

	////効果音
	//json11::Json::array soundList(m_spSoundComponent->GetVolumeList().size());
	//UINT soundIdx = 0;
	////サウンドリストからデータを取得
	//for (auto&& sound : m_spSoundComponent->GetVolumeList())
	//{
	//	json11::Json::array soundjson(2);
	//	soundjson[0] = sound.first;
	//	soundjson[1] = sound.second*100.0f;

	//	soundList[soundIdx] =soundjson;
	//	soundIdx++;
	//}

	//objectData["SoundList"] = soundList;
	
	//プレハブ指定がある場合
	if (!m_prefab.empty())
	{
		objectData["Prefab"] = m_prefab;

		json11::Json prefabJson = ResFac.GetJSON(m_prefab);

		//クラス名
		if (prefabJson["ClassName"] == objectData["ClassName"])
		{
			objectData.erase("ClassName");
		}
		//モデルファイルパス
		if (prefabJson["ModelFilePath"] == objectData["ModelFilePath"])
		{
			objectData.erase("ModelFilePath");
		}
		//タグ
		if (prefabJson["Tag"] == objectData["Tag"])
		{
			objectData.erase("Tag");
		}
		
		//座標
		if (prefabJson["Pos"] == objectData["Pos"])
		{
			objectData.erase("Pos");
		}
		//回転
		if (prefabJson["Rot"] == objectData["Rot"])
		{
			objectData.erase("Rot");
		}
		//拡縮
		if (prefabJson["Scale"] == objectData["Scale"])
		{
			objectData.erase("Scale");
		}
	}

	return objectData;
}

//更新
void GameObject::Update()
{
	m_mPrev = m_mWorld;
	m_prevPos = m_mWorld.GetTranslation();
	m_spSoundComponent->Update();
}

//描画
void GameObject::Draw()
{
	if (m_spModelComponent == nullptr) { return; }

	//透過オブジェクトの場合
	if (m_tag & TAG_TransparentObject)
	{
		//Z情報は使うが、Z書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
		//カリングなし
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);
	}

	//描画
	m_spModelComponent->Draw();

	//透過オブジェクトの場合もとに戻す
	if (m_tag & TAG_TransparentObject)
	{
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
	}

}
//シャドウマップ描画
void GameObject::DrawShadowMap()
{
	if (m_spModelComponent == nullptr)return;
	m_spModelComponent->DrawShadowMap();
}



void GameObject::ImGuiUpdate()
{
	ImGui::InputText("Name", &m_name);

	//Tag
	if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::CheckboxFlags("Character", &m_tag, TAG_Character);
		ImGui::CheckboxFlags("Player", &m_tag, TAG_Player);
		ImGui::CheckboxFlags("StageObject", &m_tag, TAG_StageObject);
		ImGui::CheckboxFlags("CanControlObject", &m_tag, TAG_CanControlObject);
		ImGui::CheckboxFlags("ProcessObject", &m_tag, TAG_ProcessObject);
		ImGui::CheckboxFlags("TransparentObject", &m_tag, TAG_TransparentObject);

		ImGui::TreePop();
	}


	//TransForm
	if (ImGui::TreeNodeEx("TransForm", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 rot = m_rot * ToDegrees;

		bool isChange = false;

		isChange |= ImGui::DragFloat3("Position", &m_pos.x, 1.0f);
		isChange |= ImGui::DragFloat3("Rotation", &rot.x, 1.0f);
		isChange |= ImGui::DragFloat3("Scale", &m_scale.x, 1.0f);
		//isChange |= ImGui::DragFloat("AllScale", &m_allScale, 1.0f);

		if (isChange)
		{
			//Radianに戻す
			m_rot = rot * ToRadians;

			m_mWorld.CreateScalling(m_scale);
			m_mWorld.Rotate(m_rot);
			m_mWorld.Move(m_pos);
		}

		ImGui::TreePop();
	}
}

void GameObject::SetAnimation(const char* pAnimName, bool isLoop)
{
	if (m_spModelComponent)
	{
		std::shared_ptr<AnimationData>animData = m_spModelComponent->GetAnimation(pAnimName);
		m_animator.SetAnimation(animData,isLoop);
	}
}

//球による当たり判定（距離判定）
bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	//当たったとする距離の計算（お互いの半径を足した値）
	float hitRange = rInfo.m_radius + m_radius;

	//自分の座標ベクトル
	Vector3 myPos = m_mWorld.GetTranslation();

	//二点間のベクトルを計算
	Vector3 betweenVec = rInfo.m_pos - myPos;

	//二点間の距離を計算
	float distance = betweenVec.Length();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
}

//レイによる当たり判定
bool GameObject::HitCheckByRay(const RayInfo& rInfo, RayResult& rResult)
{
	//判定をする対象のモデルがない場合は当たっていないとして帰る
	if (!m_spModelComponent) { return false; }

	//モデルコンポーネントはインスタンス化されているが、メッシュを読み込んでなかった場合も帰る

	//全てのノード（メッシュ）分当たり判定を行う
	for (auto& node : m_spModelComponent->GetNodes())
	{
		if (!node.m_spMesh) { continue; }

		RayResult tmpResult;//結果返送用

		//レイ判定（本体のずれ分も加味して計算）
		RayToMesh
		(
			rInfo.m_pos,
			rInfo.m_dir,
			rInfo.mMaxRange, 
			*(node.m_spMesh),
			m_spModelComponent->GetModel()->GetMaterials(),
			node.m_localTransform * m_mWorld,
			tmpResult
		);

		//より近い判定を優先する
		if (tmpResult.m_distance < rResult.m_distance)
		{
			rResult = tmpResult;
		}
	}

	return rResult.m_hit;
}

//球による当たり判定（mesh）
bool GameObject::HitCheckBySphereVsMesh(const SphereInfo& rInfo, SphereResult& rResult)
{
	//モデルコンポーネントがない場合
	if (!m_spModelComponent) { return false; }

	//全てのノードのメッシュから押し返された位置を格納する座標ベクトル
	Vector3 pushedFromNodesPos = rInfo.m_pos;

	//全ノードと当たり判定
	for (auto& node : m_spModelComponent->GetNodes())
	{
		//ノードがモデルを持っていなかった場合は無視
		if (!node.m_spMesh) { continue; }

		//点とノードの判定
		if (SphereToMesh(pushedFromNodesPos, rInfo.m_radius, *node.m_spMesh, node.m_localTransform * m_mWorld, pushedFromNodesPos))
		{
			rResult.m_hit = true;
		}
	}

	//当たっていたら
	if (rResult.m_hit)
	{
		//押し戻された球の位置と前の位置から、押し戻すベクトルを計算する
		rResult.m_push = pushedFromNodesPos - rInfo.m_pos;
	}

	return rResult.m_hit;
}

bool GameObject::HitCheckByBox(const BoxInfo& rInfo)
{
	//モデルコンポーネントがない場合
	if (!m_spModelComponent) { return false; }

	//全ノードと当たり判定
	for (auto& node : m_spModelComponent->GetNodes())
	{
		//ノードがモデルを持っていなかった場合は無視
		if (!node.m_spMesh) { continue; }
		if (
			BoxToBox(
				*rInfo.m_node.m_spMesh,
				rInfo.m_node.m_localTransform * rInfo.m_matrix,
				*node.m_spMesh,
				node.m_localTransform * m_mWorld
			)
		)
		{
			return true;
		}
	}

	return false;
}

bool GameObject::CheckGround(RayResult& finalRayResult,Vector3 pos,float& rDstDistance, UINT rTag, std::shared_ptr<GameObject> rNotObj)
{
	//レイ判定情報
	RayInfo rayInfo;
	//キャラクターの位置を発射地点に
	rayInfo.m_pos = pos;

	//キャラクターの足元からレイを発射すると地面と当たらないので少し持ち上げる（乗り越えられる段差の高さ分だけ）
	rayInfo.m_pos.y += s_allowToStepHeight;
	//落下中かもしれないので、１フレーム前の座標分も持ち上げる
	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;

	//地面方向へのレイ
	rayInfo.m_dir = { 0.0f,-1.0f,0.0f };

	//レイの結果格納用
	rayInfo.mMaxRange = FLT_MAX;

	//当たったオブジェクト保管用
	std::shared_ptr<GameObject> hitObj = nullptr;

	//全員とレイ判定
	for (auto& obj : SCENE.GetObjects())
	{
		//自分自身は判定しない
		if (obj.get() == this) { continue; }
		//指定されたタグのオブジェクトとだけ当たり判定
		if (!(obj->GetTag() & (rTag))) { continue; }
		//禁止指定されたオブジェクトは判定しない
		if (rNotObj && obj == rNotObj){ continue; }

		RayResult rayResult;

		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//最も当たったところまでの距離が短いものを保持する
			if (rayResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = rayResult;
				hitObj = obj;
			}
		}
	}

	//補正分の長さを結果に反映＆着地判定
	float distanceFromGround = FLT_MAX;

	//足元にステージオブジェクトがあった
	if (finalRayResult.m_hit)
	{
		//地面との距離を算出
		distanceFromGround = finalRayResult.m_distance - (m_prevPos.y - m_pos.y);
	}

	//上方向に力がかかっていた場合
	if (m_force.y > 0.0f)
	{
		//着地禁止
		m_isGround = false;
	}
	else
	{
		//地面からの距離（歩いて乗り越えられる高さ＋地面から足が離れていても着地判定する高さ）未満であれば着地とみなす
		m_isGround = (distanceFromGround < (s_allowToStepHeight + s_landingHeight));
	}

	//地面との距離を格納
	rDstDistance = distanceFromGround;

	//動くものの上に着地したときの判定
	if (hitObj && m_isGround)
	{
		//相手の一回動いた分を取得
		auto mOneMove = hitObj->GetOneMove();
		auto vOneMove = mOneMove.GetTranslation();

		//相手の動いた分を自分の移動に含める
		m_pos += vOneMove;

		///上方向への反射処理
		if (m_tag & TAG_CanControlObject)
		{
			if (fabs(m_force.y) >= 0.1f)
			{
				m_force = Vector3::Reflect(m_force, finalRayResult.m_polyDir) * m_force.Length();
			}
		}

		//摩擦による減速処理
		float force= (1.0f - finalRayResult.m_roughness) + m_elastic;
		if (force >= 1.0f) { force = 1.0f; }
		m_force *= force;
	}

	//着地したかどうかを返す
	return m_isGround;
}

bool GameObject::CheckBump(UINT rTag, std::shared_ptr<GameObject> rNotObj)
{
	//球情報の作成
	SphereInfo info;

	info.m_pos = m_pos;
	info.m_pos += m_centerOffset;
	info.m_radius = m_radius;

	//判定結果格納用
	bool isHit = false;

	//全員と球面判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//指定されたタグのオブジェクトとだけ当たり判定
		if (!(obj->GetTag() & (rTag))) { continue; }
		//指定されたオブジェクトは判定しない
		if (rNotObj)
		{
			if (obj == rNotObj) { continue; }
		}
		SphereResult sphereResult;
		//当たっていたら
		if (obj->HitCheckBySphereVsMesh(info, sphereResult))
		{
			isHit=true;
			//押し出された分を足しこむ
			m_pos += sphereResult.m_push;
			///反射処理======================================
			if (m_tag & TAG_CanControlObject)
			{
				m_force = Vector3::Reflect(m_force, sphereResult.m_push) * m_force.Length();
			}
		}

	}

	//反射音の処理
	if (isHit&&(m_tag & TAG_CanControlObject))
	{
		if (m_isImpactWall == false)
		{
			m_isImpactWall = true;
			//力によって音量を調整
			m_spSoundComponent->SetStateVolume(CorrectionValue(m_force.Length() / 1.0f, 1.0f, 0.0f));
			//再生
			m_spSoundComponent->SoundPlay("Data/Sound/Impact.wav");
		}
	}
	else
	{
		m_isImpactWall = false;
	}
	
	//デバック表示
	SCENE.AddDebugSphereLine(info.m_pos, info.m_radius, isHit ? Math::Color(1,0,0,1): Math::Color(0,1,0,1));

	return isHit;
}


//クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	//ゲームオブジェクト
	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
	}

	///アクションプロセス=====================================
	if (name == "ActionProcess")
	{
		return std::make_shared<ActionProcess>();
	}
	//プレイヤー
	if (name == "Player")
	{
		return std::make_shared<Player>();
	}
	//ボックス
	if (name == "Box")
	{
		return std::make_shared<Box>();
	}
	//ボール
	if (name == "Ball")
	{
		return std::make_shared<Ball>();
	}
	//バネ
	if (name == "Spring")
	{
		return std::make_shared<Spring>();
	}
	//ボックス
	if (name == "Button")
	{
		return std::make_shared<Button>();
	}
	//ドア
	if (name == "ManualDoor")
	{
		return std::make_shared<ManualDoor>();
	}
	if (name == "AutomaticDoor")
	{
		return std::make_shared<AutomaticDoor>();
	}
	//的
	if (name == "Target")
	{
		return std::make_shared<Target>();
	}
	//ライト
	if (name == "Light")
	{
		return std::make_shared<Light>();
	}
	//ゴール
	if (name == "Goal")
	{
		return std::make_shared<Goal>();
	}

	///タイトルプロセス=======================================
	if (name == "TitleProcess")
	{
		return std::make_shared<TitleProcess>();
	}

	///ステージセレクトプロセス===============================
	if (name == "StageSelectProcess")
	{
		return std::make_shared<StageSelectProcess>();
	}

	//UI======================================================
	if (name == "MenuList")
	{
		return std::make_shared<MenuList>();
	}
	//文字列が既存のクラスに一致しなかった
	assert(0 && "存在しないObjectクラスです");

	return nullptr;
}