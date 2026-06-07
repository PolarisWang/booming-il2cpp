// Auto-generated bridge redirect table — Phase 1c
#include <cstring>
#include <cstdint>
#include <chaos/chaos.h>
#include <chaos/string_table.h>
#include <gc/gc_layout.h>

extern "C" int32_t kChaosExternalRuntimeCount;
extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" void* ResolveBridge(const char* subjectId) noexcept;

extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_0_op_Inequality_D223B44D(
    void
) {

}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_1_op_Inequality_D6506C9F(
    void
) {

    {
        static int _idx = -2;
        if (_idx == -2) {
            _idx = -1;
            for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)
                if (std::strcmp(kChaosExternalRuntimeSubjects[_i], "System.Private.CoreLib/System.String::Equals:System.Boolean(System.String,System.String)") == 0)
                { _idx = _i; break; }
        }
        if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {
            auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);
            _fn(0);
        }
    }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_tmp_0) == static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(6))) ? 1 : 0);
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_1);
}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_2_Equals_E1E37C95(
    void
) {

    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(7)));
    if (chaos_arg_0 == 0) goto L12;
    if (chaos_arg_1 != 0) goto L14;

L12:
    ;
    return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(6)));

L14:
    ;
    {
        static int _idx = -2;
        if (_idx == -2) {
            _idx = -1;
            for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)
                if (std::strcmp(kChaosExternalRuntimeSubjects[_i], "System.Private.CoreLib/System.Object::Equals:System.Boolean(Object)") == 0)
                { _idx = _i; break; }
        }
        if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {
            auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);
            _fn(0);
        }
    }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_0);
}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_3_ToString_65429BDC(
    void
) {

    {
        static int _idx = -2;
        if (_idx == -2) {
            _idx = -1;
            for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)
                if (std::strcmp(kChaosExternalRuntimeSubjects[_i], "System.Private.CoreLib/System.Object::GetType:System.Type()") == 0)
                { _idx = _i; break; }
        }
        if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {
            auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);
            _fn(0);
        }
    }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    { extern "C" void chaos_bridge_3_ToString_65429BDC(); chaos_bridge_3_ToString_65429BDC(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_1);
}


extern "C" void chaos_bridge_4__ctor_B6967881(
    void
) {

    return;
}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_5_get_Error_A7C5B212(
    void
) {

}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_6_Concat_F0510932(
    void
) {

    if (chaos_arg_0 != 0) goto L6;
    goto L12;

L6:
    ;
    { extern "C" void chaos_bridge_3_ToString_65429BDC(); chaos_bridge_3_ToString_65429BDC(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;

L12:
    ;
    if (chaos_arg_1 != 0) goto L18;
    goto L24;

L18:
    ;
    { extern "C" void chaos_bridge_3_ToString_65429BDC(); chaos_bridge_3_ToString_65429BDC(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = 0;

L24:
    ;
    { extern "C" void chaos_bridge_6_Concat_F0510932(); chaos_bridge_6_Concat_F0510932(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_2 = 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_2);
}


extern "C" void chaos_bridge_7_WriteLine_42C15549(
    void
) {

    {
        static int _idx = -2;
        if (_idx == -2) {
            _idx = -1;
            for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)
                if (std::strcmp(kChaosExternalRuntimeSubjects[_i], "System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(Char)") == 0)
                { _idx = _i; break; }
        }
        if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {
            auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);
            _fn(0);
        }
    }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    {
        static int _idx = -2;
        if (_idx == -2) {
            _idx = -1;
            for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)
                if (std::strcmp(kChaosExternalRuntimeSubjects[_i], "System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()") == 0)
                { _idx = _i; break; }
        }
        if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {
            auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);
            _fn(0);
        }
    }
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = 0;
    return;
}


extern "C" void chaos_bridge_8__ctor_9B79A39B(
    void
) {

    { extern "C" void chaos_bridge_4__ctor_B6967881(); chaos_bridge_4__ctor_B6967881(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    return;
}


extern "C" void* ResolveBridge(const char* subjectId) noexcept {
    if (subjectId == nullptr) return nullptr;
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_0_op_Inequality_D223B44D);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_1_op_Inequality_D6506C9F);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_2_Equals_E1E37C95);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::ToString:System.String()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_3_ToString_65429BDC);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::.ctor:System.Void()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_4__ctor_B6967881);
    if (std::strcmp(subjectId, "System.Console/System.Console::get_Error:System.IO.TextWriter()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_5_get_Error_A7C5B212);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_6_Concat_F0510932);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_7_WriteLine_42C15549);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Attribute::.ctor:System.Void()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_8__ctor_9B79A39B);
    return nullptr;
}
}
