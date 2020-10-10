#include "Player.h"

#include"./Application/Component/InputComponent.h"

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

	//待機ステートに遷移
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
