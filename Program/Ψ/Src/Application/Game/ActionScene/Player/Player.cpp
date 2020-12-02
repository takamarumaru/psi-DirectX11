#include "Player.h"

#include"./Application/Game/Scene.h"

#include"./Application/Game/ActionScene/OperateObject.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/InputComponent.h"

#include"Application/Game/TextureEffect.h"

//初期化
void Player::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();

	//カメラにセット
	SCENE.SetTargetCamera(m_spCameraComponent);

	//待機状態からスタート
	m_spActionState = std::make_shared<WaitAction>();

	//入力用コンポーネントセット
	m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);

	//影を初期化
	m_shadow = std::make_shared<TextureEffect>();
	m_shadow->SetEffectInfo(ResFac.GetTexture("Data/Texture/shadow.png"), 2.0f,1,1,0,false,false);

	//念のテクスチャ
	m_powerEffect.SetTexture(ResFac.GetTexture("Data/Texture/powerEffect.png"));

	//ポイントのテクスチャ
	m_spPointTex = ResFac.GetTexture("Data/Texture/point.png");

	//エフェクトのロード
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Aura3.efk");
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Aura2.efk");
	EFFEKSEER.LoadEffect(u"Data/EffekseerData/Shot.efk");
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
	m_force.y -= m_gravity;

	//シーン変更中は待機
	if (SCENE.IsChangeScene())
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

	//行列をカメラにセット
	if (m_spCameraComponent)
	{
		//移動成分だけを抽出し、視点を少し上にあげる
		Matrix mCamera;
		mCamera.CreateTranslation(m_mWorld.GetTranslation());
		mCamera.Move(0.0f,1.5f,0.0f);
		//カメラにセット
		m_spCameraComponent->SetCameraMatrix(mCamera);

		m_spCameraComponent->Update();
	}

	//アクション処理
	UpdateGrab();

}

void Player::DrawEffect()
{
	//影の描画
	m_shadow->DrawEffect();

	//念の描画
	SHADER.m_effectShader.SetWorldMatrix(Matrix());
	SHADER.m_effectShader.WriteToCB();
	m_powerEffect.DrawBillboard(0.3f);
}

void Player::Draw2D()
{
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
}


void Player::UpdateCamera()
{
	//コンポーネントがない場合は返る
	if (!m_spCameraComponent) { return; }
	//シーン遷移中なら返る
	if (SCENE.IsChangeScene()) { return; }
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
	if (CheckGround(downRayResult, rayDistance, TAG_StageObject | TAG_Character, operateObj))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;

		//摩擦による減速処理
		m_moveForce *= (1.0f - downRayResult.m_roughness);
	}

	//影の更新
	Matrix mShadow;
	//ポリゴンの法線ベクトルと上方向のベクトルに垂直なベクトル
	Vector3 crossDir = Vector3::Cross(downRayResult.m_polyDir, { 0,1,0 });
	crossDir.Normalize();
	//ポリゴンの法線ベクトルから見た上方向のベクトルの角度
	float dotAngle = acosf(Vector3::Dot(downRayResult.m_polyDir, { 0,1,0 }));

	//crossDirがZeroベクトルなら回転しない
	if (!XMVector3Equal(crossDir, { 0.0f,0.0f,0.0f }))
	{
		//回転処理
		mShadow.CreateRotationAxis(crossDir, -dotAngle);
	}
	//移動処理
	mShadow.Move(downRayResult.m_hitPos += {0.0f, 0.05f, 0.0f});
	//行列をセット
	m_shadow->SetMatrix(mShadow);

	//横方向との当たり判定
	if(CheckBump(TAG_StageObject | TAG_Character, operateObj))
	{
		m_moveForce.x = 0.0f;
		m_moveForce.z = 0.0f;
	};
	
}