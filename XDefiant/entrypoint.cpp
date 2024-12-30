#pragma once
#include "core/includes.hpp"
#include "resources/user.hpp"



auto main() -> int
{
	if (!m_util.get()->attach_console())
		return false;

	if (!m_vm.get()->attach_vm((L"r5apex.exe")))
		return false;

	if (!m_render.get()->setup((L"r5apex.exe")))
		return false;


	std::jthread([&]() { 
	
		m_entity.get()->tick();
	
		}).detach();

	//std::jthread([&]() {
	//
	//	m_aimbot.get()->tick();
	//
	//	}).detach();

	std::jthread([&]() { 

		//HANDLE hThread = GetCurrentThread();
		//SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

		c_overlay overlay; 
		while (overlay.msg_loop() && !overlay.ready) {}; 
	
		}).detach();



	return std::cin.get( ) != EOF ? false : true;
}