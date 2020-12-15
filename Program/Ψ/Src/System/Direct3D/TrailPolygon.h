#pragma once

class TrailPolygon 
{
public:
	//先頭のポイントを取得
	inline Math::Matrix* GetTopPoint()
	{
		if (m_pointList.size() == 0) { return nullptr; }
		return &m_pointList.front();
	}

	//ポイントを追加
	inline void AddPoint(const Math::Matrix& mat)
	{
		m_pointList.push_front(mat);
	}
	//ポイントを最後尾に追加
	inline void AddPointBack(const Math::Matrix& mat)
	{
		m_pointList.push_back(mat);
	}

	//最後尾のポイントを削除
	inline void DelPoint_Back()
	{
		m_pointList.pop_back();
	}

	//ポイントをすべて削除
	inline void ClearPoint()
	{
		m_pointList.clear();
	}

	//リストの数を取得
	inline int getNumPoints()const
	{
		return (int)m_pointList.size();
	}

	//リストを取得
	inline std::deque<Math::Matrix> GetPoints()const
	{
		return m_pointList;
	}

	//画像をセット
	inline void  SetTexture(const std::shared_ptr<Texture>& tex)
	{
		m_texture = tex;
	}

	//通常描画　行列のX軸方向に頂点が作られ描画される
	//width　…　ポリゴンの幅
	void Draw(float width);

	void DrawDetached(float width);

	//ビルボード描画
	void DrawBillboard(float width);

	//頂点情報をそのまま繋げてポリゴンを描画
	void TrailPolygon::DrawStrip();
private:

	//煙　軌跡
	float m_trailRotate = 0.0f;

	//1頂点の形式
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
		Math::Vector4 Color = { 1,1,1,1 };
	};

	//軌跡の位置などを記憶する行列配列
	std::deque<Math::Matrix>	m_pointList;

	//テクスチャ
	std::shared_ptr<Texture>	m_texture;
};