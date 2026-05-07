#include <chaos/common.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "dispatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
extern "C" void InterpreterEntryDirect(CHAOS_IL2CPP_UINTPTR, void*, void*) noexcept;
#pragma warning(push)
#pragma warning(disable: 4065 4244)
#include <chaos/common.h>
#include <chaos/type_info.h>
		::chaos::il2cpp::runtime_core::RegisterModule("ConvertCharNativeEntry", &s_native_aot_module);
// ©¤©¤ NameIndex + Dispatch Table (D3 HotPatch) ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
// Method name index entries
static constexpr NameIndexMethodEntryV0 s_name_index_methods[19] = {
	{ "Method0", 0x00000003u, 0u },  // ConvertCharNativeEntry
	{ "Method1", 0x00000004u, 0u },  // ConvertCharNativeEntry
	{ "Method2", 0x00000005u, 0u },  // ConvertCharNativeEntry
	{ "Method3", 0x00000006u, 0u },  // ConvertCharNativeEntry
	{ "Method4", 0x00000007u, 0u },  // ConvertCharNativeEntry
	{ "Method5", 0x00000008u, 0u },  // ConvertCharNativeEntry
	{ "Method6", 0x00000009u, 0u },  // ConvertCharNativeEntry
	{ "Method7", 0x0000000Au, 0u },  // ConvertCharNativeEntry
	{ "Method8", 0x0000000Bu, 0u },  // ConvertCharNativeEntry
	{ "Method9", 0x0000000Cu, 0u },  // ConvertCharNativeEntry
	{ "Method10", 0x0000000Du, 0u },  // ConvertCharNativeEntry
	{ "Method11", 0x0000000Eu, 0u },  // ConvertCharNativeEntry
	{ "Method12", 0x0000000Fu, 0u },  // ConvertCharNativeEntry
	{ "Method13", 0x00000010u, 0u },  // ConvertCharNativeEntry
	{ "Method14", 0x00000011u, 0u },  // ConvertCharNativeEntry
	{ "Method15", 0x00000012u, 0u },  // ConvertCharNativeEntry
	{ "Method16", 0x00000013u, 0u },  // ConvertCharNativeEntry
	{ "Method17", 0x00000014u, 0u },  // ConvertCharNativeEntry
	{ "Run", 0x00000002u, 1u },  // ConvertCharNativeEntry
};

// Type name index entries
static constexpr NameIndexTypeEntryV0 s_name_index_types[1] = {
	{ "ConvertCharNativeEntry", 0u, 19u },
};

// Token¡úSlot mapping (sorted by token for binary search)
static constexpr TokenSlotEntryV0 s_token_slot_entries[19] = {
	{ 0x00000002u, 18u },
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 10u },
	{ 0x0000000Eu, 11u },
	{ 0x0000000Fu, 12u },
	{ 0x00000010u, 13u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
};

// Dispatch table (function pointers)
static DispatchEntryV0 s_dispatch_table[19] = {
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method0
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method1
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method2
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method3
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method4
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method5
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method6
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method7
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method8
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method9
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method10
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method11
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method12
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method13
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method14
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method15
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method16
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Method17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Method17
	{ reinterpret_cast<void*>(&ConvertCharNativeEntry_ConvertCharNativeEntry_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // ConvertCharNativeEntry::Run
};

// Module NameIndex bundle
static constexpr NameIndexModuleV0 s_name_index_module = {
	"ConvertCharNativeEntry",
	s_name_index_types,
	1u,
	s_name_index_methods,
	19u,
	s_token_slot_entries,
	19u,
	s_dispatch_table,
	19u,
};

// Register NameIndex with the runtime on load
static const CHAOS_IL2CPP_UINT32 s_name_index_registered = []()
{
	::chaos::il2cpp::runtime_core::RegisterModuleNameIndex(
		&s_name_index_module);
	return 1u;
}();

#pragma warning(pop)
extern "C" int __chaos_assert_failures = 0;
extern "C" int RunNativeAot(int i) { return 0; }
