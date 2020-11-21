#pragma once

#include"../Game/GameObject.h"

//================================================
//カメラコンポーネントクラス
//================================================
class CameraComponent
{
public:
	//コンストラクタ オーナーの設定と射影行列の作成
	CameraComponent(GameObject& owner);
	~CameraComponent();

	//更新
	void Update();

	//オフセット行列取得
	inline Matrix& OffsetMatrix() { return m_mOffset; }

	//カメラ行列取得
	inline const Matrix& GetCameraMatrix() { return m_mCam; }

	//ビュー行列取得
	inline const Matrix& GetViewMatrix() { return m_mView; }

	//
	inline const GameObject GetOwner() { return m_owner; }

	//カメラ行列・ビュー行列設定（行列mと行列Offsetが合成され、最終的なカメラ行列になる）
	void SetCameraMatrix(const Matrix& m);

	//カメラ情報（ビュー・射影行列など）をシェーダーにセット
	void SetToShader();
private:

	//当たり判定更新
	void UpdateCollision();


	//オフセット行列
	Matrix		m_mOffset;
	//カメラ行列
	Matrix		m_mCam;
	//ビュー行列
	Matrix		m_mView;

	//射影行列
	Matrix		m_mProj;

	GameObject&		m_owner;

};