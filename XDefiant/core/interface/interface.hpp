#ifndef INTERFACE_HPP
#define INTERFACE_HPP

namespace m_interface
{
	struct c_tab {
		int m_num;
		const char* m_label;

		std::function< void( ) > page;

		c_tab( int num, const char* label ) {
			m_num = num;
			m_label = label;
		}
	};

	class c_interface
	{
	};
};

#endif 