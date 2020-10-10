#include "CameraComponent.h"

#include"Application/Game/Scene.h"

//コンストラクタ
CameraComponent::CameraComponent(GameObject& owner)
	:m_owner(owner)
{
	m_mProj.CreateProjectionPerspecttiveFov
	(
		60 * ToRadians,
		D3D.GetBackBuffer()->GetAspectRatio(),
		0.01f,
		5000.0f
	);
}

//デストラクタ
CameraComponent::~CameraComponent(){}

//更新処理
void CameraComponent::Update()
{
	UpdateCollision();
}

////カメラ行列・ビュー行列設定（行列mと行列Offsetが合成され、最終的なカメラ行列になる）
void CameraComponent::SetCameraMatrix(const Matrix& m)
{
	//カメラ行列セット
	m_mCam = m_mOffset * m;

	//カメラ行列からビュー行列を算出
	m_mView = m_mCam;
	m_mView.Inverse();
}

//カメラ情報（ビュー・射影行列など）をシェーダーにセット
void CameraComponent::SetToShader()
{
	//追従カメラ座標をシェーダーにセット
	SHADER.m_cb7_Camera.Work().CamPos = m_mCam.GetTranslation();

	//追従カメラのビュー行列をシェーダーにセット
	m_mCam.Inverse();
	SHADER.m_cb7_Camera.Work().mV = m_mCam;

	//追従カメラの射影行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	//カメラ情報（ビュー行列、射影行列）をシェーダーの定数バッファへセット
	SHADER.m_cb7_Camera.Write();
}

//当たり判定更新
void CameraComponent::UpdateCollision()
{
	Vector3 ownerPos = m_mCam.GetTranslation() - m_mOffset.GetTranslation();
	Vector3 rayDir = m_mCam.GetTranslation() - ownerPos;

	//プレイヤーとの距離を保存
	float rayLength = rayDir.Length();
	//正規化
	rayDir.Normalize();

	//レイ判定情報
	RayInfo rayInfo;
	//発射地点をオーナーの位置に
	rayInfo.m_pos = ownerPos;

	//地面方向へのレイ
	rayInfo.m_dir = rayDir;

	//レイの結果格納用
	rayInfo.mMaxRange = FLT_MAX;

	RayResult finalRayResult;

	//全員とレイ判定
	for (auto& obj : SCENE.GetObjects())
	{
		//ステージとの当たり判定（背景オブジェクト以外に乗るときは変更）
		if (!(obj->GetTag() & (TAG_StageObject|TAG_Character))) { continue; }
		RayResult rayResult;

		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//最も当たったところまでの距離が短いものを保持する
			if (rayResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = rayResult;
			}
		}
	}

	//判定距離より短かったらカメラの位置を移動
	if (finalRayResult.m_distance < rayLength)
	{
		//ターゲットの行列作成
		Matrix cameraTarget;
		cameraTarget.CreateTranslation(ownerPos);
		//新たなオフセットを算出
		Matrix newOffest;
		newOffest.CreateTranslation(0,0,-(finalRayResult.m_distance-0.1f));
		//回転は同じものを使う
		newOffest.Rotate(m_mOffset.GetAngles());

		//カメラ行列セット
		m_mCam = newOffest * cameraTarget;

		//カメラ行列からビュー行列を算出
		m_mView = m_mCam;
		m_mView.Inverse();
	}
}
