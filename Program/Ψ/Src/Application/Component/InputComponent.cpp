#include "InputComponent.h"

#include"./Application/main.h"

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
InputComponent::InputComponent()
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
//デバッグ入力用
//===============================================
void DebugInputComponent::Update()
{
	//Aボタン入力処理
	if (GetAsyncKeyState(VK_RSHIFT)) { PushButton(Input::Buttons::A); }
	else { ReleaceButton(Input::Buttons::A); }
	//Bボタン入力処理
	if (GetAsyncKeyState('L')) { PushButton(Input::Buttons::B); }
	else { ReleaceButton(Input::Buttons::B); }
}

//===============================================
//タイトル入力用
//===============================================
void TitleProcessInputComponent::Update()
{
	//Aボタン入力処理
	if (GetAsyncKeyState(VK_LBUTTON)) { PushButton(Input::Buttons::A); }
	else { ReleaceButton(Input::Buttons::A); }
}
//===============================================
//アクション入力用
//===============================================
void ActionProcessInputComponent::Update()
{
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

	//Aボタン入力処理
	if (GetAsyncKeyState(VK_SPACE)) { PushButton(Input::Buttons::A); }
	else { ReleaceButton(Input::Buttons::A); }
	//Bボタン入力処理
	if (GetAsyncKeyState(VK_RCONTROL)) { PushButton(Input::Buttons::B); }
	else { ReleaceButton(Input::Buttons::B); }
	//Xボタン入力処理
	if (GetAsyncKeyState(VK_RBUTTON)) { PushButton(Input::Buttons::X); }
	else { ReleaceButton(Input::Buttons::X); }
	//R1ボタン入力処理
	if (GetAsyncKeyState(VK_LBUTTON)) { PushButton(Input::Buttons::R1); }
	else { ReleaceButton(Input::Buttons::R1); }


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
	//カーソルをウィンドウの中心に固定
	if (m_isHideMouse)
	{
		//ウィンドウのRECT取得
		RECT rWindow;
		GetWindowRect(APP.m_window.GetWndHandle(), &rWindow);

		SetCursorPos(
			rWindow.left+(rWindow.right- rWindow.left)/2,
			rWindow.top + (rWindow.bottom - rWindow.top) / 2
		);
		m_prevMousePos.x = rWindow.left + (rWindow.right - rWindow.left) / 2;
		m_prevMousePos.y = rWindow.top + (rWindow.bottom - rWindow.top) / 2;
	}
}
