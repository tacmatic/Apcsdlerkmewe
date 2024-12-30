#pragma once


enum InjectedInputMouseOptions
{
	move = 1,
	left_up = 4,
	left_down = 2,
	right_up = 8,
	right_down = 16
};

struct InjectedInputMouseInfo
{
	int move_direction_x;
	int move_direction_y;
	unsigned int mouse_data;
	InjectedInputMouseOptions mouse_options;
	unsigned int time_offset_in_miliseconds;
	void* extra_info;
};

inline bool(*_NtUserInjectMouseInput)(InjectedInputMouseInfo*, int) = nullptr;

namespace aimbot
{
	class c_aimbot
	{
	public:

		auto initialize() -> bool;
		auto set_cursor_position(int x, int y) -> bool;
		auto left_down() -> bool;
		auto left_up() -> bool;
		auto target_screen_pos(int x, int y,float smoothnes) -> void;

		void tick();
		c_aimbot() { this->initialize(); }
	};

} inline auto m_aimbot = std::make_unique<aimbot::c_aimbot>();

