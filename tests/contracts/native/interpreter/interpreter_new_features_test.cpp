/// Minimal test: only the new InterpreterVM feature tests
#include "interpreter_vm.h"
#include "vtable_registry.h"

#include <chaos/type_info.h>

#include <iostream>
#include <cstring>
#include <cstdint>

using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterVM;
using chaos::il2cpp::interpreter::SEHClause;
using chaos::il2cpp::interpreter::SEHFlags;
using chaos::il2cpp::interpreter::ValueTag;
using chaos::il2cpp::interpreter::InterpreterObject;
using chaos::il2cpp::interpreter::InterpreterValue;
using namespace chaos::il2cpp::vtable_registry;

static int failures = 0;

#define TEST(name)                                                      \
    do {                                                                \
        std::cout << "  " << #name << "..." << std::flush;             \
        if (!(name())) {                                                \
            std::cerr << " FAIL" << std::endl;                          \
            ++failures;                                                 \
        } else {                                                        \
            std::cout << " ok" << std::endl;                            \
        }                                                               \
    } while (false)

static bool TestFloatBlt();
static bool TestFloatBgt();
static bool TestFloatBle();
static bool TestFloatBge();
static bool TestLdArgA_RefSemantics();
static bool TestLdLocA_RefSemantics();
static bool TestInterfaceCastClass();
static bool TestInterfaceIsInst();
static bool TestInterfaceVtableDispatch();

int main()
{
    std::cout << "Running new InterpreterVM feature tests..." << std::endl;

    TEST(TestFloatBlt);
    TEST(TestFloatBgt);
    TEST(TestFloatBle);
    TEST(TestFloatBge);
    TEST(TestLdArgA_RefSemantics);
    TEST(TestLdLocA_RefSemantics);
    TEST(TestInterfaceCastClass);
    TEST(TestInterfaceIsInst);
    TEST(TestInterfaceVtableDispatch);

    std::cout << "new-feature-tests=failures=" << failures << std::endl;
    return (failures > 0) ? 1 : 0;
}

// ── Float branch tests ─────────────────────────────────────────────────

bool TestFloatBlt()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 2.5; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 3.5; method.instructions.push_back(r2);
    IRInstruction blt; blt.op_code = IROpCode::Blt; blt.branch_target = 4; method.instructions.push_back(blt);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestFloatBgt()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 5.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 2.0; method.instructions.push_back(r2);
    IRInstruction bgt; bgt.op_code = IROpCode::Bgt; bgt.branch_target = 4; method.instructions.push_back(bgt);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestFloatBle()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 3.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 3.0; method.instructions.push_back(r2);
    IRInstruction ble; ble.op_code = IROpCode::Ble; ble.branch_target = 4; method.instructions.push_back(ble);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestFloatBge()
{
    IRMethod method;
    IRInstruction r1; r1.op_code = IROpCode::LdcR8; r1.immediate_r8 = 1.0; method.instructions.push_back(r1);
    IRInstruction r2; r2.op_code = IROpCode::LdcR8; r2.immediate_r8 = 0.5; method.instructions.push_back(r2);
    IRInstruction bge; bge.op_code = IROpCode::Bge; bge.branch_target = 4; method.instructions.push_back(bge);
    IRInstruction f; f.op_code = IROpCode::LdcI4; f.immediate_i4 = 0; method.instructions.push_back(f);
    IRInstruction t; t.op_code = IROpCode::LdcI4; t.immediate_i4 = 1; method.instructions.push_back(t);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 1;
}

// ── LdArgA/LdLocA tests ───────────────────────────────────────────────

bool TestLdArgA_RefSemantics()
{
    IRMethod method;
    IRInstruction ldarga; ldarga.op_code = IROpCode::LdArgA; ldarga.operand_index = 0; method.instructions.push_back(ldarga);
    IRInstruction ldobj; ldobj.op_code = IROpCode::LdObj; method.instructions.push_back(ldobj);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(42));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestLdLocA_RefSemantics()
{
    IRMethod method;
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 10; method.instructions.push_back(push);
    IRInstruction stloc; stloc.op_code = IROpCode::StLoc; stloc.operand_index = 0; method.instructions.push_back(stloc);
    // StObj stack transition: ..., addr, val → ...  (push addr first, then val)
    IRInstruction ldloca; ldloca.op_code = IROpCode::LdLocA; ldloca.operand_index = 0; method.instructions.push_back(ldloca);
    IRInstruction push2; push2.op_code = IROpCode::LdcI4; push2.immediate_i4 = 20; method.instructions.push_back(push2);
    IRInstruction stobj; stobj.op_code = IROpCode::StObj; method.instructions.push_back(stobj);
    IRInstruction ldloc; ldloc.op_code = IROpCode::LdLoc; ldloc.operand_index = 0; method.instructions.push_back(ldloc);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 20;
}

// ── Interface CastClass/IsInst tests ──────────────────────────────────

bool TestInterfaceCastClass()
{
    const CHAOS_IL2CPP_UINT64 kIfaceStableId = 0xABCD1234ULL;
    CHAOS_IL2CPP_UINT64 obj_stable_id = 0xDEADBEEFULL;

    ChaosIl2cpp::Common::InterfaceMapEntry obj_iface_entries[] = {
        { kIfaceStableId, 0u, 0u }
    };
    VTableSlot dummy_slot = { 1u, nullptr };
    VTableSlot obj_slots[1] = { dummy_slot };
    TypeVTable obj_vtable = {};
    obj_vtable.type_token = 0x100u;
    obj_vtable.base_token = 0u;
    obj_vtable.slot_count = 1u;
    obj_vtable.slots = obj_slots;
    obj_vtable.stable_id = obj_stable_id;
    obj_vtable.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    obj_vtable.iface_map = obj_iface_entries;
    obj_vtable.iface_count = 1u;
    RegisterTypeVTable(&obj_vtable);

    VTableSlot iface_slots[1] = { dummy_slot };
    TypeVTable iface_vtable = {};
    iface_vtable.type_token = 0x200u;
    iface_vtable.base_token = 0u;
    iface_vtable.slot_count = 1u;
    iface_vtable.slots = iface_slots;
    iface_vtable.stable_id = kIfaceStableId;
    iface_vtable.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    iface_vtable.iface_map = nullptr;
    iface_vtable.iface_count = 0u;
    RegisterTypeVTable(&iface_vtable);

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction cast; cast.op_code = IROpCode::CastClass; cast.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x200u); method.instructions.push_back(cast);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.return_value.tag == ValueTag::ObjectRef;
}

bool TestInterfaceIsInst()
{
    VTableSlot dummy_slot2 = { 1u, nullptr };
    VTableSlot obj_slots2[1] = { dummy_slot2 };

    TypeVTable obj_vtable2 = {};
    obj_vtable2.type_token = 0x100u;
    obj_vtable2.base_token = 0u;
    obj_vtable2.slot_count = 1u;
    obj_vtable2.slots = obj_slots2;
    obj_vtable2.stable_id = 0xBEEFBEEFULL;
    obj_vtable2.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    obj_vtable2.iface_map = nullptr;
    obj_vtable2.iface_count = 0u;
    RegisterTypeVTable(&obj_vtable2);

    VTableSlot iface_slots2[1] = { dummy_slot2 };
    TypeVTable iface_vtable2 = {};
    iface_vtable2.type_token = 0x300u;
    iface_vtable2.base_token = 0u;
    iface_vtable2.slot_count = 1u;
    iface_vtable2.slots = iface_slots2;
    iface_vtable2.stable_id = 0xCAFE1234ULL;
    iface_vtable2.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    RegisterTypeVTable(&iface_vtable2);

    IRMethod method;
    IRInstruction ldarg; ldarg.op_code = IROpCode::LdArg; ldarg.operand_index = 0; method.instructions.push_back(ldarg);
    IRInstruction isinst; isinst.op_code = IROpCode::IsInst; isinst.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(0x300u); method.instructions.push_back(isinst);
    IRInstruction ret; ret.op_code = IROpCode::Ret; method.instructions.push_back(ret);

    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.return_value.tag == ValueTag::Null;
}

// ── Interface vtable dispatch test ────────────────────────────────────

bool TestInterfaceVtableDispatch()
{
    const CHAOS_IL2CPP_UINT64 kIfaceStable = 0xABCD0001ULL;

    ChaosIl2cpp::Common::InterfaceMapEntry impl_ifaces[] = {
        { kIfaceStable, 2u, 1u }
    };
    void* impl_vtable_array[] = {
        nullptr, nullptr, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu))
    };

    VTableSlot slot0 = { 0x600u, nullptr };
    VTableSlot slot1 = { 0x601u, nullptr };
    VTableSlot slot2 = { 0x602u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu)) };
    VTableSlot base_slots_iface[3] = { slot0, slot1, slot2 };

    TypeVTable derived_vtable_iface = {};
    derived_vtable_iface.type_token = 0x500u;
    derived_vtable_iface.base_token = 0u;
    derived_vtable_iface.slot_count = 3u;
    derived_vtable_iface.slots = base_slots_iface;
    derived_vtable_iface.stable_id = 0x50000001ULL;
    derived_vtable_iface.vtable_array = const_cast<const void**>(impl_vtable_array);
    derived_vtable_iface.vtable_length = 3u;
    derived_vtable_iface.type_shape = ChaosIl2cpp::Common::chaos_type_shape_reference;
    derived_vtable_iface.iface_map = impl_ifaces;
    derived_vtable_iface.iface_count = 1u;
    RegisterTypeVTable(&derived_vtable_iface);

    VTableSlot iface_slot = { 0x700u, nullptr };
    VTableSlot iface_slots3[1] = { iface_slot };
    TypeVTable iface_vtable3 = {};
    iface_vtable3.type_token = 0x400u;
    iface_vtable3.base_token = 0u;
    iface_vtable3.slot_count = 1u;
    iface_vtable3.slots = iface_slots3;
    iface_vtable3.stable_id = kIfaceStable;
    iface_vtable3.type_shape = ChaosIl2cpp::Common::chaos_type_shape_interface;
    RegisterTypeVTable(&iface_vtable3);

    IRMethod method;
    IRInstruction ldarg3; ldarg3.op_code = IROpCode::LdArg; ldarg3.operand_index = 0; method.instructions.push_back(ldarg3);
    IRInstruction callvirt3;
    callvirt3.op_code = IROpCode::CallVirt;
    callvirt3.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0u);  // zero-based slot index within interface
    callvirt3.arg_count = 1u;
    method.instructions.push_back(callvirt3);

    auto* storage_iface = new InterpreterObject();
    storage_iface->type_token = 0x500u;

    ExecutionFrame frame3;
    frame3.arguments.push_back(InterpreterValue::from_obj(storage_iface));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame3);
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBABEu));
}