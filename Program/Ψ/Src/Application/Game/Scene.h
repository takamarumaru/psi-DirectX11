﻿#pragma once

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

	//デバックライン描画
	void AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	//シーン変更のリクエストを受け付け
	void RequestChangeScene(const std::string& fileName);


/// オブジェクト管理=================================
	//追加
	void AddObject(std::shared_ptr <GameObject> spObject);
	//取得
	const std::list<std::shared_ptr<GameObject>> GetObjects()const { return m_spObjects; }
	//指定された名前で検索をかけて合致した最初のオブジェクトを返す
	std::shared_ptr<GameObject> FindObjectWithName(const std::string& name);

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
	//シーンを実際に変更する
	void ExecChangeScene();
	//リセット
	void Reset();

	//次のシーンのJSONファイル名
	std::string m_nextSceneFileName = "";
	//シーン遷移のリクエストがあったか
	bool m_isRequestChangeScene = false;


/// オブジェクト=====================================
	//リスト
	std::list<std::shared_ptr<GameObject>> m_spObjects;

/// ImGui============================================
	//ImGuiで選択されたオブジェクト
	std::weak_ptr<GameObject>		m_wpImGuiSelectObj;
	//ImGuiを表示するか
	bool m_isImGui = false;

/// カメラ===========================================
	//エディターカメラ
	EditorCamera*	m_pCamera;
	// EditorCamera管理フラグ
	bool			m_editorCameraEnabe = false;
	//ターゲットのカメラ
	std::weak_ptr<CameraComponent>	m_wpTaegetCamera;


	//デバックライン描画用の頂点配列
	std::vector<EffectShader::Vertex> m_debugLines;


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