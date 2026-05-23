/// bootstrap_icall.cpp — Icall resolution (ResolveIcall).
///
/// Extracted from bootstrap.cpp during industrialization (Phase D).
/// Uses std::strstr prefix matching against ~30 known icall signatures.

#include "bootstrap_internal.h"

#include "runtime_core.h"
#include "support.h"

#include <cstring>

namespace chaos::il2cpp::bootstrap {

// ── Known icall signatures (file-scope constants) ──────────────────────

namespace {

constexpr const char* kConsoleWriteLineStringIcallSignature =
    "System.Console::WriteLine(System.String)";
constexpr const char* kStringConcatPairIcallSignature =
    "System.String::Concat(System.String,System.String)";
constexpr const char* kStringConcatTripleIcallSignature =
    "System.String::Concat(System.String,System.String,System.String)";
constexpr const char* kDelegateCombineIcallSignature =
    "System.Delegate::Combine(System.Delegate,System.Delegate)";
constexpr const char* kDelegateRemoveIcallSignature =
    "System.Delegate::Remove(System.Delegate,System.Delegate)";
constexpr const char* kGetLastPInvokeErrorIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetLastPInvokeError()";
constexpr const char* kGetLastWin32ErrorIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetLastWin32Error()";
constexpr const char* kStringToBSTRIcallSignature =
    "System.Runtime.InteropServices.Marshal::StringToBSTR(";
constexpr const char* kPtrToStringBSTRICallSignature =
    "System.Runtime.InteropServices.Marshal::PtrToStringBSTR(";
constexpr const char* kFreeBSTRICallSignature =
    "System.Runtime.InteropServices.Marshal::FreeBSTR(";
constexpr const char* kGetObjectForNativeVariantIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant(";
constexpr const char* kGetNativeVariantForObjectIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetNativeVariantForObject(";
constexpr const char* kStringToHGlobalAnsiIcallSignature =
    "System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi(";
constexpr const char* kStringToHGlobalUniIcallSignature =
    "System.Runtime.InteropServices.Marshal::StringToHGlobalUni(";
constexpr const char* kPtrToStringAnsiIcallSignature =
    "System.Runtime.InteropServices.Marshal::PtrToStringAnsi(";
constexpr const char* kUnsafeAddrOfPinnedArrayElementIcallSignature =
    "System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement(";
constexpr const char* kOffsetOfByTypeIcallSignature =
    "System.Runtime.InteropServices.Marshal::OffsetOf(";
constexpr const char* kThrowExceptionForHRIcallSignature =
    "System.Runtime.InteropServices.Marshal::ThrowExceptionForHR(";
constexpr const char* kGetExceptionForHRIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetExceptionForHR(";
constexpr const char* kGetHRForExceptionIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetHRForException(";
constexpr const char* kCoInitializeIcallSignature =
    "System.Runtime.InteropServices.Marshal::CoInitializeEx(";
constexpr const char* kCoUninitializeIcallSignature =
    "System.Runtime.InteropServices.Marshal::CoUninitialize()";
constexpr const char* kCoCreateInstanceIcallSignature =
    "System.Runtime.InteropServices.Marshal::CoCreateInstance(";
constexpr const char* kCoCreateInstanceAggregatedIcallSignature =
    "System.Runtime.InteropServices.Marshal::CoCreateInstanceAggregated(";
constexpr const char* kCreateRcwIcallSignature =
    "System.Runtime.InteropServices.Marshal::CreateRcw(";
constexpr const char* kReleaseRcwIcallSignature =
    "System.Runtime.InteropServices.Marshal::ReleaseRcw(";
constexpr const char* kGetRcwUnknownIcallSignature =
    "System.Runtime.InteropServices.Marshal::GetRcwUnknown(";
constexpr const char* kRcwQueryInterfaceIcallSignature =
    "System.Runtime.InteropServices.Marshal::RcwQueryInterface(";
constexpr const char* kCreateCcwIcallSignature =
    "System.Runtime.InteropServices.Marshal::CreateCcw(";
constexpr const char* kCreateCcwAggregatedIcallSignature =
    "System.Runtime.InteropServices.Marshal::CreateCcwAggregated(";
constexpr const char* kCustomMarshalerNativeToManagedIcallSignature =
    "System.Runtime.InteropServices.Marshal::CustomMarshalerNativeToManaged(";
constexpr const char* kCustomMarshalerManagedToNativeIcallSignature =
    "System.Runtime.InteropServices.Marshal::CustomMarshalerManagedToNative(";

}  // namespace

// ── ResolveIcall ───────────────────────────────────────────────────────

void* CHAOS_RUNTIME_ABI_CALL ResolveIcall(const char* icall_name_utf8) {
    const auto* state = PeekBootstrapState();
    if (state == nullptr || !state->is_bootstrapped || icall_name_utf8 == nullptr) {
        return nullptr;
    }

    if (std::strstr(icall_name_utf8, kConsoleWriteLineStringIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::WriteLineString);
    }

    if (std::strstr(icall_name_utf8, kStringConcatPairIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringPair);
    }

    if (std::strstr(icall_name_utf8, kStringConcatTripleIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringTriple);
    }

    if (std::strstr(icall_name_utf8, kDelegateCombineIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&CombineDelegate);
    }

    if (std::strstr(icall_name_utf8, kDelegateRemoveIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&RemoveDelegate);
    }

    if (std::strstr(icall_name_utf8, kGetLastPInvokeErrorIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::GetLastPInvokeErrorIcall);
    }

    if (std::strstr(icall_name_utf8, kGetLastWin32ErrorIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::GetLastPInvokeErrorIcall);
    }

    if (std::strstr(icall_name_utf8, kStringToBSTRIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalStringToBSTR);
    }

    if (std::strstr(icall_name_utf8, kPtrToStringBSTRICallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalPtrToStringBSTR);
    }

    if (std::strstr(icall_name_utf8, kFreeBSTRICallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalFreeBSTR);
    }

    if (std::strstr(icall_name_utf8, kGetObjectForNativeVariantIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::ChaosGetObjectForNativeVariant);
    }

    if (std::strstr(icall_name_utf8, kGetNativeVariantForObjectIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::ChaosGetNativeVariantForObject);
    }

    if (std::strstr(icall_name_utf8, kStringToHGlobalAnsiIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalStringToHGlobalAnsi);
    }

    if (std::strstr(icall_name_utf8, kStringToHGlobalUniIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalStringToHGlobalUni);
    }

    if (std::strstr(icall_name_utf8, kPtrToStringAnsiIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalPtrToStringAnsiIcall);
    }

    if (std::strstr(icall_name_utf8, kUnsafeAddrOfPinnedArrayElementIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalUnsafeAddrOfPinnedArrayElement);
    }

    if (std::strstr(icall_name_utf8, kOffsetOfByTypeIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalOffsetOfByType);
    }

    // ── HRESULT exception helpers ─────────────────────────────────────
    if (std::strstr(icall_name_utf8, kThrowExceptionForHRIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalThrowExceptionForHR);
    }
    if (std::strstr(icall_name_utf8, kGetExceptionForHRIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalGetExceptionForHR);
    }
    if (std::strstr(icall_name_utf8, kGetHRForExceptionIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalGetHRForException);
    }

    // ── COM interop icalls ──────────────────────────────────────────
    if (std::strstr(icall_name_utf8, kCoInitializeIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CoInitializeApartment);
    }

    if (std::strstr(icall_name_utf8, kCoUninitializeIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CoUninitializeApartment);
    }

    if (std::strstr(icall_name_utf8, kCoCreateInstanceAggregatedIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CoCreateComInstanceAggregated);
    }
    if (std::strstr(icall_name_utf8, kCoCreateInstanceIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CoCreateComInstance);
    }

    // ── RCW icalls ──────────────────────────────────────────────────
    if (std::strstr(icall_name_utf8, kCreateRcwIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalCreateRcw);
    }

    if (std::strstr(icall_name_utf8, kReleaseRcwIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalReleaseRcw);
    }

    if (std::strstr(icall_name_utf8, kGetRcwUnknownIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalGetRcwUnknown);
    }

    if (std::strstr(icall_name_utf8, kRcwQueryInterfaceIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalRcwQueryInterface);
    }

    // ── CCW icalls ───────────────────────────────────────────────────
    if (std::strstr(icall_name_utf8, kCreateCcwAggregatedIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalCreateCcwAggregated);
    }
    if (std::strstr(icall_name_utf8, kCreateCcwIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::MarshalCreateCcw);
    }

    // ── ICustomMarshaler icalls ──────────────────────────────────────
    if (std::strstr(icall_name_utf8, kCustomMarshalerNativeToManagedIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CustomMarshalerNativeToManaged);
    }

    if (std::strstr(icall_name_utf8, kCustomMarshalerManagedToNativeIcallSignature) != nullptr) {
        return reinterpret_cast<void*>(&chaos::il2cpp::runtime_core::CustomMarshalerManagedToNative);
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::bootstrap
