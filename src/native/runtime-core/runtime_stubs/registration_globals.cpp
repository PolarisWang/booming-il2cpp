// registration_globals.cpp — Global function pointers set by generated code static init
//
// These extern "C" global function pointers are set by generated code via
// static initializers. g_chaos_populate_generic_registration is called at
// boot to populate generic method registrations. g_chaos_register_hotpatch_modules
// registers per-module hotpatch data for hot-patch dispatch.

extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;
extern "C" void (*g_chaos_register_hotpatch_modules)(void) = nullptr;