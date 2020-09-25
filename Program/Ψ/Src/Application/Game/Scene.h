#pragma once

class EditorCamera;
class CameraComponent;

class GameObject;

class Scene
{
public:
	//デストラクタ
	~Scene();

/// 基本動作=========================================
	//データ読込・初期化
	void Deserialize();
	//データ保存
	void Serialize();
	//更新
	void Update();
	//描画
	void Draw();
	//解放
	void Release();

/// オブジェクト管理=================================
	//追加
	void AddObject(std::shared_ptr <GameObject> spObject);
	//取得
	const std::list<std::shared_ptr<GameObject>> GetObjects()const { return m_spObjects; }

/// ImGui============================================
	//更新
	void ImGuiUpdate();
	//オブジェクト生成ウィンドウの更新
	void ImGuiPrefabFactoryUpdate();

/// カメラアクセサ===================================
	inline void SetTargetCamera(std::shared_ptr<CameraComponent> spCamera) { m_wpTaegetCamera = spCamera; }


private:
/// シーン===========================================
	//読み込み
	void LoadScene(const std::string& sceneFilename);
	//書き込み
	void SaveScene(const std::string& sceneFilename);
	//リセット
	void Reset();

/// オブジェクト=====================================
	//リスト
	std::list<std::shared_ptr<GameObject>> m_spObjects;
	//ImGuiで選択されたオブジェクト
	std::weak_ptr<GameObject>		m_wpImGuiSelectObj;

/// カメラ===========================================
	//エディターカメラ
	EditorCamera*	m_pCamera;
	// EditorCamera管理フラグ
	bool			m_editorCameraEnabe = true;
	//ターゲットのカメラ
	std::weak_ptr<CameraComponent>	m_wpTaegetCamera;


//===================================================
//シングルトン実装
//===================================================
public:
	//実体取得
	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}
private:
	//コンストラクタ
	Scene();
};

#define SCENE Scene::GetInstance()