#pragma once 
#include "../../core/includes.hpp"

auto framework::c_framework::get_color( int a,int r,int g,int b ) -> uemath::flinearcolor {

	return uemath::flinearcolor { 
		static_cast< std::int8_t >( a ),
		static_cast< std::int8_t >( r ),
		static_cast< std::int8_t >( g ),
		static_cast< std::int8_t >( b )
	};

}

auto framework::c_framework::end_scene( ) -> void
{
	auto end_frame = request::m_endframe;

	m_ipc.get( )->insert_object(
		&end_frame,
		sizeof( std::uint8_t )
	);

	ReleaseMutex(
		m_ipc.get( )->mutex
	);
}


auto framework::c_framework::begin_scene( ) -> void
{
	WaitForSingleObject(
		m_ipc.get( )->mutex,
		0xFFFFFFFF
	);

	m_ipc.get( )->offset = 0;
}


void framework::c_framework::add_player( new_player_t *player_info_t ) {

	auto box_location = player_info_t->box_location;
	auto box_size = player_info_t->box_size;

	auto object = new_player_t{ box_location, box_size };
	auto object_id = request::m_add_player;

	m_ipc.get( )->insert_object(
		&object_id,
		sizeof( std::uint8_t )
	);

	m_ipc.get( )->insert_object(
		&object,
		sizeof( new_player_t )
	);

}
