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

extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_0_op_Inequality_30F8EE7E(
    void
) {

}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_1_op_Inequality_BD49734C(
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


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_2_Equals_8D849DAE(
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


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_3_ToString_3BF1E8B8(
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
    { extern "C" void chaos_bridge_3_ToString_3BF1E8B8(); chaos_bridge_3_ToString_3BF1E8B8(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_1);
}


extern "C" void chaos_bridge_4__ctor_A26A867B(
    void
) {

    return;
}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_5_get_Error_890DBBCF(
    void
) {

}


extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_6_Concat_771C55C7(
    void
) {

    if (chaos_arg_0 != 0) goto L6;
    goto L12;

L6:
    ;
    { extern "C" void chaos_bridge_3_ToString_3BF1E8B8(); chaos_bridge_3_ToString_3BF1E8B8(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;

L12:
    ;
    if (chaos_arg_1 != 0) goto L18;
    goto L24;

L18:
    ;
    { extern "C" void chaos_bridge_3_ToString_3BF1E8B8(); chaos_bridge_3_ToString_3BF1E8B8(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_1 = 0;

L24:
    ;
    { extern "C" void chaos_bridge_6_Concat_771C55C7(); chaos_bridge_6_Concat_771C55C7(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_2 = 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_tmp_2);
}


extern "C" void chaos_bridge_7_WriteLine_9595AEB1(
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


extern "C" void chaos_bridge_8__ctor_49A51EA1(
    void
) {

    { extern "C" void chaos_bridge_4__ctor_A26A867B(); chaos_bridge_4__ctor_A26A867B(); }
    CHAOS_IL2CPP_INTPTR chaos_tmp_0 = 0;
    return;
}


extern "C" void* ResolveBridge(const char* subjectId) noexcept {
    if (subjectId == nullptr) return nullptr;
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_0_op_Inequality_30F8EE7E);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_1_op_Inequality_BD49734C);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_2_Equals_8D849DAE);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::ToString:System.String()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_3_ToString_3BF1E8B8);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Object::.ctor:System.Void()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_4__ctor_A26A867B);
    if (std::strcmp(subjectId, "System.Console/System.Console::get_Error:System.IO.TextWriter()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_5_get_Error_890DBBCF);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_6_Concat_771C55C7);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_7_WriteLine_9595AEB1);
    if (std::strcmp(subjectId, "System.Private.CoreLib/System.Attribute::.ctor:System.Void()") == 0)
        return reinterpret_cast<void*>(&chaos_bridge_8__ctor_49A51EA1);
    return nullptr;
}
}
