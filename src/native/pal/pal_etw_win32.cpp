// pal_etw_win32.cpp — Win32 ETW implementation

#include <chaos/pal/pal_etw.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601  // Windows 7+
#endif
#include <windows.h>
#include <evntprov.h>

#include <cstring>
#include <new>

namespace chaos::il2cpp::pal {

struct PalEtwProvider {
    REGHANDLE handle;
    GUID      guid;
};

PalEtwProvider* PalEtwRegister(const uint8_t provider_id[16]) noexcept {
    // Convert 16-byte binary GUID to Windows GUID struct.
    GUID guid;
    guid.Data1 = static_cast<ULONG>(
        (static_cast<uint32_t>(provider_id[0]) << 24) |
        (static_cast<uint32_t>(provider_id[1]) << 16) |
        (static_cast<uint32_t>(provider_id[2]) << 8)  |
        static_cast<uint32_t>(provider_id[3]));
    guid.Data2 = static_cast<USHORT>(
        (static_cast<uint16_t>(provider_id[4]) << 8) |
        static_cast<uint16_t>(provider_id[5]));
    guid.Data3 = static_cast<USHORT>(
        (static_cast<uint16_t>(provider_id[6]) << 8) |
        static_cast<uint16_t>(provider_id[7]));
    std::memcpy(guid.Data4, provider_id + 8, 8);

    REGHANDLE reg = 0;
    ULONG status = ::EventRegister(&guid, nullptr, nullptr, &reg);
    if (status != ERROR_SUCCESS) return nullptr;

    auto* p = new PalEtwProvider{reg, guid};
    return p;
}

void PalEtwUnregister(PalEtwProvider* provider) noexcept {
    if (!provider) return;
    ::EventUnregister(provider->handle);
    delete provider;
}

bool PalEtwIsEnabled(PalEtwProvider* provider) noexcept {
    return provider != nullptr && provider->handle != 0;
}

bool PalEtwWrite(PalEtwProvider* provider,
                 uint8_t event_id, uint8_t version, uint8_t channel,
                 uint8_t level, uint64_t keyword,
                 const void* data, uint32_t data_len) noexcept
{
    if (!provider) return false;

    EVENT_DESCRIPTOR desc;
    desc.Id = event_id;
    desc.Version = version;
    desc.Channel = channel;
    desc.Level = level;
    desc.Opcode = 0;
    desc.Task = 0;
    desc.Keyword = keyword;

    EVENT_DATA_DESCRIPTOR data_desc;
    ::EventDataDescCreate(&data_desc, data, data_len);

    ULONG status = ::EventWrite(provider->handle, &desc, 1, &data_desc);
    return status == ERROR_SUCCESS;
}

}  // namespace chaos::il2cpp::pal
