#include "TitleProcess.h"
#include "../Scene.h"

#include "../../Component/InputComponent.h"

void TitleProcess::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_spInputComponent = std::make_shared<TitleProcessInputComponent>(*this);
}

void TitleProcess::Update()
{
	if (m_spInputComponent == nullptr) { return; }

	m_spInputComponent->Update();

	if (m_spInputComponent->GetButton(Input::Buttons::A) & InputComponent::EXIT)
	{
		//次のシーンに遷移
		Scene::GetInstance().RequestChangeScene("Data/JsonData/ActionScene.json");
	}
}
