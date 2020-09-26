#include "InputComponent.h"

//コンストラクタ：オーナーの設定・ボタンの初期化
InputComponent::InputComponent(GameObject& owner)
	:m_owner(owner)
{
	//操作軸初期化
	for (auto& axis : m_axes)
	{
		axis = { 0.0f,0.0f };
	}

	m_buttons.fill(FREE);
}

void InputComponent::PushButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);

	//押している
	if (m_buttons[no] & STAY)
	{
		m_buttons[no] &= ~ENTER;//ENTER反転とAND = ENTERPRESSだけをOFF
	}
	//押していない
	else
	{
		m_buttons[no] |= ENTER;	//ENTERをOR = ENTERをON
		m_buttons[no] |= STAY;	//STAYをOR = STAYをON
	}
}

void InputComponent::ReleaceButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);

	//押している
	if (m_buttons[no] & STAY)
	{
		m_buttons[no] &= ~ENTER;//ENTER反転とAND = ENTERPRESSだけをOFF
		m_buttons[no] &= ~STAY;//STAY反転とAND = STAYだけをOFF
		m_buttons[no] |= EXIT;//EXITをOR = EXITだけをON
	}
	//押していない
	else
	{
		m_buttons[no] &= ~EXIT;	//EXIT反転とAND = EXITだけをOFF
	}
}

//===============================================
//プレイヤー入力用
//===============================================
void PlayerInputComponent::Update()
{
	//操作軸初期化
	for (auto& rAxis : m_axes)
	{
		rAxis = { 0.0f,0.0f };
	}


	//[右の軸値]入力処理
	if (GetAsyncKeyState('W')) { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState('S')) { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState('D')) { m_axes[Input::Axes::L].x = 1.0f; }
	if (GetAsyncKeyState('A')) { m_axes[Input::Axes::L].x = -1.0f; }


	if (GetAsyncKeyState(VK_SPACE)) { PushButton(Input::Buttons::A); }
	else { ReleaceButton(Input::Buttons::A); }

	if (GetAsyncKeyState(VK_RCONTROL)) { PushButton(Input::Buttons::B); }
	else { ReleaceButton(Input::Buttons::B); }


	POINT nowMousePos;
	GetCursorPos(&nowMousePos);
	m_axes[Input::R].x = (float)(nowMousePos.x - m_prevMousePos.x);
	m_axes[Input::R].y = (float)(nowMousePos.y - m_prevMousePos.y);
	m_prevMousePos = nowMousePos;

	//マウスモード切替
	if (GetButton(Input::B) & ENTER)
	{
		m_isHideMouse = !m_isHideMouse;
		m_isHideMouse ? ShowCursor(false) : ShowCursor(true);
	}
	//カーソル固定
	if (m_isHideMouse)
	{
		SetCursorPos(100, 100);
		m_prevMousePos.x = 100;
		m_prevMousePos.y = 100;
	}
}
