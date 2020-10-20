#include "Player.h"

#include"./Application/main.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/InputComponent.h"


///====================================================================
/// 移動更新
///====================================================================
void Player::UpdateMove()
{
	//入力情報の取得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	//カメラの右方向*レバーの左右入力
	Vector3 moveSide = m_spCameraComponent->OffsetMatrix().GetAxisX() * inputMove.x;
	//カメラの前方向*レバーの前後入力
	Vector3 moveForword = m_spCameraComponent->OffsetMatrix().GetAxisZ() * inputMove.y;
	//上下方向への移動成分はカット
	moveForword.y = 0.0f;

	//移動ベクトルの算出→正規化
	Vector3 moveVec = moveSide + moveForword;
	moveVec.Normalize();
	//回転処理→スピード合成
	UpdateRotate(moveVec);
	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;
}

///====================================================================
/// Y軸回転更新
///====================================================================
void Player::UpdateRotate(const Vector3& rMoveDir)
{
	//移動していなければ帰る
	if (rMoveDir.LengthSquared() == 0.0f) { return; }

	//今のキャラクターの方向ベクトル
	Vector3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	//キャラクターの今向いている方向の角度を求める（ラジアン角）
	float nowRadian = atan2(nowDir.x, nowDir.z);

	//移動方向へのベクトルの角度を求める（ラジアン角）
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

	//差分を求める
	float rotateRadian = targetRadian - nowRadian;

	//差分が-π～πの領域外なら遠回りしないように補正
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	//どちら側への回転かによって一回分の回転角度を代入
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * ToRadians, m_rotateAngle * ToRadians);

	m_rot.y += rotateRadian;

}

///====================================================================
/// 掴む操作の更新
///====================================================================
void Player::UpdateGrab()
{
	//オブジェクト登録用
	static std::shared_ptr<GameObject> operateObj = nullptr;
	//レイ判定情報
	RayInfo rayInfo;
	//オブジェクトまでの距離格納用
	static float operateObjDistance = 0.0f;

	//登録オブジェクトが存在しない間
	if (!m_isOperate)
	{
		//R1ボタンを押したとき
		if (m_spInputComponent->GetButton(Input::Buttons::R1) & InputComponent::ENTER)
		{
			//レイ発射座標からカメラの前方向に当たり判定
			//レイ発射座標
			Vector3 rayPos = m_spCameraComponent->GetCameraMatrix().GetTranslation();

			//レイ判定情報設定
			rayInfo.m_pos = rayPos;
			rayInfo.m_dir = m_spCameraComponent->OffsetMatrix().GetAxisZ();
			rayInfo.mMaxRange = FLT_MAX;


			//最終的な結果格納用
			RayResult finalRayResult;

			//全員とレイ判定
			for (auto& obj : SCENE.GetObjects())
			{
				//自分は判定しない
				if (obj.get() == this) { continue; }

				RayResult rayResult;
				if (obj->HitCheckByRay(rayInfo, rayResult))
				{
					//最も当たったところまでの距離が短いものを保持する
					if (rayResult.m_distance < finalRayResult.m_distance)
					{
						finalRayResult = rayResult;
						operateObj = obj;
					}
				}
			}

			//当たったオブジェクトがあったら
			if (operateObj)
			{
				//操作できるオブジェクト以外なら返る
				if (!(operateObj->GetTag() & (TAG_CanControlObject)))
				{
					operateObj = nullptr;
					return;
				}
				//操作フラグを立てる
				m_isOperate = true;
				//プレイヤーからオブジェクトの距離を算出
				Vector3 vec = operateObj->GetCenterPos() - rayPos;
				operateObjDistance = vec.Length();
				//オブジェクトの重力計算を停止
				operateObj->OffFall();
			}
		}
	}
	//登録オブジェクトが存在する間
	else
	{
		//オブジェクトのある方向に回転
		UpdateRotate(m_spCameraComponent->OffsetMatrix().GetAxisZ());

		///中心行列を生成====================================================================
		Matrix mCenter;
		mCenter.Move(GetCenterPos());

		///回転行列を生成====================================================================
		//カメラの前方向をクォータニオンに返還
		DirectX::XMVECTOR qTarget = DirectX::XMQuaternionRotationMatrix(m_spCameraComponent->OffsetMatrix());
		//最終的な角度を算出し回転行列に変換
		DirectX::XMVECTOR qFinal = DirectX::XMQuaternionSlerp(DirectX::XMQuaternionIdentity(), qTarget, 1.0f);
		Matrix mRot = DirectX::XMMatrixRotationQuaternion(qFinal);

		///外側行列を生成====================================================================
		//オブジェクトの位置を調整
		if (APP.m_window.GetMouseWheelVal())
		{
			operateObjDistance=(float)APP.m_window.GetMouseWheelVal();
		}
		//プレイヤーとの距離を制限
		if (operateObjDistance > 10.0f) { operateObjDistance = 10.0f; }
		if (operateObjDistance < 3.0f) { operateObjDistance = 3.0f; }
		//移動
		Matrix mOuter;
		mOuter.CreateTranslation(0.0f, 0.0f, operateObjDistance);

		///操作オブジェクトの移動量を算出====================================================
		//行列を合成
		mOuter *= mRot * mCenter;
		//オブジェクトから求めた行列までのベクトルを算出
		Vector3 vForce = mOuter.GetTranslation() - operateObj->GetCenterPos();
		//スピードを合成
		vForce *= 0.095f;
		//移動量を渡す
		operateObj->SetForce(vForce);

		//軌跡の座標を追加
		UpdatePowerEffect
		(
			mCenter.GetTranslation(),
			operateObj->GetCenterPos(),
			mOuter.GetTranslation()
		);

		///R1ボタンをもう一度押したときに登録を解除する======================================
		if (m_spInputComponent->GetButton(Input::Buttons::R1) & InputComponent::ENTER)
		{
			m_isOperate = false;		//操作フラグを解除
			m_powerEffect.ClearPoint();	//軌跡を消す
			operateObj->OnFall();		//重力を反映させる
			operateObj = nullptr;		//登録を外す
		}
	}
}