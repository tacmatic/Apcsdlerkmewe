#pragma once 
#include "../includes.hpp"
#include "../sdk/math/Vector.hpp"

#include <Windows.h>

#include "aimbot.hpp"


auto aimbot::c_aimbot::initialize() -> bool {
	auto win32u = import(LoadLibraryA).safe_cached()("win32u.dll");
	void* NtUserInjectMouseInputAddress = (void*)import(GetProcAddress).safe_cached()(win32u, "NtUserInjectMouseInput");
	if (!NtUserInjectMouseInputAddress) return false;
	*(void**)&_NtUserInjectMouseInput = NtUserInjectMouseInputAddress;
import(FreeLibrary).get()(win32u);
	return true;
}

auto aimbot::c_aimbot::set_cursor_position(
	int x, int y) -> bool
{
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::move;
	temp.move_direction_x = x;
	temp.move_direction_y = y;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput(&temp, 1);
}

auto aimbot::c_aimbot::left_down() -> bool {
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::left_down;
	temp.move_direction_x = 0;
	temp.move_direction_y = 0;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput(&temp, 1);
}

auto aimbot::c_aimbot::left_up() -> bool {
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::left_up;
	temp.move_direction_x = 0;
	temp.move_direction_y = 0;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput(&temp, 1);
}

struct Vec2
{
	float x, y;
};

auto aimbot::c_aimbot::target_screen_pos(int x, int y, float smoothnes) -> void {

	Vec2 ScreenCenter = { (float)ScreenX / 2 , (float)ScreenY / 2 };
	Vec2 Target;

	if (x != 0)
	{
		if (x > ScreenCenter.x)
		{
			Target.x = -(ScreenCenter.x - x);
			Target.x /= smoothnes;
			if (Target.x + ScreenCenter.x > ScreenCenter.x * 2) Target.x = 0;
		}

		if (x < ScreenCenter.x)
		{
			Target.x = x - ScreenCenter.x;
			Target.x /= smoothnes;
			if (Target.x + ScreenCenter.x < 0) Target.x = 0;
		}
	}
	if (y != 0)
	{
		if (y > ScreenCenter.y)
		{
			Target.y = -(ScreenCenter.y - y);
			Target.y /= smoothnes;
			if (Target.y + ScreenCenter.y > ScreenCenter.y * 2) Target.y = 0;
		}

		if (y < ScreenCenter.y)
		{
			Target.y = y - ScreenCenter.y;
			Target.y /= smoothnes;
			if (Target.y + ScreenCenter.y < 0) Target.y = 0;
		}
	}

	set_cursor_position(Target.x, Target.y);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

double distanceFromCenter(const Vector2D& pos, int centerX, int centerY) {
	return std::sqrt(std::pow(centerX - pos.x, 2) + std::pow(centerY - pos.y, 2));
}


Vector2D findBestPosition(const std::vector<Vector2D>& positions, int screenWidth, int screenHeight) {
	if (positions.empty()) {
		return { 0.f,0.f };
	}

	Vector2D center = { (float)(screenWidth / 2), (float)(screenHeight / 2) };
	Vector2D bestPos = positions[0];
	double bestDist = distanceFromCenter(bestPos, center.x, center.y);

	for (const auto& pos : positions) {
		double dist = distanceFromCenter(pos, center.x, center.y);
		if (dist < bestDist) {
			bestPos = pos;
			bestDist = dist;
		}
	}

	return bestPos;
}


//#include <iostream>
//#include <windows.h>
//#include <random>
//#include <dwmapi.h>
//typedef BOOL(*mouse_event_t)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR);
//inline BOOL SpoofedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo)
//{
//	static mouse_event_t origmouse_event = (mouse_event_t)GetProcAddress(GetModuleHandleA("user32.dll"), "mouse_event");
//
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_int_distribution<> dis(0, 1);
//	int random_number = dis(gen);
//	return origmouse_event(dwFlags, dx, dy, dwData, dwExtraInfo);
//}
//float distance(const Vector2D& a, const Vector2D& b) {
//	return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
//}
//
//OverLayInfo getClosestToScreenPoint(const Vector2D& point) {
//	OverLayInfo closestTarget;
//	float minDist = std::numeric_limits<float>::max();
//	
//	for (const auto& target : m_entity->m_targetInfoOverlay) {
//		float dist = distance(point, target.aim);
//		if (dist < minDist) {
//			minDist = dist;
//			closestTarget = target;
//		}
//	}
//	
//	return closestTarget;
//}
//
//void aimbot::c_aimbot::tick()
//{
//	for (;;)
//	{
//		m_entity->Target = getClosestToScreenPoint({ (float)ScreenX / 2,  (float)ScreenY / 2 });
//		if (getKeyDown(VK_XBUTTON1) && Setting::aimbot && m_entity->lobyinfo.aimbotActive)
//		{
//			Vector2D pos = m_entity->Target.aim;
//			if (pos.x != ScreenX && pos.y != ScreenY && pos.x != 0 && pos.y != 0)
//			{
//				//target_screen_pos(pos.x, pos.y, 2.f);
//				Vector2D smoothnes = { pos.x + (pos.x / Setting::smoothnes) / 2 ,pos.y + (pos.y / Setting::smoothnes)/2 };
//				SpoofedMouseEvent(MOUSEEVENTF_MOVE, pos.x - ScreenX / 2, pos.y - ScreenY / 2, NULL, NULL);
//
//			}
//		}
//		
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
//	}
//}