#include "ResourceFactory.h"

std::shared_ptr<Model> ResourceFactory::GetModel(const std::string& filename)
{
	//filenameのモデルがあるとき
	auto itFound = m_modelMap.find(filename);
	//ない場合
	if (itFound==m_modelMap.end())
	{
		//生成を読み込み
		auto newModel = std::make_shared<Model>();
		if (newModel->Load(filename)==false)
		{
			//読み込み失敗時は、nullを返す
			return nullptr;
		}
		//リスト（map）に追加
		m_modelMap[filename] = newModel;
		//リソースに返す
		return newModel;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

std::shared_ptr<Texture> ResourceFactory::GetTexture(const std::string& filename)
{
	//frameのテクスチャーがあるか
	auto itFound = m_texMap.find(filename);

	//ない場合
	if (itFound == m_texMap.end())
	{
		//生成・読み込み
		auto newTexture = std::make_shared<Texture>();
		if (newTexture->Load(filename)==false)
		{
			//読み込み失敗時はnullを返す
			return nullptr;
		}
		//リストに登録
		m_texMap[filename] = newTexture;
		//リソースを返す
		return newTexture;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

json11::Json ResourceFactory::GetJSON(const std::string& filename)
{
	////filenameのものがあるとき
	//auto itFound = m_jsonMap.find(filename);
	////ない場合
	//if (itFound == m_jsonMap.end())
	//{
		//Jsonファイルを文字列として読み込む
		json11::Json json = LoadJSON(filename);
		if (json.is_null())
		{
			assert(0 && "[GetJSON]jsonファイルが見つからない");

			return nullptr;
		}
		//リスト（map）に追加
		m_jsonMap[filename] = json;
		//リソースに返す
		return json;
	//}
	////ある場合
	//else
	//{
	//	json11::Json json = (*itFound).second;
	//	return json;
	//}
}

//json読み込み
inline json11::Json ResourceFactory::LoadJSON(const std::string& filename)
{
	//jsonファイルを開く
	std::ifstream ifs(filename);
	if (ifs.fail())
	{
		assert(0 && "Jsonファイルのパスが間違っています");
		return nullptr;
	}

	//文字列として全読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	//文字列のJSONを解析（パース）する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0)
	{
		assert(0 && "読み込んだファイルのJson変換に失敗");
		return nullptr;
	}

	return jsonObj;

}
