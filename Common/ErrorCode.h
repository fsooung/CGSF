#pragma once


enum class ERROR_CODE : short
{
	SUCCESS		= 0,

	ENGINE_INIT_LOGIC_ENTRY_FAIL	= 101,
	ENGINE_INIT_NULL_CONFIG_INFO = 102,
	
	ENGINE_INIT_CREAT_ENGINE_LOAD_DLL_FAIL = 103,
	ENGINE_INIT_CREAT_ENGINE_FUNC_NULL = 104,
	ENGINE_INIT_CREAT_ENGINE_INIT_FAIL = 105,

	ENGINE_INIT_CREAT_LOGIC_SYSTEM_FAIL = 106,

	
};