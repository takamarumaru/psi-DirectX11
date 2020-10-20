#include "ActionProcess.h"
#include "../Scene.h"

#include "../../Component/InputComponent.h"

void ActionProcess::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//マウスカーソルを非表示
	ShowCursor(false);

	//インプットコンポーネント初期化
	m_spInputComponent = std::make_shared<ActionProcessInputComponent>(*this);

	//テクスチャの読み込み

}

//2D描画
void ActionProcess::Draw2D()
{

}

void ActionProcess::Update()
{
	if (m_spInputComponent == nullptr) { return; }

	m_spInputComponent->Update();
}
