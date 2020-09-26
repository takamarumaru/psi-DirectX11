#include "Player.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/InputComponent.h"

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

	//行列をカメラにセット
	if (m_spCameraComponent)
	{
		Matrix cameraCenter=m_mWorld;
		Vector3 move = {0.0f,1.5f,0.0f};
		cameraCenter.Move(move);
		m_spCameraComponent->SetCameraMatrix(cameraCenter);
	}

	//座標
	m_mWorld.CreateTranslation(m_pos);
}

//移動更新
void Player::UpdateMove()
{
	//入力情報の取得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	Vector3 moveVec = { inputMove.x ,0.0f,inputMove.y };

	moveVec.Normalize();

	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;
}

void Player::UpdateCamera()
{
	if (!m_spCameraComponent) { return; }

	//入力情報の取得
	const Math::Vector2& inputRot = m_spInputComponent->GetAxis(Input::Axes::R);

	//カメラY軸回転
	m_spCameraComponent->OffsetMatrix().RotateY(inputRot.x * 0.2f * ToRadians);

	//X軸の移動制限
	float angle = m_spCameraComponent->OffsetMatrix().GetAngles().x * ToDegrees;
	if (fabs(m_spCameraComponent->OffsetMatrix().GetAngles().z * ToDegrees) > 90)
	{
		angle += (angle < 0) ? 180.0f : -180.0f;
	}

	if (fabs(angle + inputRot.y * 0.2f)> 90)
	{
		return;
	}

	IMGUI_LOG.Clear();
	IMGUI_LOG.AddLog("%f",angle);
	//カメラX軸回転
	m_spCameraComponent->OffsetMatrix().RotateAxis(m_spCameraComponent->OffsetMatrix().GetAxisX(),inputRot.y * 0.2f * ToRadians);

}

//当たり判定
void Player::UpdateCollision()
{
	float distanceFromGround = FLT_MAX;

	//下方向への判定を行い、着地した
	if (CheckGround(distanceFromGround, TAG_StageObject | TAG_Character))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - distanceFromGround;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;
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

//待機時の更新
void Player::WaitAction::Update(Player& rOwner)
{
	//移動ステートに遷移
	if (rOwner.IsShiftWalk())
	{
		rOwner.ShiftWalk();
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
	}
}
