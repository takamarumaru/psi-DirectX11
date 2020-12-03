#pragma once

enum DRAW_MODE
{
	NORMAL,
	GRAYSCALE,
	SEPIA
};

struct BlurTexture
{
	void Create(int w, int h)
	{
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			m_rt[i][0] = std::make_shared<Texture>();
			m_rt[i][0]->CreateRenderTarget(w / divideValue, h / divideValue,
				DXGI_FORMAT_R16G16B16A16_FLOAT);

			m_rt[i][1] = std::make_shared<Texture>();
			m_rt[i][1]->CreateRenderTarget(w / divideValue, h / divideValue,
				DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
	}

	std::shared_ptr<Texture>	m_rt[5][2];
};


class KdPostProcessShader
{
public:
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
	};

	bool Init();

	void Release();
	~KdPostProcessShader()
	{
		Release();
	}

	void SetDrawMode(DRAW_MODE mode) { m_cb0_Color.Work().DrawMode = mode; }

	void ColorDraw(const Texture* tex, const Math::Vector4& color = { 1,1,1,1 });

	void BlurDraw(const Texture* tex, const Math::Vector2& dir);

	void KdPostProcessShader::GenerateBlur(BlurTexture& blurTex, const Texture* srcTex);

	void BrightFiltering(const Texture* destRT, const Texture* srcTex);

private:
	ID3D11VertexShader* m_VS = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;

	//色変更
	ID3D11PixelShader* m_colorPS = nullptr;
	struct cbColor {
		Math::Vector4	Color = { 1,1,1,1 };
		UINT DrawMode = DRAW_MODE::NORMAL;
		float tmp[3];
	};
	ConstantBuffer<cbColor>	m_cb0_Color;

	//ぼかし
	ID3D11PixelShader* m_blurPS = nullptr;
	struct cbBlur {
		Math::Vector4	Offset[31];
	};
	ConstantBuffer<cbBlur>	m_cb0_Blur;

	//
	ID3D11PixelShader* m_HBrightPS = nullptr;

};
