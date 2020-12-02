#include "main.h"

#include"Game/Scene.h"

//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	APP.Execute();

	// COM解放
	CoUninitialize();

	return 0;
}


// アプリケーション初期設定
bool Application::Init(int w, int h)
{

	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "Ψ", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	//bool bFullScreen = false;
	//if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
	//	bFullScreen = true;
	//}


	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	//// フルスクリーン設定
	//if (bFullScreen) {
	//	D3D.GetSwapChain()->SetFullscreenState(TRUE, 0);
	//}

	// シェーダー初期化
	SHADER.Init();

	//===================================================================
	// Imgui初期化
	//===================================================================
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(m_window.GetWndHandle());
	ImGui_ImplDX11_Init(D3D.GetDev(), D3D.GetDevContext());

	#include"imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	//日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	//===================================================================
	//Effekseer初期化
	//===================================================================
	EFFEKSEER.Initialize();


	return true;
}

// アプリケーション終了
void Application::Release()
{
	//シーンの解放
	SCENE.Release();

	//Imgui解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// シェーダ解放
	SHADER.Release();

	// Direct3D解放
	D3D.Release();


	// ウィンドウ削除
	m_window.Release();

	// Effekseer解放
	EFFEKSEER.Finalize();


}

// アプリケーション実行
void Application::Execute()
{
	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (APP.Init(1280, 720) == false) {
		return;
	}

	//シーンの初期化
	SCENE.Deserialize();

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	DWORD baseTime = timeGetTime();
	int count = 0;

	// ループ
	while (1)
	{

		// 処理開始時間Get
		DWORD st = timeGetTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{ 
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		//=========================================
		//
		// ゲーム処理
		//
		//=========================================

		//ImGui初期化
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		SCENE.ImGuiUpdate();


		//バックバッファクリア
		D3D.GetDevContext()->ClearRenderTargetView(D3D.GetBackBuffer()->GetRTView(), Math::Color(0.25f, 0.25f, 1.0f, 1.0f));

		//Zバッファクリア
		D3D.GetDevContext()->ClearDepthStencilView(D3D.GetZBuffer()->GetDSView(), D3D11_CLEAR_DEPTH || D3D11_CLEAR_STENCIL, 1.0f, 0);

		{
			//シーンの更新
			SCENE.Update();

			//シーンの描画
			SCENE.Draw();

			//ImGui描画実行
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			//Effekseer描画
			EFFEKSEER.Update();

		}

		//バックバッファを画面表示
		D3D.GetSwapChain()->Present(0, 0);


		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		constexpr int kSecond = 1000;				// １秒のミリ秒
		constexpr float kFpsRefreshFrame = 500;		// FPS計測のタイミング

		// 処理終了時間Get
		DWORD et = timeGetTime();
		// Fps制御
		DWORD ms = kSecond / m_maxFps;
		if (et - st < ms)
		{
			Sleep(ms - (et - st));	// 速すぎたら待つ
		}

		// FPS計測
		count++;
		if (st - baseTime >= kFpsRefreshFrame)
		{
			m_fps = (count * kSecond) / (st - baseTime);
			baseTime = st;
			count = 0;
		}
		//IMGUI_LOG.Clear();
		//IMGUI_LOG.AddLog("%d/%d",m_fps, m_maxFps);

	}


	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}