#include "EditorCamera.h"
#include "../main.h"

const float EditorCamera::s_slideSpeed	 = 0.05f;
const float EditorCamera::s_rotateSpeed	 = 0.5f;
const float EditorCamera::s_scalingSpeed = 0.02f;

//コンストラクタ
EditorCamera::EditorCamera()
{
	//初期行列設定
	m_mCam.CreateTranslation(0.0f,0.0f,-5.0f);
	m_mCam.RotateX(DirectX::XMConvertToRadians(20));
	m_mCam.RotateY(DirectX::XMConvertToRadians(10));

	//射影行列
	m_mProj = DirectX::XMMatrixPerspectiveFovLH
	(
		DirectX::XMConvertToRadians(60),		//視野角
		D3D.GetBackBuffer()->GetAspectRatio(),	//アスペクト比
		0.01f,									//最近接距離
		5000.0f									//再遠方距離
	);
}

//デストラクタ
EditorCamera::~EditorCamera(){}

//更新
void EditorCamera::Update()
{
	//マウスの座標取得
	POINT nowPos;
	GetCursorPos(&nowPos);

	// shift+マウスの中ボタンでスライド操作==================================================
	if ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000))
	{
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * s_slideSpeed;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * s_slideSpeed;

		m_viewPos -= m_mCam.GetAxisX() * deltaX;
		m_viewPos += m_mCam.GetAxisY() * deltaY;
	}

	// マウスの中ボタンで回転操作============================================================
	else if (GetAsyncKeyState(VK_MBUTTON)& 0x8000)
	{
		//マウスの前回からの移動量
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * s_rotateSpeed;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * s_rotateSpeed;

		//Y軸回転
		Matrix mRotateY;
		mRotateY.CreateRotationY(deltaX * ToRadians);
		m_mCam *= mRotateY;

		//X軸回転
		Matrix mRotateX;
		mRotateX.RotateAxis(m_mCam.GetAxisX(),deltaY * ToRadians);
		m_mCam *= mRotateX;
	}

	// マウスのホイール回転で視野の拡大縮小==================================================
	if (APP.m_window.GetMouseWheelVal())
	{
		Matrix mDistance;
		mDistance.CreateTranslation(0.f,0.f,(float)APP.m_window.GetMouseWheelVal() * s_scalingSpeed);

		m_mCam = mDistance * m_mCam;
	}

	m_prevMousePos = nowPos;
}

//エディターカメラを使用する際のシェーダーセッティング
void EditorCamera::SetToShader()
{
	//カメラ行列の作成
	Math::Matrix mCam = m_mCam;

	//カメラ行列に中心点の行列を合成する
	mCam *= DirectX::XMMatrixTranslation(m_viewPos.x, m_viewPos.y, m_viewPos.z );

	//カメラの座標をシェーダーにセット
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();

	//カメラのビュー行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mV = mCam.Invert();

	//カメラの射影行列をセット
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	//カメラ情報（ビュー行列、射影行列）を、各シェーダーの定数バッファへセット
	SHADER.m_cb7_Camera.Write();
}
