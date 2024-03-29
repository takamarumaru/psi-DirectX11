﻿#pragma once

//3Dベクトル(整数)
class Point3
{
public:
	//座標指定付きコンストラクタ
	Point3(int _x, int _y, int _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	int x;
	int y;
	int z;
};

//3Dベクトル
class Vector3 :public DirectX::XMFLOAT3
{
public:

	//指定行列でVectorを変換する
	Vector3& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformCoord(*this, m);
		return *this;
	}

	//指定（回転）行列でVectorを変換する
	Vector3& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformNormal(*this, m);
		return *this;
	}

	//デフォルトコンストラクタは座標の０クリアを行う
	Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	//座標指定付きコンストラクタ
	Vector3(float _x,float _y,float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	//XMVECTORから代入してきたとき
	Vector3(const DirectX::XMVECTOR& v)
	{
		//変換して代入
		DirectX::XMStoreFloat3(this,v);
		//下記と同意だが、上記のSIMD命令を使用した方が高速
		/*
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
		*/
	}
	// XMFLOAT3から代入してきた時
	Vector3(const DirectX::XMFLOAT3& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
	}

	//Effekseer::Vector3Dとの互換性
	operator Effekseer::Vector3D() { return Effekseer::Vector3D(x, y, z); }

	
	//XMVECTORに変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

	//Math::Vector3と互換性を持つための関数
	operator Math::Vector3& () { return *(Math::Vector3*)this; }
	
	//算術演算子　加算(+=)
	Vector3& operator += (const Vector3& V)
	{
		this->x += V.x;
		this->y += V.y;
		this->z += V.z;
		return *this;
	}

	//算術演算子　減算(-=)
	Vector3& operator -= (const Vector3& V)
	{
		this->x -= V.x;
		this->y -= V.y;
		this->z -= V.z;
		return *this;
	}

	//算術演算子　乗算(*=)
	Vector3& operator*=(float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	//自分を正規化
	Vector3& Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
		return *this;
	}

	//ターゲットに向けてベクトルを徐々に変換
	inline void Complement(const Vector3& vTo, float rot)
	{
		//***回転軸作成(この軸で回転する)***
		Vector3 vRotAxis = Vector3::Cross(*this, vTo);
		//自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
		float d = Vector3::Dot(*this, vTo);

		//誤差で-1から1以外になる可能性大なので、クランプする。
		if (d > 1.0f)d = 1.0f;
		else if (d < -1.0f)d = -1.0f;

		//自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度(radian)
		float radian = acos(d);

		////角度制限　１フレームにつき最大で１度以上回転しない
		if (radian > rot * ToRadians)
		{
			radian = rot * ToRadians;
		}

		////＊＊＊radain(ここまでで回転角度が求まった)＊＊＊

		//ベクトル回転
		Math::Matrix mRot;
		mRot = DirectX::XMMatrixRotationAxis(vRotAxis, radian);
		*this = XMVector3TransformNormal(*this, mRot);
	}

	//長さ
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}
	
	//長さの２乗（高速なので判定用に使用することが多い）
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	//内積
	static float Dot(const Vector3& v1, const Vector3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}

	//外積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	//壁ずりベクトル
	static Vector3 WallScratch(const Vector3& rFront, const Vector3& rNormal)
	{
		Vector3 frontInv = VectorNomalize(-rFront);
		Vector3 normal = VectorNomalize(rNormal);
		Vector3 dot = DirectX::XMVector3Dot(frontInv,normal);

		return VectorNomalize(VectorNomalize(rFront) + dot * normal);
	}

	//反射ベクトル
	static Vector3 Reflect(const Vector3& rFront, const Vector3& rNormal)
	{
		Vector3 frontInv = VectorNomalize(-rFront);
		Vector3 normal = VectorNomalize(rNormal);
		Vector3 dot = DirectX::XMVector3Dot(frontInv, normal);

		return VectorNomalize(VectorNomalize(rFront) + 2.0f * dot * normal);
	}

	//正規化
	static Vector3 VectorNomalize(const Vector3& v)
	{
		return DirectX::XMVector3Normalize(v);
	}
};

//4*4の行列
class Matrix :public DirectX::XMFLOAT4X4
{
public:
	//デフォルトコンストラクタは単位行列化
	Matrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	//XMMatrixから代入してきた
	Matrix(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}

	//KdFLORT4X4,Math::Matrixから代入してきた
	Matrix(const DirectX::XMFLOAT4X4& m)
	{
		memcpy_s(this,sizeof(float)*16,&m,sizeof(XMFLOAT4X4));
	}

	//XMMATRIXへ変換
	operator DirectX::XMMATRIX()const
	{
		return DirectX::XMLoadFloat4x4(this);
	}

	//Math::Matrixと互換性を持つための関数
	operator Math::Matrix& (){return *(Math::Matrix*)this;}


	//Effekseer::Matrix44と互換性を持つための関数
	operator Effekseer::Matrix44()
	{
		Effekseer::Matrix44 result;
		for (int height = 0; height < 4; height++)
		{
			for (int width = 0; width < 4; width++)
			{
				result.Values[height][width] = this->m[height][width];
			}
		}
		return result;
	}

	//Effekseer::Matrix43と互換性を持つための関数
	operator Effekseer::Matrix43()
	{
		Effekseer::Matrix43 result;
		for (int height = 0; height < 4; height++)
		{
			for (int width = 0; width < 3; width++)
			{
				result.Value[height][width] = this->m[height][width];
			}
		}
		return result;
	}




	//代入演算子　乗算
	Matrix& operator *= (const Matrix& m)
	{
		*this = DirectX::XMMatrixMultiply(*this, m);
		return *this;
	}


//作成=================================================//

	//移動行列作成
	void CreateTranslation(float x,float y,float z)
	{
		*this = DirectX::XMMatrixTranslation(x,y,z);
	}
	void CreateTranslation(const Vector3& v)
	{
		*this = DirectX::XMMatrixTranslation(v.x, v.y, v.z);
	}

	//全ての回転行列作成
	void CreateRotation(const Vector3& v)
	{
		*this = DirectX::XMMatrixRotationX(v.x);
		*this *= DirectX::XMMatrixRotationY(v.y);
		*this *= DirectX::XMMatrixRotationZ(v.z);
	}
	void CreateRotation(float x, float y, float z)
	{
		*this = DirectX::XMMatrixRotationX(x);
		*this *= DirectX::XMMatrixRotationY(y);
		*this *= DirectX::XMMatrixRotationZ(z);
	}

	//X回転行列作成
	void CreateRotationX(float angle)
	{
		*this = DirectX::XMMatrixRotationX(angle);
	}

	//Y回転行列作成
	void CreateRotationY(float angle)
	{
		*this = DirectX::XMMatrixRotationY(angle);
	}

	//Z回転行列作成
	void CreateRotationZ(float angle)
	{
		*this = DirectX::XMMatrixRotationZ(angle);
	}

	//クォータニオンから回転行列作成
	void CreateFromQuaternion(const Math::Quaternion& quat)
	{
		*this = DirectX::XMMatrixRotationQuaternion(quat);
	}

	//拡縮行列作成
	void CreateScalling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x,y,z);
	}
	void CreateScalling(Vector3 v)
	{
		*this = DirectX::XMMatrixScaling(v.x, v.y, v.z);
	}


	//指定軸回転行列作成
	void CreateRotationAxis(const Vector3& axis, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(axis, angle);
	}

	//透視影行列の作成
	Matrix& CreateProjectionPerspecttiveFov(float fovAngle,float aspectRatio,float nearZ,float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngle,aspectRatio,nearZ,farZ);
		return *this;
	}

	// 正射影行列作成
	Matrix& CreateProjection_Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		return *this;
	}


//操作=================================================//

	//移動
	void Move(const Vector3& v)
	{
		*this *= DirectX::XMMatrixTranslation(v.x,v.y,v.z);
	}
	void Move(float x, float y, float z)
	{
		*this *= DirectX::XMMatrixTranslation(x,y,z);
	}

	//回転
	void Rotate(const Vector3& v)
	{
		*this *= DirectX::XMMatrixRotationX(v.x);
		*this *= DirectX::XMMatrixRotationY(v.y);
		*this *= DirectX::XMMatrixRotationZ(v.z);
	}

	//軸回転
	void RotateAxis(const Vector3& v,float angle)
	{
		*this *= DirectX::XMMatrixRotationAxis(v, angle);
	}

	//X軸回転
	void RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
	}

	//Y軸回転
	void RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
	}

	//Z軸回転
	void RotateZ(float angle)
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
	}

	//拡縮
	void Scale(float x, float y, float z)
	{
		*this *= DirectX::XMMatrixScaling(x, y, z);
	}

	// 逆行列にする
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}


	//ビルボード
	inline void SetBillboard(const Matrix& mat)
	{
		//対象の逆行列を取得
		Matrix camera = mat;
		camera.Inverse();
		//自分に合成
		*this *= camera;
	}


//プロパティ=================================================//
	
	//X軸取得
	Vector3 GetAxisX() const { return { _11,_12,_13 }; }

	//X軸セット
	void SetAxisX(const Vector3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	//Y軸取得
	Vector3 GetAxisY() const { return { _21,_22,_23 }; }

	//Y軸セット
	void SetAxisY(const Vector3& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}

	//Z軸取得
	Vector3 GetAxisZ() const { return { _31,_32,_33 }; }

	//Z軸セット
	void SetAxisZ(const Vector3& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	//座標取得
	Vector3 GetTranslation() const { return { _41,_42,_43 }; }
	
	//座標セット
	void SetTranslation(const Vector3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	//	XYZの順番で合成したときの、回転角度を算出する
	Vector3 GetAngles()const
	{
		Matrix mat = *this;

		//各軸を取得
		Vector3 axisX = mat.GetAxisX();
		Vector3 axisY = mat.GetAxisY();
		Vector3 axisZ = mat.GetAxisZ();

		//各軸を正規化
		axisX.Normalize();
		axisY.Normalize();
		axisZ.Normalize();

		//マトリクスを正規化
		mat.SetAxisX(axisX);
		mat.SetAxisY(axisY);
		mat.SetAxisZ(axisZ);

		Vector3 angles;
		angles.x = atan2f(mat.m[1][2], mat.m[2][2]);
		angles.y = atan2f(-mat.m[0][2], sqrtf(mat.m[1][2]* mat.m[1][2]+ mat.m[2][2]* mat.m[2][2]));
		angles.z = atan2f(mat.m[0][1], mat.m[0][0]);

		return angles;
	}


	// Z軸を指定方向に向ける
	Matrix& LookTo(const Vector3& dir, const Vector3& up)
	{
		Vector3 vZ = dir;
		vZ.Normalize();
		Vector3 vX = Vector3::Cross(up, vZ).Normalize();
		if (vX.LengthSquared() == 0)
		{
			vX = { 1,0,0 };
		}
		Vector3 vY = Vector3::Cross(vZ, vX).Normalize();

		float scaleX = GetAxisX().Length();
		float scaleY = GetAxisY().Length();
		float scaleZ = GetAxisZ().Length();

		SetAxisX(vX * scaleX);
		SetAxisY(vY * scaleY);
		SetAxisZ(vZ * scaleZ);

		return *this;
	}


	//拡縮取得
	Vector3 GetScale()const { return Vector3(_11,_22,_33); }

	//拡縮セット
	void SetScale(const Vector3& v)
	{
		_11 = v.x;
		_22 = v.y;
		_33 = v.z;
	}
	void SetScale(float x, float y, float z)
	{
		_11 = x;
		_22 = y;
		_33 = z;
	}

	//逆行列を取得
	Matrix Invert()
	{
		return DirectX::XMMatrixInverse(nullptr, *this);
	}

};

//KdMatrix同士の合成
inline Matrix operator* (const Matrix& M1, const Matrix& M2)
{
	using namespace DirectX;
	return XMMatrixMultiply(M1, M2);
}


//============================
//クォータニオン
//============================

class Quaternion :public DirectX::XMFLOAT4
{
public:
	//コンストラクタで初期値代入
	Quaternion()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	//XMVECTORから代入してきたとき
	Quaternion(const DirectX::XMVECTOR& V)
	{
		//変換して代入
		DirectX::XMStoreFloat4(this, V);
	}

	//XMVECTORに変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }


};

