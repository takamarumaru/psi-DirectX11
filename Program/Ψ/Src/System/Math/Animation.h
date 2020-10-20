#pragma once

//アニメーションキー（クォータニオン）
struct AnimKeyQuaternion
{
	float			m_time = 0;		//時間
	Quaternion	m_quat;			//クォータニオンデータ
};

//アニメーションキー（ベクトル）
struct AnimKeyVector3
{
	float			m_time = 0;		//時間
	Vector3			m_vec;			//3Dベクトルデータ
};


//===========================
//アニメーションデータ
//===========================
struct AnimationData
{
	//アニメーション名
	std::string		m_name;
	//アニメの長さ
	float			m_maxLength = 0;

	//1ノードのアニメーションデータ
	struct Node
	{
		void Interpolate(Matrix& rDst, float time);
		bool InterpolateTranslations(Vector3& result, float time);
		bool InterpolateRotations(Quaternion& result, float time);
		//対象モデルノードのOffset値
		int		m_nodeOffset = -1;
		//各チャンネル
		std::vector<AnimKeyVector3>		m_translations;	//位置キーリスト
		std::vector<AnimKeyQuaternion>	m_rotaions;		//回転キーリスト
	};

	//全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};

class Animator
{
public:

	inline void SetAnimation(std::shared_ptr<AnimationData>& rData,bool isLoop)
	{
		m_isLoop = isLoop;
		m_spAnimation = rData;
		m_time = 0.0f;
	}

	bool IsAnimationEnd()const
	{
		if (m_spAnimation == nullptr) { return true; }
		if (m_time >= m_spAnimation->m_maxLength) { return true; }

		return false;
	}

	//アニメーションの更新
	void AdvanceTime(std::vector<Model::Node>& rNodes, float speed = 1.0f);

	inline float GetAnimTime() { return m_time; }

private:

	//再生するアニメーションデータ
	std::shared_ptr<AnimationData>	m_spAnimation = nullptr;

	float m_time = 0.0f;
	//アニメーションをループさせるかどうか
	bool m_isLoop = true;
};