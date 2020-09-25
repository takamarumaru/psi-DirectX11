#pragma once

class EditorCamera
{
public:
	//コンストラクタ
	EditorCamera();
	//デストラクタ
	~EditorCamera();

	void Update();		//更新
	void SetToShader();	//ビュー行列・射影行列をシェーダーへセット

	//注視点取得
	Vector3 GetViewPos() { return m_viewPos; }

private:

	Vector3	m_viewPos;		//注視点
	Matrix	m_mCam;			//注視点からの行列
	Matrix	m_mProj;		//射影行列

	//スライド操作の速度
	static const float s_slideSpeed;
	//回転操作の速度
	static const float s_rotateSpeed;
	//拡縮速度
	static const float s_scalingSpeed;

	POINT			m_prevMousePos = { 0,0 };//前フレームのマウス座標

};