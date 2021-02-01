#include "Player.h"

#include"./Application/main.h"

#include"./Application/Game/Scene.h"

#include"./Application/Game/ActionScene/OperateObject.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/InputComponent.h"
#include"./Application/Component/SoundComponent.h"
#include"./Application/Component/ModelComponent.h"

#include"Application/Game/UI/PopUpTexture/PopUpTexture.h"


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

	m_moveForce.x += moveVec.x;
	m_moveForce.z += moveVec.z;

	if (m_moveForce.Length()>=m_moveSpeed)
	{
		m_moveForce.Normalize();
		m_moveForce *= m_moveSpeed;
	}

	//足音を再生
	if(m_isGround)
	{
		static int playerWalkCount = 0;
		playerWalkCount++;
		if (playerWalkCount % 20 == 0)
		{
			m_spSoundComponent->SoundPlay("Data/Sound/Walk.wav");
		}
	}
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
/// 掴む操作の更新と操作オブジェクトのリセット関数
///====================================================================
void Player::UpdateGrab()
{
	//操作オブジェクト
	static std::shared_ptr<GameObject> hitObj=nullptr;
	//レイ判定情報
	RayInfo rayInfo;
	//オブジェクトまでの距離格納用
	static float operateObjDistance = 0.0f;

	//シーン停止中なら操作を中止
	if (!SCENE.IsUpdate())
	{
		if (m_isOperate)
		{
			//操作オブジェクト初期化
			OperateReset();
		}
		return;
	}

	//登録オブジェクトが存在しない間
	if (!m_isOperate)
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
					hitObj = obj;
				}
			}
		}

		//当たったオブジェクトがあったら
		if (hitObj && (hitObj->GetTag() & (TAG_CanControlObject)))
		{
			//チュートリアル2を表示
			if (m_spTutorialTexList.find("Data/Texture/Tutorial2.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList["Data/Texture/Tutorial2.png"]->SetVisible(true);
			}
		}
		else
		{
			//チュートリアル2を非表示
			if (m_spTutorialTexList.find("Data/Texture/Tutorial2.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList.at("Data/Texture/Tutorial2.png")->SetVisible(false);
			}
			//チュートリアル3を非表示
			if (m_spTutorialTexList.find("Data/Texture/Tutorial3.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList.at("Data/Texture/Tutorial3.png")->SetVisible(false);
			}
			//チュートリアル4を非表示
			if (m_spTutorialTexList.find("Data/Texture/Tutorial4.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList["Data/Texture/Tutorial4.png"]->SetVisible(false);
			}
		}

		//R1ボタンを押したとき
		if (m_spInputComponent->GetButton(Input::Buttons::R1) & InputComponent::ENTER)
		{

			//当たったオブジェクトがあったら
			if (hitObj)
			{
				//操作できるオブジェクト以外なら返る
				if (!(hitObj->GetTag() & (TAG_CanControlObject)))
				{
					//当たったオブジェクトは削除しておく
					hitObj = nullptr;
					//キャンセル音再生
					m_spSoundComponent->SoundPlay("Data/Sound/Cancel.wav");
					return;
				}
				
				//チュートリアル2を非表示にして無効化
				if (m_spTutorialTexList.find("Data/Texture/Tutorial2.png") != m_spTutorialTexList.end())
				{
					m_spTutorialTexList["Data/Texture/Tutorial2.png"]->SetVisible(false);
					m_spTutorialTexList["Data/Texture/Tutorial2.png"]->SetEnable(false);
				}

				//チュートリアル3を表示
				if (m_spTutorialTexList.find("Data/Texture/Tutorial3.png") != m_spTutorialTexList.end())
				{
					m_spTutorialTexList["Data/Texture/Tutorial3.png"]->SetVisible(true);
				}

				//チュートリアル4を表示
				if (m_spTutorialTexList.find("Data/Texture/Tutorial4.png") != m_spTutorialTexList.end())
				{
					m_spTutorialTexList["Data/Texture/Tutorial4.png"]->SetVisible(true);
				}

				//当たったオブジェクトを操作オブジェクトにいれる
				m_spOperateObj = std::dynamic_pointer_cast<OperateObject>(hitObj);
				//操作フラグを立てる
				m_isOperate = true;
				//プレイヤーからオブジェクトの距離を算出
				Vector3 vec = m_spOperateObj->GetCenterPos() - rayPos;
				operateObjDistance = vec.Length();
				//オブジェクトの重力計算を停止
				m_spOperateObj->OffFall();
				//オーナーポインタを渡す
				m_spOperateObj->SetOwner(shared_from_this());
				//リムライティングを有効に
				m_spOperateObj->GetModelComponent()->SetRimColor(Vector3(1,0,1));
				//エフェクトを再生開始
				EFFEKSEER.Play(u"Data/EffekseerData/Aura5.efk", m_spOperateObj->GetCenterPos());
				//再生
				m_spSoundComponent->SoundPlay("Data/Sound/PowerStart.wav");
				m_spSoundComponent->SoundPlay("Data/Sound/PowerMid.wav",true);
			}
		}
	}
	//登録オブジェクトが存在する間
	else
	{
		//登録オブジェクトがなくなったら
		if (!(m_spOperateObj->IsAlive()))
		{
			//操作オブジェクト初期化
			OperateReset();
			return;
		}

		if (Vector3(m_spOperateObj->GetCenterPos() - m_pos).Length() < m_operateObjMinDst)
		{
			//操作オブジェクト初期化
			OperateReset();
			return;
		}

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
		operateObjDistance += APP.m_window.GetMouseWheelVal() * m_operateObjAroundSpeed;
		if (APP.m_window.GetMouseWheelVal() != 0.0f)
		{
			//チュートリアル3を非表示にして無効化
			if (m_spTutorialTexList.find("Data/Texture/Tutorial3.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList["Data/Texture/Tutorial3.png"]->SetVisible(false);
				m_spTutorialTexList["Data/Texture/Tutorial3.png"]->SetEnable(false);
			}
		}
		//プレイヤーとの距離を制限
		if (operateObjDistance > m_operateObjMaxDst) { operateObjDistance = m_operateObjMaxDst; }
		if (operateObjDistance < m_operateObjMinDst) { operateObjDistance = m_operateObjMinDst; }
		//移動
		Matrix mOuter;
		mOuter.CreateTranslation(0.0f, 0.0f, operateObjDistance);

		///操作オブジェクトの移動量を算出====================================================
		//行列を合成
		mOuter *= mRot * mCenter;
		//オブジェクトから求めた行列までのベクトルを算出
		Vector3 vForce = mOuter.GetTranslation() - m_spOperateObj->GetCenterPos();
		//スピードを合成
		vForce *= m_operateObjTrackingSpeed;
		//オブジェクトが壁をすり抜けないように制限
		if (vForce.Length() >= m_operateObjTrackingMaxSpeed)
		{
			vForce.Normalize();
			vForce *= m_operateObjTrackingMaxSpeed;
		}
		//移動量を渡す
		m_spOperateObj->SetForce(vForce);
		

		// オーラエフェクトの再生
		{
			static int playerEfcCount = 0;
			playerEfcCount++;
			if (playerEfcCount % 10 == 0)
			{
				//エフェクトを再生開始
				EFFEKSEER.Play(u"Data/EffekseerData/Aura4.efk", m_spOperateObj->GetCenterPos());

				playerEfcCount = 0;
			}
			//エフェクトの座標を更新
			EFFEKSEER.UpdatePos(u"Data/EffekseerData/Aura4.efk", m_spOperateObj->GetCenterPos());
			EFFEKSEER.UpdatePos(u"Data/EffekseerData/Aura5.efk", m_spOperateObj->GetCenterPos());
		}
		

		//軌跡の座標を追加
		UpdatePowerEffect
		(
			mCenter.GetTranslation(),
			m_spOperateObj->GetCenterPos(),
			mOuter.GetTranslation()
		);

		///R1ボタンをもう一度押したときに登録を解除する======================================
		if (m_spInputComponent->GetButton(Input::Buttons::R1) & InputComponent::ENTER)
		{
			//操作オブジェクト初期化
			OperateReset();
		}

		///Xボタンを押したときにオブジェクトを前方に発射する==================================
		if (m_spInputComponent->GetButton(Input::Buttons::X) & InputComponent::ENTER)
		{
			//発射エフェクトの再生
			{
				///カメラ中心行列を生成
				Matrix mCamCenter;
				mCamCenter.CreateTranslation(m_mWorld.GetTranslation());
				mCamCenter.Move(0.0f, 1.5f, 0.0f);
				//エッフェクト外側行列の作成
				Matrix mShotEffectOuter;
				mShotEffectOuter.CreateTranslation(0.0f, 0.0f, 1.0f);
				mShotEffectOuter *= mRot * mCamCenter;
				//再生
				EFFEKSEER.Play(u"Data/EffekseerData/Shot.efk", mShotEffectOuter);
			}

			vForce= m_spOperateObj->GetCenterPos()-mCenter.GetTranslation();
			vForce.Normalize();
			vForce *= m_operateObjShotPower;
			//移動量を渡す
			m_spOperateObj->SetForce(vForce);
			//操作オブジェクト初期化
			OperateReset();
			//発射音再生
			m_spSoundComponent->SoundPlay("Data/Sound/PowerShot.wav");
			//チュートリアル4を非表示にして無効化
			if (m_spTutorialTexList.find("Data/Texture/Tutorial4.png") != m_spTutorialTexList.end())
			{
				m_spTutorialTexList["Data/Texture/Tutorial4.png"]->SetVisible(false);
				m_spTutorialTexList["Data/Texture/Tutorial4.png"]->SetEnable(false);
			}
		}
	}
}

void Player::OperateReset()
{
	m_isOperate = false;												//操作フラグを解除
	m_powerEffect.ClearPoint();											//軌跡を消す
	m_spOperateObj->OnFall();											//重力を反映させる
	m_spOperateObj->GetModelComponent()->SetRimColor(Vector3(0, 0, 0)); //リムライティングを無効に
	m_spOperateObj->ClearOwner();										//オーナー情報を削除
	m_spOperateObj = nullptr;											//登録を外す
	EFFEKSEER.Stop(u"Data/EffekseerData/Aura5.efk");					//エフェクト再生停止
	m_spSoundComponent->SoundAllStop();
}