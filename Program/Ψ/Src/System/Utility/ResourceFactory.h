#pragma once
class Model;
class Texture;

//==========================
//リソース管理クラス
//・デザインパターン
//	Flyweightパターン
//==========================

class ResourceFactory
{
public:
	//モデルデータ取得
	std::shared_ptr<Model> GetModel(const std::string& filename);

	//テクスチャデータ取得
	std::shared_ptr<Texture> GetTexture(const std::string& filename);

	//json取得
	json11::Json GetJSON(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelMap.clear();
		m_texMap.clear();
		m_jsonMap.clear();
	}

private:

	//json読み込み
	json11::Json LoadJSON(const std::string& filename);

	//モデルデータ管理マップ
	std::unordered_map < std::string, std::shared_ptr<Model>> m_modelMap;

	//テクスチャー管理マップ
	std::unordered_map < std::string, std::shared_ptr<Texture>> m_texMap;

	//JSON管理マップ
	std::unordered_map < std::string,json11::Json> m_jsonMap;


//====================
//シングルトン
//====================
private:
	ResourceFactory() {}
public:
	static ResourceFactory& GetInstance()
	{
		static ResourceFactory instance;
		return instance;
	}
};

#define ResFac ResourceFactory::GetInstance()