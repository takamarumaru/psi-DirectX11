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

