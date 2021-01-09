#include "System/System.h"

#include "Utility.h"

// viewから画像情報を取得する
void KdGetTextuxxreInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}


float CorrectionValue(float value, float max, float min)
{
	if (value >= max)return max;
	if (value <= min)return min;
	return value;
}

void MergePrefab(json11::Json& rSrcJson)
{
	// プレハブ指定ありの場合は、プレハブ側のものをベースにこのJSONをミックスする
	std::string prefabFilename = rSrcJson["Prefab"].string_value();
	if (prefabFilename.size() > 0)
	{
		// プレハブで指定したJSONの読み込み
		json11::Json prefJson = ResFac.GetJSON(prefabFilename);
		if (prefJson.is_null() == false)
		{
			json11::Json::object copyPrefab = prefJson.object_items();
			// マージする
			for (auto&& n : rSrcJson.object_items())
			{
				copyPrefab[n.first] = n.second;
			}
			// マージしたものに差し替え
			rSrcJson = copyPrefab;
		}
	}
}

RestoreRenderTarget::RestoreRenderTarget()
{
	//今のレンダーターゲットを覚える
	D3D.GetDevContext()->OMGetRenderTargets(1, &m_pSaveRT1, &m_pSaveZ);
}

RestoreRenderTarget::~RestoreRenderTarget()
{
	//レンダーターゲットを元に戻す
	D3D.GetDevContext()->OMSetRenderTargets(1, &m_pSaveRT1, m_pSaveZ);
	m_pSaveRT1->Release();
	m_pSaveZ->Release();
}
