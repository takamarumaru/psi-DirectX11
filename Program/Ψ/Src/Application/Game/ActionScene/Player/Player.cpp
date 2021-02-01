#include "Player.h"

#include"./Application/Game/Scene.h"

#include"./Application/Game/ActionScene/OperateObject.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/InputComponent.h"

#include"Application/Game/TextureEffect.h"
#include"Application/Game/UI/PopUpTexture/PopUpTexture.h"

//初期化
void Player::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//カメラの設定
	m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 0.0f, -0.01f);
	m_spCameraComponent->OffsetMatrix().RotateY(m_rot.y);
	m_spCameraComponent->OffsetMatrix().RotateX(m_rot.x);
	//カメラにセット
	SCENE.SetTargetCamera(m_spCameraComponent);

	//待機状態からスタート
	m_spActionState = std::make_shared<WaitAction>();

	//入力用コンポーネントセット
	m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);
	m_spInputComponent->Init();

	//念のテクスチャ
	m_powerEffect.SetTexture(ResFac.GetTexture("Data/Texture/powerEffect.png"));

	//ポイントのテクスチャ
	m_spPointTex = ResFac.GetTexture("Data/Texture/point.png");

	//エフェクトのロード
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Aura4.efk");
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Aura5.efk");
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Shot.efk");

	//チュートリアルの画像をロード
	auto& rTutorialTex = jsonObj["TutorialTexList"].array_items();
	for (auto&& texture : rTutorialTex)
	{
		//画像と座標を保存
		std::shared_ptr<PopUpTexture> tex = std::make_shared<PopUpTexture>();
		tex->SetTexture(ResFac.GetTexture(texture[0].string_value()));
		tex->SetPos(texture[1].number_value()*0.01f, texture[2].number_value() * 0.01f, 0.0f);
		m_spTutorialTexList[texture[0].string_value().c_str()] = tex;
	}

	//チュートリアル１を表示
	if (m_spTutorialTexList.find("Data/Texture/Tutorial1.png") != m_spTutorialTexList.end())
	{
		m_spTutorialTexList["Data/Texture/Tutorial1.png"]->SetVisible(true);
	}

}

json11::Json::object Player::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	json11::Json::array tutorialTexList(m_spTutorialTexList.size());

	UINT i=0;
	for (auto&& texture : m_spTutorialTexList)
	{
		json11::Json::array tutorialTex(3);
		tutorialTex[0] = texture.first.c_str();
		tutorialTex[1] = (int)(texture.second->GetCenterPos().x * 100.0f);
		tutorialTex[2] = (int)(texture.second->GetCenterPos().y * 100.0f);

		tutorialTexList[i] = tutorialTex;
		i++;
	}

	objectData["TutorialTexList"] = tutorialTexList;

	return objectData;
}

//更新
void Player::Update()
{
	GameObject::Update();

	//InputComponentの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	//カメラの更新
	UpdateCamera();

	//重力をキャラクターのYの移動力に変える
	if (SCENE.IsUpdate()) { m_force.y -= m_gravity; }

	//シーン変更中は待機
	if (!SCENE.IsUpdate())
	{
		ShiftWait();
	}

	//移動更新
	m_spActionState->Update(*this);

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;
	m_pos += m_moveForce;

	//当たり判定
	UpdateCollision();

	//行列合成
	m_mWorld.CreateRotation(m_rot);	//回転
	m_mWorld.Move(m_pos);			//座標

	//シーン更新不可なら行列を元に戻す
	if (!SCENE.IsUpdate())
	{
		m_mWorld = m_mPrev;
		m_pos = m_prevPos;
	}

	//行列をカメラにセット
	if (m_spCameraComponent)
	{
		//移動成分だけを抽出し、視点を少し上にあげる
		Matrix mCamera;
		mCamera.CreateTranslation(m_mWorld.GetTranslation());
		mCamera.Move(0.0f, 1.5f, 0.0f);
		//カメラにセット
		m_spCameraComponent->SetCameraMatrix(mCamera);
	}

	//アクション処理
	UpdateGrab();

	//チュートリアルテクスチャの更新
	for (auto&& texture : m_spTutorialTexList)
	{
		if (texture.second)
		texture.second->Update();
	}
}

void Player::DrawEffect()
{
	//念の描画
	SHADER.m_effectShader.SetWorldMatrix(Matrix());
	SHADER.m_effectShader.WriteToCB();
	m_powerEffect.DrawBillboard(0.3f);
}

void Player::Draw2D()
{
	if (!SCENE.IsUpdate()) { return; }
	//ポイントの描画
	if (m_spPointTex) 
	{
		//操作中ではないとき
		if (!m_isOperate)
		{
			//2D描画
			SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
			SHADER.m_spriteShader.DrawTex(m_spPointTex.get(), 0, 0);
		}
	}

	//チュートリアルテクスチャの描画
	for (auto&& texture : m_spTutorialTexList)
	{
 		if(texture.second)
		texture.second->Draw2D();
	}
}


void Player::UpdateCamera()
{
	//コンポーネントがない場合は返る
	if (!m_spCameraComponent) { return; }
	//シーン停止中なら返る
	if (!SCENE.IsUpdate()) { return; }
	//デバック中は返る
	if (SCENE.IsImGui()) { return; }

	//入力情報の取得
	const Math::Vector2& inputRot = m_spInputComponent->GetAxis(Input::Axes::R);

	//カメラY軸回転
	m_spCameraComponent->OffsetMatrix().RotateY(inputRot.x * m_cameraRotSpeed * ToRadians);

	//X軸の移動制限
	float angle = m_spCameraComponent->OffsetMatrix().GetAngles().x * ToDegrees;
	if (fabs(m_spCameraComponent->OffsetMatrix().GetAngles().z * ToDegrees) > 90)
	{
		angle += (angle < 0) ? 180.0f : -180.0f;
	}
	if (fabs(angle + inputRot.y * m_cameraRotSpeed)> 80)
	{
		return;
	}
	//カメラX軸回転
	m_spCameraComponent->OffsetMatrix().RotateAxis(m_spCameraComponent->OffsetMatrix().GetAxisX(),inputRot.y * m_cameraRotSpeed * ToRadians);

}


//当たり判定
void Player::UpdateCollision()
{
	float rayDistance= FLT_MAX;

	RayResult downRayResult;

	std::shared_ptr<GameObject> operateObj = std::dynamic_pointer_cast<GameObject>(m_spOperateObj);

	//下方向への判定を行い、着地した
	if (CheckGround(downRayResult, m_pos, rayDistance, TAG_StageObject | TAG_Character, operateObj))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;

		//摩擦による減速処理
		m_moveForce *= (1.0f - downRayResult.m_roughness);
	}

	//横方向との当たり判定
	if(CheckBump(TAG_StageObject | TAG_Character,TAG_None, operateObj))
	{
		m_moveForce.x = 0.0f;
		m_moveForce.z = 0.0f;
	};
	
}