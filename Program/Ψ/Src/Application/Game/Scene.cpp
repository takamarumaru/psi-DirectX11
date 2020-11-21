﻿#include "Scene.h"

#include "../main.h"
#include"GameObject.h"


#include"EditorCamera.h"
#include"../Component/InputComponent.h"
#include"../Component/CameraComponent.h"

//コンストラクタ
Scene::Scene(){}

//デストラクタ
Scene::~Scene(){}

//データ読込・初期化
void Scene::Deserialize()
{
	//シーン読み込み
	LoadScene("Data/JsonData/TitleScene.json");

	//エディターカメラ
	m_pCamera = new EditorCamera();

	//入力用コンポーネントセット
	m_spInputComponent = std::make_shared<DebugInputComponent>();

	//フェードテクスチャ
	m_spFadeTex = ResFac.GetTexture("Data/Texture/fadeTex.png");

}

//データ保存
void Scene::Serialize()
{
	//jsonファイイルに書き込み
	SaveScene("Data/JsonData/ActionScene.json");
}

//更新
void Scene::Update()
{
	//エディターカメラ更新
	if (m_editorCameraEnabe)
	{
		if (m_pCamera)
		{
			m_pCamera->Update();
		}
	}

	//インプットコンポーネント更新
	m_spInputComponent->Update();

	//選択しているオブジェクトを取得
	auto selectObject = m_wpImGuiSelectObj.lock();

	//オブジェクト更新
	for (auto pObject : m_spObjects)
	{
		//ImGuiで選択されていたら実行しない
		if (pObject == selectObject) { continue; }

		if (pObject)
		{
			pObject->Update();
		}
	}

	//リストからの除外処理
	for (auto spObjectItr = m_spObjects.begin(); spObjectItr != m_spObjects.end();)
	{
		if ((*spObjectItr)->IsAlive() == false)
		{
			spObjectItr = m_spObjects.erase(spObjectItr);
		}
		else
		{
			++spObjectItr;
		}
	}


	if (m_isRequestChangeScene)
	{
		ExecChangeScene();
	}
}

//描画
void Scene::Draw()
{
	//エディターカメラ用にシェーダーをセット
	if (m_editorCameraEnabe)
	{
		if (m_pCamera)
		{
			m_pCamera->SetToShader();
		}
	}
	//エディターカメラ機能offの時
	else
	{
		std::shared_ptr<CameraComponent> spCamera = m_wpTaegetCamera.lock();

		if (spCamera)
		{
			spCamera->SetToShader();
		}
	}

	//ライトの情報をセット
	SHADER.m_cb8_Light.Write();

	//エフェクトシェーダを描画デバイスにセット
	SHADER.m_effectShader.SetToDevice();

	//不透明物描画
	{
		SHADER.m_standardShader.SetToDevice();

		//オブジェクト描画
		for (auto pObject : m_spObjects)
		{
			pObject->Draw();
		}
	}

	//半透明物描画
	{
		SHADER.m_effectShader.SetToDevice();
		SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

		//Z情報は使うが、Z書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
		//カリングなし
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);

		for (auto spObj : m_spObjects)
		{
			spObj->DrawEffect();
		}

		//もとに戻す
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWhiteEnable, 0);
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
	}

	//2D描画用のシェーダー開始
	{
		SHADER.m_spriteShader.Begin();
		//全てのオブジェクトの2D描画を行う
		for (auto obj : m_spObjects)
		{
			obj->Draw2D();
		}
		//フェード描画
		FadeDraw();
		//シェーダー終了
		SHADER.m_spriteShader.End();
	}

	//デバックライン描画
	if(m_isDebugLine)
	{
		SHADER.m_effectShader.SetToDevice();
		SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
		//Zバッファ使用OFF・書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWhiteDisable, 0);
		if (m_debugLines.size() >= 1)
		{
			SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());
			SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

			m_debugLines.clear();
		}
		//Zバッファ使用ON・書き込みON
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWhiteEnable, 0);
	}
}

//解放
void Scene::Release()
{
	//配列の入れ物を削除
	m_spObjects.clear();
}


//デバックライン描画
void Scene::AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color)
{

	//ラインの開始頂点w
	EffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };
	ver.Pos = p1;
	m_debugLines.push_back(ver);

	//ラインの終端頂点
	ver.Pos = p2;
	m_debugLines.push_back(ver);

}


//デバックスフィア描画
void Scene::AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color)
{
	//EffectShader::Vertex ver;
	//ver.Color = color;
	//ver.UV = { 0.0f,0.0f };

	//static constexpr int kDetail = 32;
	//for (UINT i = 0; i < kDetail + 1; ++i)
	//{
	//	//XZ平面
	//	ver.Pos = pos;
	//	ver.Pos.x += cos((float)i * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.z += sin((float)i * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);

	//	ver.Pos = pos;
	//	ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);

	//	//XY平面
	//	ver.Pos = pos;
	//	ver.Pos.x += cos((float)i * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.y += sin((float)i * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);

	//	ver.Pos = pos;
	//	ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.y += sin((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);

	//	//YZ平面
	//	ver.Pos = pos;
	//	ver.Pos.y += cos((float)i * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.z += sin((float)i * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);

	//	ver.Pos = pos;
	//	ver.Pos.y += cos((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * ToRadians) * radius;
	//	m_debugLines.push_back(ver);
	//}
}

void Scene::RequestChangeScene(const std::string& fileName)
{
	//すでにシーンリクエストがある場合は返る
	if (m_isRequestChangeScene) { return; }

	m_nextSceneFileName = fileName;

	m_isFade = true;
	m_isRequestChangeScene = true;
}

//オブジェクトの追加
void Scene::AddObject(std::shared_ptr<GameObject> spObject)
{
	if (spObject == nullptr) { return; }

	m_spObjects.push_back(spObject);
}

//指定された名前で検索をかけて合致した最初のオブジェクトを返す
std::shared_ptr<GameObject> Scene::FindObjectWithName(const std::string& name)
{
	//文字列比較
	for (auto&& obj : m_spObjects)
	{
		if (obj->GetName() == name) { return obj; }
	}
	//見つからなかったらnullを返す
	return nullptr;
}

//シーンの読み込み
void Scene::LoadScene(const std::string& sceneFilename)
{
	//各項目のクリア
	Reset();

	//デバッグ表示
	std::string message = sceneFilename + "を読み込み\n";
	OutputDebugStringA(message.c_str());

	//json読み込み
	json11::Json json = ResFac.GetJSON(sceneFilename);
	if (json.is_null())
	{
		IMGUI_LOG.AddLog(u8"[LoadScene]jsonファイル読み込み失敗");
		return;
	}

	//シーン番号取得
	m_nowSceneNo = json["SceneNo"].number_value();

	//オブジェクトリスト取得
	auto& objectDataList = json["GameObjects"].array_items();

	//オブジェクト生成ループ
	for (auto&& objJsonData : objectDataList)
	{
		//プレハブ指定ありの場合は、プレハブ側のものをベースにこのJsonをマージする
		MergePrefab(objJsonData);

		//オブジェクト生成
		auto newGameObj = CreateGameObject(objJsonData["ClassName"].string_value());

		if (newGameObj == nullptr) { break; }

		//オブジェクトのデシリアライズ
		newGameObj->Deserialize(objJsonData);

		//リストへ追加
		AddObject(newGameObj);
	}
}

//シーンの書き出し
void Scene::SaveScene(const std::string& sceneFilename)
{
	//シーンJson生成
	json11::Json::object jsonScene;
	//Jsonのオブジェクト配列生成
	json11::Json::array jsonObject;

	//オブジェクト分のデータを取得
	for (auto&& rObj : m_spObjects)
	{
		jsonObject.push_back(rObj->Serialize());
	}

	//オブジェクト配列を追加
	jsonScene["GameObjects"] = jsonObject;

	//データ出力
	{
		//ファイル出力用テキスト
		std::string outputStr;
		//dump(jsonデータからテキストを生成)用のjson生成
		json11::Json jsonDump = jsonScene;
		//テキスト生成
		jsonDump.dump(outputStr, true);

		//ファイル出力
		std::ofstream file;
		file.open(sceneFilename);

		if(!file.is_open())
		{
			IMGUI_LOG.AddLog(u8"ファイルが開けません。パスを入力してください。");
			return;
		}

		file << outputStr << std::endl;
		file.close();
	}

}

//シーンを実際に変更する
void Scene::ExecChangeScene()
{
	//フェードアウト中なら帰る
	if (m_isFade) { return; }
	//シーンを読み込む
	LoadScene(m_nextSceneFileName);

	m_isRequestChangeScene = false;
}

//シーンをまたぐ際のリセット
void Scene::Reset()
{
	m_spObjects.clear();		//オブジェクトリスト
	m_wpImGuiSelectObj.reset();	//ImGuiが選んでいるオブジェクトをリセット
}


//フェード処理
void Scene::FadeDraw()
{
	//フェード処理
	static float alpha = 0;
	if (m_isFade)
	{
		alpha+=0.01f;
		if (alpha >= 1.0f)
		{
			m_isFade = false;
		}
	}
	else
	{
		alpha -= 0.01f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
		}
	}
	//2D描画
	SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
	SHADER.m_spriteShader.DrawTex(m_spFadeTex.get(), 0, 0,nullptr,&Math::Color(0,0,0, alpha));
}

//ImGui更新
void Scene::ImGuiUpdate()
{
	//ログウィンドウ更新
	if (m_spInputComponent->GetButton(Input::Buttons::B) & m_spInputComponent->ENTER) { m_isImGuiLog = !m_isImGuiLog; }
	if (m_isImGuiLog)
	{
		IMGUI_LOG.ImGuiUpdate("Log Window");
	}
	
	//ImGuiOffの場合返る
	if (m_spInputComponent->GetButton(Input::Buttons::A) & m_spInputComponent->ENTER) { m_isImGui = !m_isImGui; }
	if (!m_isImGui) { return; }

	auto selectObject = m_wpImGuiSelectObj.lock();

	if (ImGui::Begin("Scene"))
	{
		//エディターカメラ機能
		ImGui::Checkbox("EditorCamera:", &m_editorCameraEnabe);
		//デバックライン表示
		ImGui::Checkbox("DebugLine:", &m_isDebugLine);

		//オブジェクトリストの描画
		if (ImGui::CollapsingHeader("Object List", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto&& rObj : m_spObjects)
			{
				//選択オブジェクトと一致するオブジェクトかどうか
				bool selected = (rObj == selectObject);

				ImGui::PushID(rObj.get());

				if (ImGui::Selectable(rObj->GetName(), selected))
				{
					m_wpImGuiSelectObj = rObj;
				}

				ImGui::PopID();

			}
		}
	}

	ImGui::End();

	//インスペクタウィンド
	if (ImGui::Begin("Inspector"))
	{

		if (selectObject)
		{
			//オブジェクトリストで選択したゲームオブジェクトの情報を描画
			selectObject->ImGuiUpdate();
		}
	}
	ImGui::End();

	//オブジェクト生成
	ImGuiPrefabFactoryUpdate();
}

//ImGuiからオブジェクト生成
void Scene::ImGuiPrefabFactoryUpdate()
{
	if (ImGui::Begin("PrefabFactory"))
	{
		//オブジェクト生成
		if (ImGui::CollapsingHeader("CreateObject", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//パス入力
			static std::string objectPath;
			ImGui::InputText("オブジェクトパス", &objectPath);

			//パス取得ボタン
			ImGui::SameLine();
			if (ImGui::Button(u8"objectパス取得"))
			{
				//エクスプローラを開く
				Window::OpenFileDialog(objectPath, "jsonファイルを開く", "Objectのjsonファイル\0*Object*\0*.json\0\0");
			}

			//名前入力
			static std::string objectName;
			ImGui::InputText("ObjectName", &objectName);

			//生成ボタン
			if (ImGui::Button("Create"))
			{
				//json読み込み
				json11::Json json = ResFac.GetJSON(objectPath);
				if (json.is_null())
				{
					IMGUI_LOG.AddLog(u8"オブジェクト生成に失敗(ファイルのパスが間違っています)");
					ImGui::End();
					return;
				}
				//生成
				auto newGameObj = CreateGameObject(json["ClassName"].string_value());
				if (!newGameObj)
				{
					IMGUI_LOG.AddLog(u8"項目\"ClassName\"に異常がある可能性");
					ImGui::End();
					return;
				}

				//名前の変更
				if (objectName.empty())
				{
					IMGUI_LOG.AddLog(u8"オブジェクト名が入力されていません");
					ImGui::End();
					return;
				}
				json["Name"] = objectName;
				//オブジェクトのデシリアライズ
				newGameObj->Deserialize(json);

				//リストへ追加
				AddObject(newGameObj);
				IMGUI_LOG.AddLog(u8"オブジェクトの生成に成功");

			}
		}

		//選択しているオブジェクトを削除
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("DeleteSelectObject",ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Delete"))
			{
				if (MessageBoxA(APP.m_window.GetWndHandle(), Format("現在選択中のオブジェクトを削除します。\n本当によろしいですか？").c_str(), "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
					//選択しているオブジェクトを取得
					auto selectObject = m_wpImGuiSelectObj.lock();
					if (selectObject)
					{
						//削除
						selectObject.get()->Destroy();
						m_wpImGuiSelectObj.reset();
					}
				}
			}
		}

		//シーンの読み書き
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("SceneIO", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static std::string scenePath;
			//シーンパス入力
			ImGui::InputText("シーンパス", &scenePath);

			//パス取得ボタン
			ImGui::SameLine();
			if (ImGui::Button(u8"Sceneパス取得"))
			{
				//エクスプローラを開く
				Window::OpenFileDialog(scenePath, "jsonファイルを開く", "Sceneのjsonファイル\0*Scene*\0*.json\0\0");
			}

			//シーン読み込み
			if (ImGui::Button("Load"))
			{
				if (MessageBoxA(APP.m_window.GetWndHandle(), Format(scenePath + "を読み込みます。\n現在編集中のデータは消えてしまいますが本当によろしいですか？").c_str(), "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
					IMGUI_LOG.AddLog(u8"シーンを読み込みました。");
					LoadScene(scenePath);
				}
			}

			//データ保存
			if (ImGui::Button("Save"))
			{
				if (MessageBoxA(APP.m_window.GetWndHandle(), Format("現在のオブジェクトリストを" + scenePath + "に上書きします。\n本当によろしいですか？").c_str(), "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
					SaveScene(scenePath);
				}
			}
		}

	}

	ImGui::End();

}