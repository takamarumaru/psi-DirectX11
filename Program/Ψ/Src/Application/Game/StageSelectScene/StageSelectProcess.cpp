#include "StageSelectProcess.h"

#include "../../main.h"
#include "../Scene.h"

#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"

#include "../UI/Menu/MenuList.h"

void StageSelectProcess::Deserialize(const json11::Json& jsonObj)
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

	//タイトルロゴの読み込み
	if (jsonObj["SceneTex"].is_null() == false)
	{
		m_spStageSelectTex = ResFac.GetTexture(jsonObj["SceneTex"].string_value());
	}
}

void StageSelectProcess::Draw2D()
{
	Math::Color backColor(0.1f, 0.1f, 0.1f, 0.7f);
	SHADER.m_spriteShader.DrawBox(0,0,1280 / 2 , 720 / 2,&backColor,true);
	//タイトルの描画
	if (m_spStageSelectTex)
	{
		//2D描画
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_spStageSelectTex.get(), -1280 / 2 + 350, 720 / 2 - 100);
	}
}

void StageSelectProcess::Update()
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
