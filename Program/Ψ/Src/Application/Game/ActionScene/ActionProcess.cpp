#include "ActionProcess.h"
#include "../Scene.h"

#include "../../Component/InputComponent.h"

void ActionProcess::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//マウスカーソルを非表示
	ShowCursor(false);

	m_spInputComponent = std::make_shared<ActionProcessInputComponent>(*this);
}

void ActionProcess::Update()
{
	if (m_spInputComponent == nullptr) { return; }

	m_spInputComponent->Update();
}
