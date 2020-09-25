#pragma once

//==============================
//ログウィンドウクラス
//==============================

class ImGuiLogWindow 
{
public:
	//ログをクリア
	void Clear() { m_Buf.clear(); }

	//文字列を追加
	template<class...Args>
	void AddLog(const char* fmt,Args...args)
	{
		//改行を加える
		std::string str = fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), args...);
		m_ScrollToButton = true;
	}

	//ウィンドウ描画
	void ImGuiUpdate(const char* title,bool *p_opened = NULL)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(m_Buf.begin());
		if (m_ScrollToButton)
			ImGui::SetScrollHere(1.0f);
		m_ScrollToButton = false;
		ImGui::End();
	}
private:
	ImGuiTextBuffer	m_Buf;
	bool			m_ScrollToButton = false;

//===================================================
//シングルトン実装
//===================================================
public:
	//実体取得
	static ImGuiLogWindow& GetInstance()
	{
		static ImGuiLogWindow instance;
		return instance;
	}
private:
	//コンストラクタ
	ImGuiLogWindow() {};
};

#define IMGUI_LOG ImGuiLogWindow::GetInstance()