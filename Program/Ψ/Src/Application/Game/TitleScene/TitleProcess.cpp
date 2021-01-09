#include "TitleProcess.h"

#include "../../main.h"
#include "../Scene.h"

#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"

#include "../UI/Menu/MenuList.h"

void TitleProcess::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//マウスカーソルを表示
	while (ShowCursor(true) < 0);

	//カメラの初期設定
	m_spCameraComponent->OffsetMatrix().CreateRotationX(10.0f * ToRadians);
	m_spCameraComponent->OffsetMatrix().RotateY(-130.0f * ToRadians);
	m_spCameraComponent->OffsetMatrix().Move(4.0f, 3.0f, 9.0f);
	//カメラにセット
	SCENE.SetTargetCamera(m_spCameraComponent);

	m_spInputComponent = std::make_shared<TitleProcessInputComponent>(*this);

	//タイトルロゴの読み込み
	if (jsonObj["TitleLogoTex"].is_null() == false)
	{
		m_spLogoTex = ResFac.GetTexture(jsonObj["TitleLogoTex"].string_value());
	}
}

void TitleProcess::Draw2D()
{
	//タイトルの描画
	if (m_spLogoTex)
	{
		//2D描画
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_spLogoTex.get(), 1280/2-300, -100);
	}
}

void TitleProcess::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	//行列をカメラにセット
	if (m_spCameraComponent)
	{
		//カメラにセット
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}
