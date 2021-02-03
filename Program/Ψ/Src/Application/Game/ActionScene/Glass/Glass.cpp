#include "Glass.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Glass::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();

	m_rimColor = m_spModelComponent->GetRimColor();

}

void Glass::Update()
{
	GameObject::Update();

	m_count+=0.05f;

	m_spModelComponent->SetRimColor(m_rimColor * (1.0f+sinf(m_count) * 0.15f));


	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}

void Glass::DrawEffect()
{

	SHADER.m_modelShader.SetToDevice();
	
	GameObject::Draw();

	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

	//Z情報は使うが、Z書き込みOFF
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
	//カリングなし
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);
}
