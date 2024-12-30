#pragma once 
#include "../includes.hpp"
#include "../sdk/math/Vector.hpp"
#include "entity.hpp"

static inline std::vector<Player> TempPlayersCache{};
void entity::c_entity::tick() 
{
    while (true)
    {
		uint64_t localplayerPTR = m_vm.get()->read<std::uintptr_t>(m_vm.get()->m_image_base + 0x22b0328);//OFFSET_LOCAL_ENT
		TempPlayersCache.clear();
		for (int i = 0; i < 128; ++i)//player cacheing loop
		{
			//offsets  https://pastebin.com/raw/06HSv0x6
			uint64_t entity_ptr = m_vm.get()->read<uint64_t>(m_vm.get()->m_image_base + 0x1ef7c38/*OFFSET_ENTITYLIST*/ + (i << 5));

			if (entity_ptr == 0)
				continue;

			
			Player playertemp;

			//playertemp.team = m_vm.get()->read<int>(entity_ptr + OFFSET_TEAM);
			//playertemp.health = m_vm.get()->read<int>(entity_ptr + OFFSET_Health);
			//playertemp.isDown = m_vm.get()->read<bool>(entity_ptr + OFFSET_State);
			//playertemp.name = m_vm.get()->read<std::string>(entity_ptr + OFFSET_Name);
			//playertemp.isAlive = m_vm.get()->read<bool>(entity_ptr + OFFSET_Alive);
			//playertemp.pos = m_vm.get()->read<Vector>(entity_ptr + OFFSET_ORGIN);
			TempPlayersCache.push_back(playertemp);
		}
		PlayersCache = TempPlayersCache;



		//normal single loop 
		m_vm.get()->write<Vector>(
			reinterpret_cast<uint64_t>(localplayerPTR) + 0x2544 - 0x14,/*OFFSET_VIEWANGLES */ 
			Vector(0.f, 0.f, 0.f)
		);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));// NEEDS TO SLEEP a litle or will over heat cpu
    }
}
