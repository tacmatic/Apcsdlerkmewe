#pragma once 

namespace render
{

	class c_render
	{
	private:

		std::mutex m_mutex{};

		std::unique_ptr<m_interface::c_interface>	m_interface{};

	public:

		c_render();

		bool setup(const std::wstring& proc_name);

		std::string get_window_name(HWND window);
		HWND get_window_handle(std::uint32_t pid);

		bool get_screen_status();
		bool get_window();

		void clean_context();
		void release_objects();

		void tick();

	public:

		MARGINS m_margin{ -1 };
		RECT m_rect{ };
		MSG m_msg{ };

		std::uint32_t m_pid{ };
		std::string m_name{ };

		HWND m_window_target{ };
		HWND m_overlay{ };

		int m_width{ };
		int m_height{ };

		int m_width_2{ };
		int m_height_2{ };
	};
} inline std::shared_ptr<render::c_render> m_render = std::make_shared<render::c_render>();

#include <d3d11.h>
#include <d3dcompiler.h>

#include "../resources/misc/Timer.h"

#include "../resources/imgui/imgui.h"
#include "../resources/imgui/imgui_impl_dx11.h"
#include "../resources/imgui/imgui_impl_win32.h"
#include "../resources/imgui/imgui_internal.h"	// gradient

constexpr int menu_anim_time = 350;
constexpr int breath_anim_time = 1000;

class c_overlay {
private:
	HWND								window_handle;
	WNDCLASSEX							window_class;
	ID3D11Device* d3d_device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target_view;
	ImDrawList* draw_list;
	c_timer								breath_timer, menu_timer;

	struct s_font {
		ImFont* im_font;
		FLOAT font_size;
	};
private:
	template <typename T>
	inline VOID safe_release(T*& p);

	BOOL init_device();
	VOID dest_device();

	VOID init_imgui();
	VOID dest_imgui();

	VOID init_render_target();
	VOID dest_render_target();
	const VOID render(FLOAT width, FLOAT height);

	void input_handler();
	void anim_handler();
public:
	c_overlay();
	~c_overlay();

	BOOL msg_loop();
	void ClickThrough(bool v);

	VOID init_draw_list();
	VOID crosshair(const FLOAT aSize, ImU32 color);
	VOID box(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width = 2.f);
	VOID filledRect(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width = 2.f);
	VOID line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width = 2.f);
	VOID circle(const ImVec2& point, const FLOAT radius, ImU32 color);
	VOID circleFilled(const ImVec2& point, const FLOAT radius, ImU32 color);
	VOID text(const ImVec2& pos, float size, const std::string& text, ImU32 color = ImColor(240, 248, 255), bool outline = true);
	VOID radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);
	VOID DrawHealthBar(ImVec2 Pos, int health, int maxHealth, ImColor Health_Color);
	void RenderMenu(ImGuiStyle& style);
	
	BOOL in_screen(const ImVec2& pos);
public:
	FLOAT window_width, window_height;
	bool exit, ready, show_menu = false;
	INT breath, menu_ticks;
	s_font* font;
};
