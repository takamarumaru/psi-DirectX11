#include "Player.h"

#include"./Application/Game/Scene.h"

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

	//影の初期化
	m_shadow = std::make_shared<TextureEffect>();
	m_shadow->SetEffectInfo(ResFac.GetTexture("Data/Texture/shadow.png"), 2.0f,1,1,0,false,false);
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

	//移動更新
	m_spActionState->Update(*this);

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

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
}

void Player::DrawEffect()
{
	//影の描画
	m_shadow->DrawEffect();
}

//移動更新
void Player::UpdateMove()
{
	//入力情報の取得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	//カメラの右方向*レバーの左右入力
	Vector3 moveSide = m_spCameraComponent->OffsetMatrix().GetAxisX() * inputMove.x;
	//カメラの前方向*レバーの前後入力
	Vector3 moveForword = m_spCameraComponent->OffsetMatrix().GetAxisZ() * inputMove.y;
	//上下方向への移動成分はカット
	moveForword.y = 0.0f;

	//移動ベクトルの算出→正規化
	Vector3 moveVec = moveSide + moveForword;
	moveVec.Normalize();
	//回転処理→スピード合成
	UpdateRotate(moveVec);
	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;
}

//操作やキャラクターの行動による回転計算
void Player::UpdateRotate(const Vector3& rMoveDir)
{
	//移動していなければ帰る
	if (rMoveDir.LengthSquared() == 0.0f) { return; }

	//今のキャラクターの方向ベクトル
	Vector3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	//キャラクターの今向いている方向の角度を求める（ラジアン角）
	float nowRadian = atan2(nowDir.x, nowDir.z);

	//移動方向へのベクトルの角度を求める（ラジアン角）
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

	//差分を求める
	float rotateRadian = targetRadian - nowRadian;

	//差分が-π～πの領域外なら遠回りしないように補正
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	//どちら側への回転かによって一回分の回転角度を代入
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * ToRadians, m_rotateAngle * ToRadians);
	
	m_rot.y += rotateRadian;

}

void Player::UpdateCamera()
{
	if (!m_spCameraComponent) { return; }

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
	if (fabs(angle + inputRot.y * m_cameraRotSpeed)> 90)
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

	//下方向への判定を行い、着地した
	if (CheckGround(downRayResult, rayDistance, TAG_StageObject | TAG_Character))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;
	}

	//影の更新
	Matrix mShadow;
	//ポリゴンの法線ベクトルと上方向のベクトルに垂直なベクトル
	Vector3 crossDir = Vector3::Cross(downRayResult.m_polyDir, { 0,1,0 });
	crossDir.Normalize();
	//ポリゴンの法線ベクトルから見た上方向のベクトルの角度
	float dotAngle = acosf(Vector3::Dot(downRayResult.m_polyDir, { 0,1,0 }));

	//crossDirがZeroベクトルなら回転しない
	if (!XMVector3Equal(crossDir, {0.0f,0.0f,0.0f}))
	{
		//回転処理
		mShadow.CreateRotationAxis(crossDir, -dotAngle);
	}
	//移動処理
	mShadow.Move(downRayResult.m_hitPos += {0.0f,0.05f,0.0f});
	//行列をセット
	m_shadow->SetMatrix(mShadow);


	//進行方向への当たり判定
	RayResult axisZRayResult;
	//進行方向への判定を行い、衝突
	if (CheckXZDir(m_mWorld.GetAxisZ(), m_radius, axisZRayResult, TAG_StageObject | TAG_Character))
	{
		m_pos.x = m_prevPos.x;
		m_pos.z = m_prevPos.z;
	}

	//Z方向への当たり判定
	RayResult frontRayResult;
	//Z方向への判定を行い、衝突
	if (CheckXZDir({0,0,1}, m_radius, frontRayResult, TAG_StageObject | TAG_Character))
	{
		m_pos.z = frontRayResult.m_hitPos.z - m_radius;
	}

	//-Z方向への当たり判定
	RayResult backRayResult;
	//-Z方向への判定を行い、衝突
	if (CheckXZDir({ 0,0,-1 }, m_radius, backRayResult, TAG_StageObject | TAG_Character))
	{
		m_pos.z = backRayResult.m_hitPos.z + m_radius;
	}

	//X方向への当たり判定
	RayResult rightRayResult;
	//X方向への判定を行い、衝突
	if (CheckXZDir({1,0,0}, m_radius, rightRayResult, TAG_StageObject | TAG_Character))
	{
		m_pos.x = rightRayResult.m_hitPos.x - m_radius;
	}

	//-X方向への当たり判定
	RayResult leftRayResult;
	//-X方向への判定を行い、衝突
	if (CheckXZDir({-1,0,0}, m_radius, leftRayResult, TAG_StageObject | TAG_Character))
	{
		m_pos.x = leftRayResult.m_hitPos.x + m_radius;
	}
}

//移動ステートへの移行条件を満たしているか
bool Player::IsShiftWalk()
{
	if (!m_spInputComponent) { return false; }

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	//移動してたら
	if (inputMove.LengthSquared() != 0.0f)
	{
		return true;
	}

	return false;
}

//ジャンプステートへの移行条件を満たしているか
bool Player::IsShiftJump()
{
	if (!m_spInputComponent) { return false; }

	if (m_isGround)
	{
		//ジャンプ(SPACE)
		if (m_spInputComponent->GetButton(Input::Buttons::A))
		{
			return true;
		}
	}

	return false;
}

//待機時の更新
void Player::WaitAction::Update(Player& rOwner)
{
	//移動ステートに遷移
	if (rOwner.IsShiftWalk())
	{
		rOwner.ShiftWalk();
		return;
	}
	//ジャンプ
	if (rOwner.IsShiftJump())
	{
		//ジャンプアクションへ遷移
		rOwner.ShiftJump();
		return;
	}
}

//移動時の更新
void Player::WalkAction::Update(Player& rOwner)
{
	//移動処理
	rOwner.UpdateMove();

	//移動ステートに遷移
	if (!rOwner.IsShiftWalk())
	{
		rOwner.ShiftWait();
		return;
	}
	//ジャンプ
	if (rOwner.IsShiftJump())
	{
		//ジャンプアクションへ遷移
		rOwner.ShiftJump();
		return;
	}
}

void Player::JumpAction::Update(Player& rOwner)
{
	//移動処理
	rOwner.UpdateMove();

	if (rOwner.IsGround())
	{
		//待機アクションへ遷移
		rOwner.ShiftWait();
		return;
	}
}
