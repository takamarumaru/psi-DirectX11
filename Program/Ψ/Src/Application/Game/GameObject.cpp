#include "GameObject.h"

#include"./Scene.h"

#include "../Component/ModelComponent.h"
#include "../Component/CameraComponent.h"

#include "ActionScene/Box.h"
#include "ActionScene/Player.h"

const float GameObject::s_allowToStepHeight = 0.8f;
const float GameObject::s_landingHeight = 0.1f;

//コンストラクタ
GameObject::GameObject(){}

//デストラクタ
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
		mRotate.CreateRotationX((float)rRot[0].number_value() * ToRadians);
		mRotate.RotateY((float)rRot[1].number_value() * ToRadians);
		mRotate.RotateZ((float)rRot[2].number_value() * ToRadians);
	}

	//拡縮
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	if (rScale.size() == 3)
	{
		mScale.CreateScalling
		(
			(float)rScale[0].number_value(),
			(float)rScale[1].number_value(),
			(float)rScale[2].number_value()
		);

		m_defScale = mScale.GetScale();
	}

	m_mWorld = mScale * mRotate * mTrans;


	//カメラの初期設定
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 0.0f, -3.0f);
	m_spCameraComponent->OffsetMatrix().RotateX(0.0f * ToRadians);
}

//データ保存
json11::Json::object GameObject::Serialize()
{
	json11::Json::object objectData;

	objectData["ClassName"] = m_className;			//クラス名
	objectData["Name"] = m_name;					//名前
	objectData["ModelFilePath"] = m_modelFilePath;	//モデルパス
	objectData["Tag"] = (int)m_tag;					//タグ

	//座標
	json11::Json::array mat(3);
	mat[0] = m_mWorld.GetTranslation().x;
	mat[1] = m_mWorld.GetTranslation().y;
	mat[2] = m_mWorld.GetTranslation().z;

	objectData["Pos"] = mat;

	//回転
	mat[0] = m_mWorld.GetAngles().x * ToDegrees;
	mat[1] = m_mWorld.GetAngles().y * ToDegrees;
	mat[2] = m_mWorld.GetAngles().z * ToDegrees;

	objectData["Rot"] = mat;

	//拡縮
	mat[0] = m_mWorld.GetScale().x;
	mat[1] = m_mWorld.GetScale().y;
	mat[2] = m_mWorld.GetScale().z;

	objectData["Scale"] = mat;
	
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
	m_prevPos = m_mWorld.GetTranslation();
}

//描画
void GameObject::Draw()
{
	if (m_spModelComponent == nullptr) { return; }

	m_spModelComponent->Draw();
}

void GameObject::DrawEffect(){}

void GameObject::ImGuiUpdate()
{
	ImGui::InputText("Name", &m_name);

	//Tag
	if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::CheckboxFlags("Character", &m_tag, TAG_Character);
		ImGui::CheckboxFlags("Player", &m_tag, TAG_Player);
		ImGui::CheckboxFlags("StageObject", &m_tag, TAG_StageObject);


		if (ImGui::Button(u8"JSONテキストにコピー"))
		{
			ImGui::SetClipboardText(Format("\"Tag\": %d", m_tag).c_str());
		}

		ImGui::TreePop();
	}


	//TransForm
	if (ImGui::TreeNodeEx("TransForm", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 pos = m_mWorld.GetTranslation();
		Vector3 rot = m_mWorld.GetAngles() * ToDegrees;
		Vector3 scale = m_mWorld.GetScale();

		bool isChange = false;

		isChange |= ImGui::DragFloat3("Position", &pos.x, 0.01f);
		isChange |= ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
		isChange |= ImGui::DragFloat3("Scale", &scale.x, 0.1f);
		isChange |= ImGui::DragFloat("AllScale", &m_allScale, 0.1f);

		if (isChange)
		{
			//=================================
			//回転
			//=================================

			//計算するときにはRadianに戻す
			rot *= ToRadians;

			Matrix mRX;
			mRX.RotateX(rot.x);
			Matrix mRY;
			mRY.RotateY(rot.y);
			Matrix mRZ;
			mRZ.RotateZ(rot.z);

			m_mWorld = mRX * mRY * mRZ;

			//=================================
			//全体の拡縮
			//=================================
			if (m_allScale > 0)
			{
				Matrix mS;

				mS.Scale
				(
					m_defScale.x * m_allScale,
					m_defScale.y * m_allScale,
					m_defScale.z * m_allScale
				);
				m_mWorld *= mS;
			}

			//=================================
			//移動
			//=================================

			m_mWorld.SetTranslation(pos);
			m_pos = pos;

		}
		if (ImGui::Button(u8"JSONテキストにコピー"))
		{
			std::string s = Format("\"Pos\": [%.1f,%.1f,%.1f],\n", pos.x, pos.y, pos.z);
			s += Format("\"Rot\": [%.1f,%.1f,%.1f],\n", rot.x, rot.y, rot.z);
			s += Format("\"Scale\": [%.1f,%.1f,%.1f],\n", scale.x, scale.y, scale.z);

			ImGui::SetClipboardText(s.c_str());
		}

		ImGui::TreePop();
	}
}

//解放
void GameObject::Release()
{

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
			rInfo.mMaxRange, *(node.m_spMesh),
			node.m_localTransform * m_mWorld, tmpResult
		);

		//より近い判定を優先する
		if (tmpResult.m_distance < rResult.m_distance)
		{
			rResult = tmpResult;
		}
	}

	return rResult.m_hit;
}

bool GameObject::CheckGround(RayResult& downRayResult,float& rDstDistance, UINT rTag)
{
	//レイ判定情報
	RayInfo rayInfo;
	//キャラクターの位置を発射地点に
	rayInfo.m_pos = m_pos;

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
		//自分自身は無理
		if (obj.get() == this) { continue; }
		//ステージとの当たり判定（背景オブジェクト以外に乗るときは変更）
		if (!(obj->GetTag() & (rTag))) { continue; }
		RayResult rayResult;

		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//最も当たったところまでの距離が短いものを保持する
			if (rayResult.m_distance < downRayResult.m_distance)
			{
				downRayResult = rayResult;
				hitObj = obj;
			}
		}
	}

	//補正分の長さを結果に反映＆着地判定
	float distanceFromGround = FLT_MAX;

	//足元にステージオブジェクトがあった
	if (downRayResult.m_hit)
	{
		//地面との距離を算出
		distanceFromGround = downRayResult.m_distance - (m_prevPos.y - m_pos.y);
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
	//if (hitObj && m_isGround)
	//{
	//	//相手の一回動いた分を取得
	//	auto mOneMove = hitObj->GetOneMove();
	//	auto vOneMove = mOneMove.GetTranslation();

	//	//相手の動いた分を自分の移動に含める
	//	m_pos += vOneMove;
	//}

	//着地したかどうかを返す
	return m_isGround;
}

bool GameObject::CheckXZDir (Vector3 rRayDir,float rCheckDistance, RayResult& frontRayResult, UINT rTag)
{
	//Yは加味しない
	rRayDir.y = 0.0f;
	//動いていないなら判定しない
	if (rRayDir.LengthSquared() == 0.0f) { return false; }


	//レイ判定情報
	RayInfo rayInfo;
	//キャラクターの位置を発射地点に
	rayInfo.m_pos = m_pos;

	//キャラクターの足元からレイを発射すると地面と当たらないので少し持ち上げる（乗り越えられる段差の高さ分だけ）
	rayInfo.m_pos.y += s_allowToStepHeight;
	//落下中かもしれないので、１フレーム前の座標分も持ち上げる
	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;

	//指定された方向へのレイ
	rayInfo.m_dir = rRayDir;

	//レイの結果格納用
	rayInfo.mMaxRange = FLT_MAX;

	//当たったオブジェクト保管用
	std::shared_ptr<GameObject> hitObj = nullptr;

	//全員とレイ判定
	for (auto& obj : SCENE.GetObjects())
	{
		//指定されたもの以外は判定しない
		if (obj.get() == this) { continue; }
		if (!(obj->GetTag() & (rTag))) { continue; }

		RayResult rayResult;
		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//最も当たったところまでの距離が短いものを保持する
			if (rayResult.m_distance < frontRayResult.m_distance)
			{
				frontRayResult = rayResult;
				hitObj = obj;
			}
		}
	}

	//補正分の長さを結果に反映＆着地判定
	float hitDistance = FLT_MAX;

	//発射方向にオブジェクトがあった
	if (frontRayResult.m_hit)
	{
		//オブジェクトとの距離を算出
		hitDistance = frontRayResult.m_distance - (m_prevPos.y - m_pos.y);
	}

	//判定距離より短かったら衝突判定に
	if (hitDistance < rCheckDistance)
	{
		return true;
	}
	return false;
}

//クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
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


	//文字列が既存のクラスに一致しなかった
	assert(0 && "存在しないObjectクラスです");

	return nullptr;
}