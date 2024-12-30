#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "Vector.hpp"


#define ScreenX m_render->m_width
#define ScreenY m_render->m_height


struct Player
{
    int health = 0;
    int team = 0;

    bool isAlive = false;
    bool isDown = false;

    std::string name = "";
};

namespace entity
{
    class c_entity
    {
    public:
        std::vector<Player> PlayersCache {};


        ALWAYS_INLINE c_entity() {
            PlayersCache.reserve(128);
        };

        void tick();

    };

} inline auto m_entity = std::make_unique<entity::c_entity>();
