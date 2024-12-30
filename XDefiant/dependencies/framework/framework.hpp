#pragma once 
namespace framework {

	enum class request : std::uint8_t {
		m_endframe,
		m_add_player
	};

	using vec2 = struct {
		float x,y;
	};

	using rect_t = struct {
		float height,width,x,y;
	};

	using new_player_t = struct {
		vec2 box_location;
		vec2 box_size;
		
		float top, bottom, left, right;
	};

	class c_framework {

	public:

		c_framework( ) = default;

		[[nodiscard]] uemath::flinearcolor get_color( int a,int r,int g,int b );

		void end_scene( );
		void begin_scene( );

		void add_player( new_player_t* player_info_t );

	};

} inline std::shared_ptr<framework::c_framework> m_framework = std::make_shared<framework::c_framework>( );