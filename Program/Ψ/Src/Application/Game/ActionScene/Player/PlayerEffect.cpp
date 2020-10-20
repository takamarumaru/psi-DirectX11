#include"Player.h"

#include"./Application/Game/Scene.h"
//====================================================
//パワーエフェクトの更新
//====================================================
void Player::UpdatePowerEffect(const Vector3 rStart, const Vector3 rEnd, const Vector3 rTarget)
{
	//ポイントを初期化
	m_powerEffect.ClearPoint();

	//スタートから目的地点への距離を求める
	Vector3 vec = rTarget - rStart;

	//ベジェ曲線生成ループ
	for (int i=0;i<vec.Length();i++)
	{
		Vector3 vTo = rTarget - rStart;
		//始点から目的地点への座標の補間処理
		Vector3 vStartToTargetLerp = rStart + vTo * (i / vec.Length());
		//目的地点から終点への座標の補間処理
		vTo = rEnd - rTarget;
		Vector3 vTargetToEndLerp = rTarget + vTo * (i / vec.Length());
		//二つの補間座標から求めた曲線を描く補間座標
		vTo = vTargetToEndLerp - vStartToTargetLerp;
		Vector3 vFinalPoint = vStartToTargetLerp + vTo * (i / vec.Length());
		//ポイントを生成
		Matrix mPoint;
		mPoint.CreateTranslation(vFinalPoint);

		//ポイントを追加
		m_powerEffect.AddPoint(mPoint);
	}
}