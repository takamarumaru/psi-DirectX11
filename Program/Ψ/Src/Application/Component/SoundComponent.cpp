#include "SoundComponent.h"

//再生に必要なAudioEngineなどの初期化
void SoundComponent::Init()
{
	//サウンドエンジンの準備
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

	//サウンドエンジンの作成
	m_audioEng = std::make_unique<DirectX::AudioEngine>();
	m_audioEng->SetReverb(DirectX::Reverb_Default);


}

//AudioEngineの更新と再生が終わったインスタンスの破棄など
void SoundComponent::Update()
{
	//サウンドエンジンの更新
	if (m_audioEng == nullptr)
	{
		m_audioEng->Update();
	}


	//UIのSE以外なら距離によって音量を変更
	if (!(m_owner.GetTag() & TAG_ProcessObject))
	{
		UpdateCollision();
	}

	//再生中ではないインスタンスは終了したとしてリストから削除
	for (auto iter = m_instances.begin(); iter != m_instances.end();)
	{
		if (iter->get()->GetState() != DirectX::SoundState::PLAYING)
		{
			iter = m_instances.erase(iter);
			continue;
		}
		++iter;
	}
}

//音データの読み込み
void SoundComponent::SoundLoad(const char* fileName, float volume)
{
	//サウンドの読み込み
	if (m_audioEng != nullptr)
	{
		try
		{
			std::wstring wFileName = sjis_to_wide(fileName);
			m_soundEffects[fileName] = std::make_unique<DirectX::SoundEffect>(m_audioEng.get(), wFileName.c_str());
			m_soundVolumeList[fileName] = volume;
			basicVolume = volume;
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");
		}
	}
}

//サウンドの再生
void SoundComponent::SoundPlay(const char* soundName,bool isLoop)
{
	auto itFound = m_soundEffects.find(soundName);
	//BGMの再生
	if (itFound == m_soundEffects.end()) { return; }

	//再生オプション
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;
	//サウンドエフェクトからサウンドインスタンスの作成
	auto instance = (m_soundEffects[soundName]->CreateInstance(flags));
	//サウンドインスタンスを使ってい再生
	if (instance)
	{
		instance->SetVolume(m_soundVolumeList[soundName] * byDistanceVolume * byStateVolume);
		instance->Play(isLoop);

		IMGUI_LOG.AddLog("=======================");
		IMGUI_LOG.AddLog(m_owner.GetName());
		IMGUI_LOG.AddLog("Distance : %.2f", byDistanceVolume);
		IMGUI_LOG.AddLog("State    : %.2f", byStateVolume);
	}
	//再生中インスタンスリストに加える
	m_instances.push_back(std::move(instance));

}

//サウンドの停止
void SoundComponent::SoundAllStop()
{
	//再生中ではないインスタンスは終了したとしてリストから削除
	for (auto iter = m_instances.begin(); iter != m_instances.end();)
	{
		iter->get()->Stop();
		++iter;
	}
}

void SoundComponent::UpdateCollision()
{
	float toCameraDistance = (m_owner.GetCenterPos() - SHADER.m_cb7_Camera.GetWork().CamPos).Length();

	//距離が遠いほど小さくなる数値を算出
	byDistanceVolume = 1.0f-CorrectionValue(toCameraDistance / 25.0f, 1.0f, 0.0f);

	//if (m_owner.GetTag() & TAG_CanControlObject)
	//{

	//	IMGUI_LOG.AddLog(m_owner.GetName());
	//	IMGUI_LOG.AddLog("Distance : %.2f", byDistanceVolume);
	//	IMGUI_LOG.AddLog("State    : %.2f", byStateVolume);
	//}
}
