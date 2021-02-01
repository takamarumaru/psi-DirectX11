#include "ObjectCreator.h"

#include "Application/Game/Scene.h"

void ObjectCreator::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_pos = m_mWorld.GetTranslation();

	//生成する子供のクラスネーム
	if (jsonObj["ChildClassName"].is_null() == false)
	{
		m_childClassName = jsonObj["ChildClassName"].string_value();
	}
	//生成する子供のjsonファイル
	if (jsonObj["ChildPrefab"].is_null() == false)
	{
		m_childPrefab = jsonObj["ChildPrefab"].string_value();
	}

	ChildCreate();
}

void ObjectCreator::Update()
{
	if (!m_wpChildObject.lock())
	{
		ChildCreate();
	}
}

void ObjectCreator::ChildCreate()
{
	//オブジェクト生成
	auto newGameObj = CreateGameObject(m_childClassName);

	if (newGameObj == nullptr) { return; }

	m_wpChildObject = newGameObj;

	json11::Json json = ResFac.GetJSON(m_childPrefab);
	newGameObj->Deserialize(json);

	//オブジェクトの位置を設定
	newGameObj->SetPos(m_pos);

	//リストへ追加
	SCENE.AddObject(newGameObj);
}
