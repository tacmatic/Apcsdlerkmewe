#include "..\includes.hpp"
#include <stack>

constexpr bool developer = true;

// constructor 
render::c_render::c_render()
{

}

auto render::c_render::setup(
	const std::wstring& proc_name) -> bool {

	m_target_pid = m_util.get()->get_process_id(proc_name.c_str());
	if (!m_target_pid) {
		print_error(OBFUSCATE_STR("failed to get application pid.\n"), false);
		return false;
	}

	auto w_handle = this->get_window_handle(m_target_pid);
	if (!w_handle) {
		print_error(OBFUSCATE_STR("failed to get window handle.\n"), false);
		return false;
	}

	auto w_name = this->get_window_name(w_handle);
	if (w_name.empty()) {
		print_error(OBFUSCATE_STR("failed to get window name.\n"), false);
		return false;
	}

	this->m_interface = std::make_unique<m_interface::c_interface>();
	//this->m_setting = std::make_unique<settings::c_settings>();
	this->m_pid = std::move(m_target_pid);
	this->m_name = std::move(w_name);
	this->m_window_target = std::move(w_handle);

	//this->m_setting->load();

	if (!this->m_interface.get())
		return false;

	//if (!this->m_setting.get())
	//	return false;

	auto get_window = this->get_window();
	if (!get_window) {
		LOG(OBFUSCATE_STR("failed to get window info.\n"));
		return false;
	}

	print_info(OBFUSCATE_STR("pid: %i\n"), this->m_pid);
	print_info(OBFUSCATE_STR("target window: %llx\n"), this->m_window_target);
	print_info(OBFUSCATE_STR("width: %i\n"), this->m_width);
	print_info(OBFUSCATE_STR("height: %i\n"), this->m_height);

	return true;
}

auto render::c_render::release_objects() -> void
{

}

auto render::c_render::clean_context() -> void
{

}

auto render::c_render::get_window() -> bool
{
	auto result = GetWindowRect(
		this->m_window_target,
		&this->m_rect);
	if (!result) {
		return false;
	}

	this->m_width = m_rect.right - m_rect.left;
	this->m_height = m_rect.bottom - m_rect.top;

	this->m_width_2 = m_width / 2;
	this->m_height_2 = m_height / 2;

	m_engine.get()->m_width = m_rect.right - m_rect.left;
	m_engine.get()->m_height = m_rect.bottom - m_rect.top;

	m_engine.get()->m_width_2 = m_width / 2;
	m_engine.get()->m_height_2 = m_height / 2;

	return true;
}

auto render::c_render::get_window_name(
	HWND window) -> std::string
{

	wchar_t title[1024];
	GetWindowTextW(window, title, 1024);

	// Convert wide character array to narrow character array
	char title_conver[1024];
	std::wcstombs(title_conver, title, 1024);


	return std::string(title_conver);
}

auto render::c_render::get_window_handle(
	std::uint32_t pid) -> HWND
{
	std::pair<HWND, DWORD> params = { 0, pid };

	auto result = EnumWindows([](HWND hwnd, LPARAM lParam) -> int
		{
			auto pParams = (std::pair<HWND, DWORD>*)(lParam);

			DWORD processId;
			if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
			{

				SetLastError(-1);
				pParams->first = hwnd;
				return false;
			}

			return true;

		}, reinterpret_cast<LPARAM>(&params));

	if (!result && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

auto render::c_render::get_screen_status() -> bool
{
	if (this->m_window_target == GetForegroundWindow()) {
		return true;
	}

	if (this->m_window_target == GetActiveWindow()) {
		return true;
	}

	if (GetActiveWindow() == GetForegroundWindow()) {
		return true;
	}

	return false;
}


#include "../../resources/misc/skcrypt.hpp"

#include <Windows.h>
#include <d3d9.h>
#include <d3d11.h>
#include <string>
#include <dwmapi.h>

#include "../includes.hpp"
#include "render.hpp"

namespace customControls {
	void ToggleButton(const char* str_id, bool* v)
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height * 1.55f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), colors[ImGuiCol_ButtonActive], t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), colors[ImGuiCol_Button], t));
		//col_bg = ImGui::GetColorU32(ImLerp(colors[ImGuiCol_Button], ImVec4(0.85f, 0.85f, 0.85f, 1.0f), t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 10.f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255), 1000);
	}

	bool SideNavButton(const char* label, bool active, float width, float height) {
		bool returnValue = false;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::InvisibleButton(label, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			returnValue = true;


		ImGuiContext& g = *GImGui;

		if (active == true) {
			draw_list->AddRectFilled({ p.x, p.y }, { p.x + width, p.y + height }, IM_COL32(26, 26, 43, 255));
			draw_list->AddRectFilled({ p.x, p.y }, { p.x + 5, p.y + height }, IM_COL32(121, 71, 255, 255));
			draw_list->AddRectFilledMultiColor({ p.x + 5, p.y }, { p.x + 5 + (width / 10) , p.y + height }, IM_COL32(121, 71, 255, 255), IM_COL32(26, 26, 43, 0), IM_COL32(26, 26, 43, 0), IM_COL32(121, 71, 255, 255));
		}
		else {
			if (ImGui::IsItemHovered()) {
				draw_list->AddRectFilled({ p.x, p.y }, { p.x + width, p.y + height }, IM_COL32(0.20f, 0.25f, 0.29f, 1.00f));
			}
			else {
				draw_list->AddRectFilled({ p.x, p.y }, { p.x + width, p.y + height }, IM_COL32(0.09f, 0.09f, 0.15f, 0.f));
			}
		}

		draw_list->AddText({ p.x + 20.f, p.y + (height / 3.15f) }, IM_COL32(255, 255, 255, 255), label);
		return returnValue;
	}

	bool Checkbox(const char* label, bool* v)

	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = g.Style;

		style.FrameRounding = 3.0f;

		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 tmp3 = ImVec2(label_size.y + style.FramePadding.y * 0.5, label_size.y + style.FramePadding.y * 0.5);
		ImVec2 tmp4 = ImVec2(window->DC.CursorPos.x + tmp3.x, window->DC.CursorPos.y + tmp3.y);

		const ImRect check_bb(window->DC.CursorPos, tmp4);
		ImGui::ItemSize(check_bb, style.FramePadding.y);


		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			ImGui::SameLine(0, style.ItemInnerSpacing.x);


		ImVec2 tmp1 = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + 2);
		ImVec2 tmp2 = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y);
		tmp2.x = tmp2.x + label_size.x;
		tmp2.y = tmp2.y + label_size.y;

		const ImRect text_bb(tmp1, tmp2);
		if (label_size.x > 0)
		{
			ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			*v = !(*v);

		ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		if (*v)
		{
			const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
			const float pad = ImMax(1.0f, (float)(int)(check_sz / 6.0f));
			const ImVec2 pts[] = {
				ImVec2 {check_bb.Min.x + pad, check_bb.Min.y + ((check_bb.Max.y - check_bb.Min.y) / 2)},
				ImVec2 {check_bb.Min.x + ((check_bb.Max.x - check_bb.Min.x) / 3), check_bb.Max.y - pad * 1.5f},
				ImVec2 {check_bb.Max.x - pad, check_bb.Min.y + pad}
			};

			window->DrawList->AddPolyline(pts, 3, ImGui::GetColorU32(ImGuiCol_CheckMark), false, 2.0f);
		}


		ImVec2 tmp5 = text_bb.GetTL();

		if (label_size.x > 0.0f)
			ImGui::RenderText({ tmp5.x, tmp5.y - 2 }, label);



		style.FrameRounding = 0.0f;

		return pressed;
	}

	void PanelHeader(const char* label, float width, float height) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 tmp3 = ImVec2(label_size.y + style.FramePadding.y * 0.5, label_size.y + style.FramePadding.y * 0.5);
		ImVec2 tmp4 = ImVec2(window->DC.CursorPos.x + tmp3.x, window->DC.CursorPos.y + tmp3.y);

		const ImRect check_bb(window->DC.CursorPos, tmp4);
		ImGui::ItemSize(check_bb, style.FramePadding.y);


		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			ImGui::SameLine(0, style.ItemInnerSpacing.x);


		ImVec2 tmp1 = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + 2);
		ImVec2 tmp2 = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y);
		tmp2.x = tmp2.x + label_size.x;
		tmp2.y = tmp2.y + label_size.y;

		const ImRect text_bb(tmp1, tmp2);
		if (label_size.x > 0)
		{
			ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ImGui::ItemAdd(total_bb, id))
			return;

		window->DrawList->AddRectFilled({ tmp4.x - 12, tmp4.y - 12 }, { tmp4.x - 12 + width, tmp4.y - 12 + height }, IM_COL32(0.15f, 0.18f, 0.22f, 1.f));
		//window->DrawList->AddRectFilledMultiColor({ tmp4.x - 12, tmp4.y - 12 }, { tmp4.x - 12 + (width / 5), tmp4.y - 12 + height }, IM_COL32(71, 143, 255, 64), IM_COL32(28, 38, 45, 0), IM_COL32(28, 38, 45, 0), IM_COL32(71, 143, 255, 64));

		float gradient_start_x = (tmp4.x - 12) + width * 0.2f; // Gradient starts after 20% of the width
		window->DrawList->AddRectFilled({ tmp4.x - 12, tmp4.y - 12 }, { gradient_start_x, tmp4.y - 12 + height }, IM_COL32(121, 71, 255, 100));
		window->DrawList->AddRectFilledMultiColor(
			ImVec2(gradient_start_x, (tmp4.y - 12)),
			ImVec2((tmp4.x - 12) + (width / 1.5), (tmp4.y - 12) + height),
			IM_COL32(121, 71, 255, 100), IM_COL32(0.15f, 0.18f, 0.22f, 0.11f), IM_COL32(0.15f, 0.18f, 0.22f, 0.11f), IM_COL32(121, 71, 255, 100)
		);


		ImVec2 tmp5 = text_bb.GetTL();

		ImGui::RenderText({ tmp5.x , tmp5.y + 6 }, label);

		return;
	}

}


// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p) {
		p->Release();
		p = NULL;
	}
}

BOOL c_overlay::init_device() {
	ClickThrough(!show_menu);
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
	ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
	vSwapChainDesc.BufferCount = 2;
	vSwapChainDesc.BufferDesc.Width = 0;
	vSwapChainDesc.BufferDesc.Height = 0;
	vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 200;
	vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	vSwapChainDesc.OutputWindow = window_handle;
	vSwapChainDesc.SampleDesc.Count = 1;
	vSwapChainDesc.SampleDesc.Quality = 0;
	vSwapChainDesc.Windowed = TRUE;
	vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL vFeatureLevel;
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &vSwapChainDesc, &swap_chain, &d3d_device, &vFeatureLevel, &device_context) != S_OK)
		return false;

	init_render_target();

	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	return true;
}

VOID c_overlay::dest_device() {
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}

void StyleColorsCustom(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors;


	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.17f, 1.00f);


	colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);

	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.47f, 0.47f, 1.f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.2f, 0.29f, 1.00f);

	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 1.f, 0.5f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);

	colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.07f, 0.12f, 0.65f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.07f, 0.12f, 1.00f);

	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);

	colors[ImGuiCol_CheckMark] = ImVec4(0.47f, 0.47f, 1.f, 1.00f);


	colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.47f, 1.f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.47f, 0.47f, 1.f, 0.5f);

	colors[ImGuiCol_Button] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


	style->GrabRounding = 4.0f;
	style->FrameRounding = 4.0f;
	style->ChildRounding = 8.0f;
	style->ChildBorderSize = 0.f;
	style->AntiAliasedFill = true;
	style->AntiAliasedLines = true;
}




VOID c_overlay::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGuiStyle& style = ImGui::GetStyle();

	//StyleColorsCustom(&style);

	ImGuiIO& io = ImGui::GetIO();
	font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 12.f);
	font->font_size = 12.f;

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer)
		d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	// todo: error here if it can't get the backbuffer of the render target
	back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
	if (!render_target_view)
		return;

	render_target_view->Release();
	render_target_view = NULL;
}

c_overlay::c_overlay() :
	ready(false),
	exit(false),
	window_handle(nullptr),
	window_class({}),
	window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
	window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
	font(nullptr),
	d3d_device(nullptr),
	device_context(nullptr),
	swap_chain(nullptr),
	render_target_view(nullptr),
	draw_list(nullptr),
	breath(NULL),
	menu_ticks(NULL)
{
	// get a handle to the nvidia window
	//window_handle = FindWindowA(skCrypt("MedalOverlayClass"), skCrypt("MedalOverlay")); //Hijack nvidia share, bc for some reason this is fucking possible?
	window_handle = FindWindowA(skCrypt("Chrome_WidgetWin_1"), skCrypt("Medal Overlay"));// WTF medal update window name change if no ui render use old one or find the new name if medal crashes i think this is the alternitave menu
	auto getInfo = GetWindowLongA(window_handle, -20);
	auto changeAttributes = SetWindowLongA(window_handle, -20, (LONG_PTR)(getInfo | 0x20));

	// avoiding window flags: https://www.unknowncheats.me/forum/general-programming-and-reversing/229070-overlay-window-topmost-top-game-windows-video.html
	MARGINS margins = { -1 };
	margins.cyBottomHeight = margins.cyTopHeight = margins.cxLeftWidth = margins.cxRightWidth = -1;
	DwmExtendFrameIntoClientArea(window_handle, &margins);
	SetLayeredWindowAttributes(window_handle, 0x000000, 0xFF, 0x02);
	SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);

	if (!window_handle)
		return; // error grabbing nvidia overlay

	font = new s_font();

	if (!init_device())
		return; // error creating device

	init_imgui();

	std::thread(&c_overlay::input_handler, this).detach();
	std::thread(&c_overlay::anim_handler, this).detach();
}

c_overlay::~c_overlay() {
	dest_imgui();
	dest_device();

	delete font;
}
int RecordMouseButtonPress() {
	// Map mouse buttons to VK codes
	const int mouseButtonToVK[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };
	const int mouseButtonCount = sizeof(mouseButtonToVK) / sizeof(mouseButtonToVK[0]);

	while (true) {
		for (int i = 0; i < mouseButtonCount; ++i) {
			if (GetAsyncKeyState(mouseButtonToVK[i]) & 0x8000) {
				return mouseButtonToVK[i];
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
void c_overlay::RenderMenu(ImGuiStyle& style) {
	static bool test = false;
	// imgui menu itself
	//style.FramePadding = ImVec2(0, 0);
	style.CellPadding = ImVec2(0, 0);
	style.WindowPadding = ImVec2(0, 0);
	style.WindowBorderSize = 0.0f;

	style.GrabRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.ChildRounding = 0.0f;

	float totalWidth = 380;
	float totalHeight = 540;

	float sidebarWidth = 100;
	float sidebarButtonHeight = 31.f;

	float pad10 = 10;

	float mainContentWidth = 410;
	float mainContentHeight = 225;

	float espPanelWidth = 200;
	float espPanelHeight = 225;

	float weaponPanelWidth1 = 175;
	float weaponPanelWidth2 = 225;
	float weaponPanelHeight = 225;

	float panelHeaderHeight = 28;

	ImGui::SetNextWindowSize(ImVec2(totalWidth, totalHeight));

	style.FramePadding = ImVec2(7, 7);

	ImGui::Begin("adthaerth", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	// menu stuff

	ImGui::End();

}

void RenderInfo(ImGuiStyle& style)
{
	ImGui::SetNextWindowPos(ImVec2(4, 20));
	ImGui::SetNextWindowSize(ImVec2(111, 0.f));

	style.WindowBorderSize = 1.0f;
	style.CellPadding = ImVec2(5, 5);

	ImGui::Begin("##info", (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPosY() + 10));
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); ImGui::SameLine();
	

	ImGui::End();



	style.WindowBorderSize = 0.0f;
}


LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG nv_default_in_game = nv_default | WS_DISABLED;
LONG nv_edit = nv_default_in_game | WS_VISIBLE;

LONG nv_ex_default = WS_EX_TOOLWINDOW;
LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG nv_ex_edit_menu = nv_ex_default | WS_EX_TRANSPARENT;

void c_overlay::ClickThrough(bool v)
{
	if (v)
	{
		nv_edit = nv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(window_handle, GWL_EXSTYLE) != nv_ex_edit)
			SetWindowLong(window_handle, GWL_EXSTYLE, nv_ex_edit);
	}
	else
	{
		SetForegroundWindow(window_handle);
		nv_edit = nv_default | WS_VISIBLE;
		if (GetWindowLong(window_handle, GWL_EXSTYLE) != nv_ex_edit_menu)
			SetWindowLong(window_handle, GWL_EXSTYLE, nv_ex_edit_menu);
	}
}
#include "../entity/entity.hpp"

const void c_overlay::render(FLOAT width, FLOAT height) {
	if (exit) {
		ready = true;
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	StyleColorsCustom(&style);



	init_draw_list(); 

	for (const auto& Player : m_entity->PlayersCache)
	{
		//each entiuty proces
		//if (Setting::Line)
		//{
		//	line({ m_entity->localplayer.org.x,m_entity->localplayer.org.y }, { Player.org.x,Player.org.y }, ImColor{ Setting::Line_color.x,Setting::Line_color.y,Setting::Line_color.z,Setting::Line_color.a }, 2);
		//}
		//else if (Setting::aim::Aim_Line)
		//{
		//	line({ m_entity->localplayer.org.x,m_entity->localplayer.org.y }, { Player.aim.x,Player.aim.y }, ImColor(20, 200, 20, 225), 2);
		//}
		//
		//if (Setting::aim::Aim_Point)
		//{
		//	circle
		//	(
		//		{ Player.aim.x,Player.aim.y},
		//		ImColor{ Setting::aim::Aim_Point_color.x,Setting::aim::Aim_Point_color.y,Setting::aim::Aim_Point_color.z,Setting::aim::Aim_Point_color.a }, 2);
		//}
		
	}

	
	//circle({ (float)ScreenX / 2.f, (float)ScreenY / 2.f }, Setting::FOV, IM_COL32_BLACK);
	crosshair(10, ImColor(3, 232, 244));

	if (show_menu)
		RenderMenu(style);
	
	RenderInfo(style);
}

BOOL c_overlay::msg_loop() {
	if (window_handle) {
		MSG msg{ 0 };
		ImVec4 clear_clr = { .01f, .01f, .01f, .01f };

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render(window_width, window_height);

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr); // might break, if so then put cheat color of type ImVec4 { 0,0,0,0 }
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(1, 0);

		return msg.message != WM_QUIT;
	}
	return false;
}
void c_overlay::input_handler() {
	// added a sleep per user @colbyfortnite's suggestion
	for (; !exit; Sleep(1)) {
		// for our imgui menu interaction
		ImGuiIO& io = ImGui::GetIO();

		POINT p{};
		GetCursorPos(&p);
		io.MousePos = ImVec2((float)p.x, (float)p.y);

		io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
	}
}
bool k_ins = false;
void c_overlay::anim_handler() {
	for (; !exit;) {
		RECT rect{ 0 };
		POINT point{ 0 };
		GetClientRect(window_handle, &rect);
		SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		// panic key
		exit = GetAsyncKeyState(VK_END) & 0x8000;

		// timer for menu circle animation & fade in
		int menu_tmr = menu_timer.end();
		if ((GetAsyncKeyState(VK_INSERT) & 0x8000) != 0 && !k_ins) {
			k_ins = true;
			show_menu = !show_menu;
			ClickThrough(!show_menu);
		}
		else if ((GetAsyncKeyState(VK_INSERT) & 0x8000) == 0 && k_ins) {
			k_ins = false;
		}
	}
}

VOID c_overlay::init_draw_list() {
	draw_list = ImGui::GetBackgroundDrawList();
}

VOID c_overlay::crosshair(const FLOAT aSize, ImU32 color) {
	draw_list->AddLine({ window_width / 2,window_height / 2 - (aSize + 1) }, { window_width / 2 ,window_height / 2 + (aSize + 1) }, color, 2);
	draw_list->AddLine({ window_width / 2 - (aSize + 1),window_height / 2 }, { window_width / 2 + (aSize + 1), window_height / 2 }, color, 2);
}

VOID c_overlay::filledRect(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width) {
	draw_list->AddRectFilled({ pos.x, pos.y }, { pos.x + width, pos.y + height }, color);
}

VOID c_overlay::box(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width) {
	std::array<ImVec2, 4> box_lines{
		pos,
		ImVec2{pos.x + width,pos.y},
		ImVec2{pos.x + width,pos.y + height},
		ImVec2{pos.x,pos.y + height}
	};
	draw_list->AddPolyline(box_lines.data(), box_lines.size(), color, true, 1);
}

VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width) {
	draw_list->AddLine(point1, point2, color, line_width);
}

VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircle(point, radius, color, 200); // num segments is how circular the circle actually is
}

VOID c_overlay::circleFilled(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircleFilled(point, radius, color, 200); // num segments is how circular the circle actually is
}

VOID c_overlay::text(const ImVec2& pos, float size, const std::string& text, ImU32 color, bool outline) {
	if (outline) {
		draw_list->AddText(font->im_font, size, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, size, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, size, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, size, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font->im_font, size, pos, color, text.c_str());
}

VOID c_overlay::radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out) {
	if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
		return;

	draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
	const int count = draw_list->_Path.Size - 1;

	unsigned int vtx_base = draw_list->_VtxCurrentIdx;
	draw_list->PrimReserve(count * 3, count + 1);

	const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
	draw_list->PrimWriteVtx(center, uv, col_in);
	for (int n = 0; n < count; n++)
		draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);

	for (int n = 0; n < count; n++) {
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	draw_list->_Path.Size = 0;
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}

VOID c_overlay::DrawHealthBar(ImVec2 Pos, int health, int maxHealth, ImColor Health_Color) {
	auto draw_list = ImGui::GetBackgroundDrawList();

	// Define bar dimensions
	ImVec2 barSize(100, 10);
	float fillRatio = (float)health / maxHealth;

	// Draw outline
	draw_list->AddRect(Pos, ImVec2(Pos.x + barSize.x, Pos.y + barSize.y), IM_COL32(0, 0, 0, 255));

	// Draw filled portion
	draw_list->AddRectFilled(Pos, ImVec2(Pos.x + barSize.x * fillRatio, Pos.y + barSize.y), Health_Color);

	// Draw text
	//char buffer[32];
	//snprintf(buffer, sizeof(buffer), "%d / %d", health, maxHealth);
	//draw_list->AddText(ImVec2(Pos.x + 5, Pos.y + 1), IM_COL32(255, 255, 255, 255), buffer);
}