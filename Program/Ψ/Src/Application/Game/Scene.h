#pragma once

class EditorCamera;
class CameraComponent;
class InputComponent;

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
	//デバックスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

/// シーン===========================================
	//変更のリクエストを受け付け
	void RequestChangeScene(const std::string& fileName);
	//番号取得
	UINT GetSceneNo() { return m_nowSceneNo; }
	//シーン遷移中か
	bool IsChangeScene() { return m_isRequestChangeScene; }
	//一時停止
	void Stop() { m_isUpdate = false; }
	//再開
	void Restart() { m_isUpdate = true; }
	//更新しているか
	bool IsUpdate() { return m_isUpdate; }
	//画面遷移しているか
	bool IsFade() { return m_isFade; }


/// オブジェクト管理=================================
	//追加
	void AddObject(std::shared_ptr <GameObject> spObject);
	//取得
	const std::list<std::shared_ptr<GameObject>> GetObjects()const { return m_spObjects; }
	//指定された名前で検索をかけて合致した最初のオブジェクトを返す
	std::shared_ptr<GameObject> FindObjectWithName(const std::string& name);

	void DebugLineClear() 
	{
		//デバックラインリストの初期化
		if (m_debugLines.size() >= 1)
		{
			m_debugLines.clear();
		}
	}

/// ImGui============================================
	//更新
	void ImGuiUpdate();
	//オブジェクト生成ウィンドウの更新
	void ImGuiPrefabFactoryUpdate();
	//ImGui表示
	bool IsImGui() { return m_isImGui; }

/// カメラアクセサ===================================
	inline void SetTargetCamera(std::shared_ptr<CameraComponent> spCamera) { m_wpTaegetCamera = spCamera; }


private:
	//デバック表示をするか
	bool m_isDebugLine = false;
	//インプットコンポーネント
	std::shared_ptr<InputComponent> m_spInputComponent = nullptr;

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

	//現在のシーン番号
	UINT m_nowSceneNo = 0;

	//フェード処理
	void FadeDraw();
	//フェード処理中かどうか
	bool m_isFade = false;
	//フェードのテクスチャ
	std::shared_ptr<Texture> m_spFadeTex;

	//シーンを更新するか
	bool m_isUpdate = true;


/// オブジェクト=====================================
	//リスト
	std::list<std::shared_ptr<GameObject>> m_spObjects;

/// ImGui============================================
	//ImGuiで選択されたオブジェクト
	std::weak_ptr<GameObject>		m_wpImGuiSelectObj;
	//ImGuiを表示するか
	bool m_isImGui = false;
	//ImGuiLogを表示するか
	bool m_isImGuiLog = false;


/// カメラ===========================================
	//エディターカメラ
	EditorCamera*	m_pCamera;
	// EditorCamera管理フラグ
	bool			m_editorCameraEnabe = false;
	//ターゲットのカメラ
	std::weak_ptr<CameraComponent>	m_wpTaegetCamera;

	//デバックライン描画用の頂点配列
	std::vector<EffectShader::Vertex> m_debugLines;

/// シェーダー=======================================

	// 平行光
	Vector3 m_lightDir = { 0,-1,0 };		//方向
	Vector3 m_lightColor = { 1,1,1 };		//光の色

	//描画する画面
	std::shared_ptr<Texture> m_spScreenRT = nullptr;
	std::shared_ptr<Texture> m_spScreenZ = nullptr;

	BlurTexture m_blurTex;
	std::shared_ptr<Texture> m_spHeightBrightTex = nullptr;

	std::shared_ptr<Texture> m_spToneTex = nullptr;

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