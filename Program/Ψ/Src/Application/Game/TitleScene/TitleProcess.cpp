#include "TitleProcess.h"
#include "../Scene.h"

#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"

void TitleProcess::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//マウスカーソルを表示
	while (ShowCursor(true) < 0);

	//カメラの初期設定
	m_spCameraComponent->OffsetMatrix().CreateTranslation(0.5f, 2.5f, -5.0f);
	m_spCameraComponent->OffsetMatrix().RotateX(-2.0f * ToRadians);
	m_spCameraComponent->OffsetMatrix().RotateY(-80.0f * ToRadians);
	//カメラにセット
	SCENE.SetTargetCamera(m_spCameraComponent);

	m_spInputComponent = std::make_shared<TitleProcessInputComponent>(*this);
}

void TitleProcess::Update()
{
	if (m_spInputComponent == nullptr) { return; }

	m_spInputComponent->Update();

	//行列をカメラにセット
	if (m_spCameraComponent)
	{
		//カメラにセット
		m_spCameraComponent->SetCameraMatrix(m_mWorld);

		m_spCameraComponent->Update();
	}
}
