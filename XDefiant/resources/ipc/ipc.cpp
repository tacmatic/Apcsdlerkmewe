#pragma once
#include "../../core/includes.hpp"

ipc::c_ipc::c_ipc()
{

}

auto ipc::c_ipc::insert_object(
	const void* source, std::size_t size) -> void
{
	memcpy_s(
		reinterpret_cast<void*>((char*)m_ipc.get()->buffer + m_ipc.get()->offset),
		size,
		source,
		size
	);

	m_ipc.get()->offset += size;

	//// 0xF4240
	//if ( this->offset + size > 0xF4240 ) {
	//	LOG( OBFUSCATE_STR( "overflow detected.\n" ) );
	//	this->offset = 0;
	//}

	//std::memcpy(
	//	reinterpret_cast< void* >( reinterpret_cast< char* >( this->buffer ) + this->offset ),
	//	source,
	//	size );

	//this->offset += size;
}

auto ipc::c_ipc::map_buffer() -> bool
{
	buffer = MapViewOfFile(
		this->ipc,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		//4096 // !page
		sizeof(TCHAR) * 0xF4240
	);

	return buffer != nullptr;
}

auto ipc::c_ipc::create_mutex(
	const std::string& module) -> bool
{
	this->mutex = OpenMutexA(
		0x00100000L, // !synchronize
		false,
		module.c_str()
	);

	return this->mutex != nullptr;
}

auto ipc::c_ipc::connect_buffer(
	const std::string& module) -> bool
{
	this->ipc = OpenFileMappingA(
		FILE_MAP_ALL_ACCESS,
		false,
		module.c_str()
	);

	return this->ipc != nullptr;
}

auto ipc::c_ipc::initialize(
	const std::string& module
) -> bool
{

	const auto pid = m_util.get()->get_process_id(OBFUSCATE_STR(L"embark-uwp.exe"));
	if (!pid) {
		LOG(OBFUSCATE_STR("invalid widget.\n"));
		return false;
	}

	const auto gamebar_sid = m_util.get()->get_gamebar_sid(pid);

	const std::string sid(
		gamebar_sid.begin(),
		gamebar_sid.end()
	);

	const std::string buffer = OBFUSCATE_STR("AppContainerNamedObjects\\") + sid + OBFUSCATE_STR("\\") + module;
	const std::string mtx_path = OBFUSCATE_STR("AppContainerNamedObjects\\") + sid + OBFUSCATE_STR("\\") + OBFUSCATE_STR("lumigen_mutex");

	auto status = this->create_mutex(mtx_path);
	if (!status)
	{
		LOG(OBFUSCATE_STR("failed to create mutex.\n"));
		return false;
	}

	status = this->connect_buffer(buffer.c_str());
	if (!status)
	{
		LOG(OBFUSCATE_STR("failed to open file.\n"));
		return false;
	}

	status = this->map_buffer();
	if (!status)
	{
		LOG(OBFUSCATE_STR("failed to map file.\n"));
		return false;
	}

	return true;
}