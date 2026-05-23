#include <cstdlib>
#include <cstring>
#include <new>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include "com_typelib.h"
#include "com_ccw.h"
#include "memory_domain.h"

namespace chaos::il2cpp::runtime_core {
namespace {

// ── ITypeInfo method implementations ──

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_QueryInterface(void* self, const void* iid, void** ppv) noexcept {
    if (ppv == nullptr) return static_cast<int32_t>(0x80004003u); // E_POINTER
    *ppv = nullptr;
    // IUnknown and ITypeInfo IIDs both return self.
    *ppv = self;
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    impl->refcount++;
    return 0; // S_OK
}

uint32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_AddRef(void* self) noexcept {
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    return ++impl->refcount;
}

uint32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_Release(void* self) noexcept {
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    uint32_t remaining = --impl->refcount;
    if (remaining == 0) {
        memory_domain::DomainFreeTagged(impl);
    }
    return remaining;
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetTypeAttr(void* self, void** ppTypeAttr) noexcept {
    if (ppTypeAttr == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    *ppTypeAttr = &impl->type_attr;
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetTypeComp(void*, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetFuncDesc(void* self, uint32_t index, void** ppFuncDesc) noexcept {
    if (ppFuncDesc == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    if (index >= static_cast<uint32_t>(impl->data->method_count)) {
        return static_cast<int32_t>(0x80070057u); // E_INVALIDARG
    }
    // Allocate and populate a proper FUNCDESC from the static method data.
    auto* fd = static_cast<ComFuncDesc*>(
        memory_domain::DomainCurrentAllocateTagged(sizeof(ComFuncDesc)));
    if (fd == nullptr) return static_cast<int32_t>(0x8007000Eu); // E_OUTOFMEMORY

    const auto& md = impl->data->methods[index];
    std::memset(fd, 0, sizeof(*fd));
    fd->memid = md.disp_id;
    fd->cParams = md.c_params;
    fd->oVft = md.o_vft;
    fd->funckind = 0;  // FUNC_DISPATCH for IDispatch, FUNC_VIRTUAL for vtable
    fd->invkind = 1;   // INVOKE_FUNC = 1
    fd->callconv = 4;  // CC_STDCALL = 4
    *ppFuncDesc = fd;
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetVarDesc(void*, uint32_t, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL (no variable desc for interfaces)
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetNames(void* self, int32_t memid, void** rgszNames,
                                                  uint32_t cMaxNames, uint32_t* pcNames) noexcept {
    if (rgszNames == nullptr || pcNames == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeInfoImpl*>(self);
    *pcNames = 0;

    // Find the method with matching DISPID.
    for (int16_t i = 0; i < impl->data->method_count; ++i) {
        if (impl->data->methods[i].disp_id == memid) {
            // Return method name as BSTR-like pointer (simplified: just the char*).
            // The caller is expected to free via SysFreeString for real BSTRs.
            rgszNames[0] = const_cast<char*>(impl->data->methods[i].name);
            *pcNames = 1;

            // Return parameter names if there's room.
            uint32_t count = 1;
            auto* params = impl->data->methods[i].params;
            if (params != nullptr) {
                for (int16_t p = 0; p < impl->data->methods[i].c_params && count < cMaxNames; ++p, ++count) {
                    rgszNames[count] = const_cast<char*>(params[p].name);
                }
            }
            *pcNames = count;
            return 0; // S_OK
        }
    }
    return static_cast<int32_t>(0x80020003u); // DISP_E_MEMBERNOTFOUND
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetRefTypeOfImplType(void*, uint32_t, uint32_t*) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetImplTypeFlags(void*, uint32_t, int32_t*) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetIDsOfNames(void* self, void** rgszNames,
                                                       uint32_t cNames, int32_t* rgDispId) noexcept {
    if (rgszNames == nullptr || rgDispId == nullptr) return static_cast<int32_t>(0x80004003u);
    if (cNames == 0) return 0; // S_OK
    auto* impl = static_cast<ComTypeInfoImpl*>(self);

    // The first name is the method name; subsequent names are parameter names.
    const char* target = static_cast<const char*>(rgszNames[0]);
    for (int16_t i = 0; i < impl->data->method_count; ++i) {
        if (std::strcmp(impl->data->methods[i].name, target) == 0) {
            rgDispId[0] = impl->data->methods[i].disp_id;
            return 0; // S_OK
        }
    }
    return static_cast<int32_t>(0x80020003u); // DISP_E_MEMBERNOTFOUND
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_Invoke(void* self, void* pvInstance, int32_t memid,
                                                uint16_t wFlags, void* pDispParams,
                                                void* pVarResult, void* pExcepInfo,
                                                uint32_t* puArgErr) noexcept {
    (void)self;
    (void)pvInstance;
    (void)memid;
    (void)wFlags;
    (void)pDispParams;
    (void)pVarResult;
    (void)pExcepInfo;
    (void)puArgErr;
    // ITypeInfo::Invoke is not used in our dispatch model — we use the
    // codegen-generated Invoke thunk instead.  Return E_NOTIMPL.
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetDocumentation(void* self, int32_t memid, void** bstrName,
                                                          void** bstrDocString, uint32_t* pdwHelpContext,
                                                          void** bstrHelpFile) noexcept {
    if (bstrName == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeInfoImpl*>(self);

    // memid == -1 (MEMBERSID_NIL) returns type documentation.
    // Otherwise find the method with matching DISPID.
    if (memid == -1) {
        // Type-level documentation: return type name.
        *bstrName = const_cast<char*>(impl->data->type_name);
        if (bstrDocString != nullptr) *bstrDocString = nullptr;
        if (pdwHelpContext != nullptr) *pdwHelpContext = 0;
        if (bstrHelpFile != nullptr) *bstrHelpFile = nullptr;
        return 0; // S_OK
    }

    for (int16_t i = 0; i < impl->data->method_count; ++i) {
        if (impl->data->methods[i].disp_id == memid) {
            *bstrName = const_cast<char*>(impl->data->methods[i].name);
            if (bstrDocString != nullptr) *bstrDocString = nullptr;
            if (pdwHelpContext != nullptr) *pdwHelpContext = 0;
            if (bstrHelpFile != nullptr) *bstrHelpFile = nullptr;
            return 0; // S_OK
        }
    }
    return static_cast<int32_t>(0x80020003u); // DISP_E_MEMBERNOTFOUND
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetDllEntry(void*, int32_t, void**, void**, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetRefTypeInfo(void*, uint32_t, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_AddressOfMember(void*, int32_t, uint16_t, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_CreateInstance(void*, void*, const void*, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetMops(void*, int32_t, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeInfo_GetContainingTypeLib(void*, void**, uint32_t*) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

void CHAOS_RUNTIME_ABI_CALL TypeInfo_ReleaseTypeAttr(void*, void*) noexcept {
    // No-op: TYPEATTR is embedded in the impl.
}

void CHAOS_RUNTIME_ABI_CALL TypeInfo_ReleaseFuncDesc(void* self, void* pFuncDesc) noexcept {
    if (pFuncDesc != nullptr) {
        memory_domain::DomainFreeTagged(pFuncDesc);
    }
}

void CHAOS_RUNTIME_ABI_CALL TypeInfo_ReleaseVarDesc(void*, void*) noexcept {
    // No-op.
}

static ITypeInfoVtbl s_type_info_vtbl = {
    &TypeInfo_QueryInterface,
    &TypeInfo_AddRef,
    &TypeInfo_Release,
    &TypeInfo_GetTypeAttr,
    &TypeInfo_GetTypeComp,
    &TypeInfo_GetFuncDesc,
    &TypeInfo_GetVarDesc,
    &TypeInfo_GetNames,
    &TypeInfo_GetRefTypeOfImplType,
    &TypeInfo_GetImplTypeFlags,
    &TypeInfo_GetIDsOfNames,
    &TypeInfo_Invoke,
    &TypeInfo_GetDocumentation,
    &TypeInfo_GetDllEntry,
    &TypeInfo_GetRefTypeInfo,
    &TypeInfo_AddressOfMember,
    &TypeInfo_CreateInstance,
    &TypeInfo_GetMops,
    &TypeInfo_GetContainingTypeLib,
    &TypeInfo_ReleaseTypeAttr,
    &TypeInfo_ReleaseFuncDesc,
    &TypeInfo_ReleaseVarDesc,
};

// ── ITypeLib method implementations ──

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_QueryInterface(void* self, const void* iid, void** ppv) noexcept {
    if (ppv == nullptr) return static_cast<int32_t>(0x80004003u);
    *ppv = nullptr;
    *ppv = self;
    auto* impl = static_cast<ComTypeLibImpl*>(self);
    impl->refcount++;
    return 0; // S_OK
}

uint32_t CHAOS_RUNTIME_ABI_CALL TypeLib_AddRef(void* self) noexcept {
    auto* impl = static_cast<ComTypeLibImpl*>(self);
    return ++impl->refcount;
}

uint32_t CHAOS_RUNTIME_ABI_CALL TypeLib_Release(void* self) noexcept {
    auto* impl = static_cast<ComTypeLibImpl*>(self);
    uint32_t remaining = --impl->refcount;
    if (remaining == 0) {
        memory_domain::DomainFreeTagged(impl);
    }
    return remaining;
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetTypeInfoCount(void* self, uint32_t* pctInfo) noexcept {
    if (pctInfo == nullptr) return static_cast<int32_t>(0x80004003u);
    (void)self;
    *pctInfo = 1; // One type info per CCW.
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetTypeInfo(void* self, uint32_t index, void** ppTInfo) noexcept {
    if (ppTInfo == nullptr) return static_cast<int32_t>(0x80004003u);
    if (index != 0) return static_cast<int32_t>(0x80070057u); // E_INVALIDARG
    auto* impl = static_cast<ComTypeLibImpl*>(self);
    // For V2, we need to create a ComTypeInfoImpl from the same data.
    auto* type_info = static_cast<ComTypeInfoImpl*>(
        memory_domain::DomainCurrentAllocateTagged(sizeof(ComTypeInfoImpl)));
    if (type_info == nullptr) return static_cast<int32_t>(0x8007000Eu); // E_OUTOFMEMORY

    type_info->vtable = &s_type_info_vtbl;
    type_info->refcount = 1;
    type_info->data = impl->data;

    // Populate TYPEATTR from the static data.
    ComTypeAttr& attr = type_info->type_attr;
    std::memset(&attr, 0, sizeof(attr));
    if (impl->data->guid != nullptr) {
        std::memcpy(&attr.guid_lo, impl->data->guid, 8);
        std::memcpy(&attr.guid_hi, impl->data->guid + 8, 8);
    }
    attr.cFuncs = impl->data->method_count;
    attr.typekind = 5; // TKIND_INTERFACE
    attr.cbSizeVft = static_cast<int16_t>(3 + impl->data->method_count) * sizeof(void*);
    attr.wMajorVerNum = 1;
    attr.wMinorVerNum = 0;

    *ppTInfo = type_info;
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetTypeInfoType(void* self, uint32_t index, int32_t* pTKind) noexcept {
    if (pTKind == nullptr) return static_cast<int32_t>(0x80004003u);
    if (index != 0) return static_cast<int32_t>(0x80070057u);
    (void)self;
    *pTKind = 5; // TKIND_INTERFACE
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetTypeInfoOfGuid(void* self, const void* guid, void** ppTInfo) noexcept {
    if (ppTInfo == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeLibImpl*>(self);

    // Check if the GUID matches our interface.
    if (impl->data->guid != nullptr &&
        CHAOS_IL2CPP_MEMCMP(guid, impl->data->guid, 16) == 0) {
        return TypeLib_GetTypeInfo(self, 0, ppTInfo);
    }
    return static_cast<int32_t>(0x8002802Bu); // TYPE_E_ELEMENTNOTFOUND
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetLibAttr(void*, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetTypeComp(void*, void**) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_GetDocumentation(void* self, int32_t index, void** bstrName,
                                                         void** bstrDocString, uint32_t* pdwHelpContext,
                                                         void** bstrHelpFile) noexcept {
    if (bstrName == nullptr) return static_cast<int32_t>(0x80004003u);
    auto* impl = static_cast<ComTypeLibImpl*>(self);
    if (index == -1) {
        // Library documentation.
        *bstrName = const_cast<char*>(impl->data->type_namespace);
    } else if (index == 0) {
        // Type documentation.
        *bstrName = const_cast<char*>(impl->data->type_name);
    } else {
        return static_cast<int32_t>(0x80070057u); // E_INVALIDARG
    }
    if (bstrDocString != nullptr) *bstrDocString = nullptr;
    if (pdwHelpContext != nullptr) *pdwHelpContext = 0;
    if (bstrHelpFile != nullptr) *bstrHelpFile = nullptr;
    return 0; // S_OK
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_IsName(void*, void*, uint32_t, int32_t*) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

int32_t CHAOS_RUNTIME_ABI_CALL TypeLib_FindName(void*, void*, uint32_t, void**, uint32_t*, uint32_t*) noexcept {
    return static_cast<int32_t>(0x80004001u); // E_NOTIMPL
}

void CHAOS_RUNTIME_ABI_CALL TypeLib_ReleaseTLibAttr(void*, void*) noexcept {
    // No-op.
}

static ITypeLibVtbl s_type_lib_vtbl = {
    &TypeLib_QueryInterface,
    &TypeLib_AddRef,
    &TypeLib_Release,
    &TypeLib_GetTypeInfoCount,
    &TypeLib_GetTypeInfo,
    &TypeLib_GetTypeInfoType,
    &TypeLib_GetTypeInfoOfGuid,
    &TypeLib_GetLibAttr,
    &TypeLib_GetTypeComp,
    &TypeLib_GetDocumentation,
    &TypeLib_IsName,
    &TypeLib_FindName,
    &TypeLib_ReleaseTLibAttr,
};

}  // anonymous namespace

void* GetComTypeInfoForCcw(com_ccw::ComCcw* ccw) noexcept {
    if (ccw == nullptr || ccw->typelib_data == nullptr) return nullptr;

    auto* type_info = static_cast<ComTypeInfoImpl*>(
        memory_domain::DomainCurrentAllocateTagged(sizeof(ComTypeInfoImpl)));
    if (type_info == nullptr) return nullptr;

    type_info->vtable = &s_type_info_vtbl;
    type_info->refcount = 1;
    type_info->data = ccw->typelib_data;

    // Populate TYPEATTR from the static data.
    ComTypeAttr& attr = type_info->type_attr;
    std::memset(&attr, 0, sizeof(attr));
    if (ccw->typelib_data->guid != nullptr) {
        std::memcpy(&attr.guid_lo, ccw->typelib_data->guid, 8);
        std::memcpy(&attr.guid_hi, ccw->typelib_data->guid + 8, 8);
    }
    attr.cFuncs = ccw->typelib_data->method_count;
    attr.typekind = 5; // TKIND_INTERFACE
    attr.cbSizeVft = static_cast<int16_t>(3 + ccw->typelib_data->method_count) * sizeof(void*);
    attr.wMajorVerNum = 1;
    attr.wMinorVerNum = 0;

    return type_info;
}

void* GetComTypeLibForCcw(com_ccw::ComCcw* ccw) noexcept {
    if (ccw == nullptr || ccw->typelib_data == nullptr) return nullptr;

    auto* type_lib = static_cast<ComTypeLibImpl*>(
        memory_domain::DomainCurrentAllocateTagged(sizeof(ComTypeLibImpl)));
    if (type_lib == nullptr) return nullptr;

    type_lib->vtable = &s_type_lib_vtbl;
    type_lib->refcount = 1;
    type_lib->data = ccw->typelib_data;

    return type_lib;
}

}  // namespace chaos::il2cpp::runtime_core
