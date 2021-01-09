#pragma once

#include"../Game/GameObject.h"

class SoundComponent
{
public:

	SoundComponent(GameObject& owner) :m_owner(owner) {}

	//再生に必要なAudioEngineなどの初期化
	void Init();
	//AudioEngineの更新と再生が終わったインスタンスの破棄など
	void Update();
	//音データの読み込み
	void SoundLoad(const char* fileName,float volume);
	//サウンドの再生
	void SoundPlay(const char* soundName,bool isLoop=false);
	//全てのサウンドを停止
	void SoundAllStop();

	//距離による音量調整
	void UpdateCollision();

	//音量のリスト取得
	inline std::unordered_map<std::string, float> GetVolumeList() { return m_soundVolumeList; }

	void SetStateVolume(float value) { byStateVolume = value; }

private:

	//サウンドエンジン
	std::unique_ptr<DirectX::AudioEngine>			m_audioEng = nullptr;
	//再生するサウンド1つの元データ
	std::unordered_map<std::string, std::unique_ptr<DirectX::SoundEffect>>			m_soundEffects;
	//サウンドの再生インスタンス
	std::list<std::unique_ptr<DirectX::SoundEffectInstance>>	m_instances;

	//再生するサウンド1つの元データ
	std::unordered_map<std::string, float>	m_soundVolumeList;

	float basicVolume = 1.0f;
	float byDistanceVolume = 1.0f;
	float byStateVolume = 1.0f;

	GameObject& m_owner;

};
