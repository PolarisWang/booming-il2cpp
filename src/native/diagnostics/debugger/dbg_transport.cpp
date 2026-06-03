// dbg_transport.cpp — Debugger transport implementation (stdio pipes)

#include "dbg_transport.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"

#include <cstdio>
#include <cstring>

#include <chaos/pal/pal_io.h>

#include <string>

namespace chaos::il2cpp::diagnostics {

namespace {

// Read buffer for incoming DAP messages (accumulates raw bytes).
constexpr int kReadBufSize = 65536;
char g_read_buf[kReadBufSize];
int g_read_pos = 0;

// Transport state.
bool g_initialized = false;

// On Windows, set stdin/stdout to binary mode to avoid CR/LF translation.
void SetBinaryMode() noexcept {
    chaos::il2cpp::pal::PalSetBinaryMode();
}

}  // anonymous namespace

bool DbgTransportInitialize() noexcept {
    if (g_initialized) return true;
    SetBinaryMode();
    g_initialized = true;
    return true;
}

void DbgTransportShutdown() noexcept {
    g_initialized = false;
}

bool DbgTransportReadMessage(char* buffer, int buffer_size) noexcept {
    if (!g_initialized) return false;

    while (true) {
        // Try to parse a message from the accumulated buffer.
        int content_length = DapParseHeader(g_read_buf, g_read_pos);
        if (content_length > 0 && content_length < buffer_size) {
            const char* json_start = DapFindJsonStart(g_read_buf, g_read_pos);
            if (json_start) {
                int header_end = static_cast<int>(json_start - g_read_buf);
                int json_avail = g_read_pos - header_end;
                if (json_avail >= content_length) {
                    // Complete message available.
                    memcpy(buffer, json_start, content_length);
                    buffer[content_length] = '\0';

                    // Remove consumed data from buffer.
                    int consumed = header_end + content_length;
                    g_read_pos -= consumed;
                    if (g_read_pos > 0) {
                        memmove(g_read_buf, g_read_buf + consumed, g_read_pos);
                    }
                    return true;
                }
                // Need more data — fall through to read more.
            }
        }

        // Read more data from stdin.
        int bytes_read = static_cast<int>(
            fread(g_read_buf + g_read_pos, 1, kReadBufSize - g_read_pos - 1, stdin));
        if (bytes_read <= 0) {
            if (feof(stdin)) return false;  // pipe closed
            if (ferror(stdin)) {
                clearerr(stdin);
                return false;  // error
            }
            continue;
        }
        g_read_pos += bytes_read;
        g_read_buf[g_read_pos] = '\0';
    }
}

bool DbgTransportWriteMessage(const char* json_body, int body_len) noexcept {
    if (!g_initialized) return false;
    std::string wire = DapEncodeMessage(json_body, body_len);
    size_t written = fwrite(wire.data(), 1, wire.size(), stdout);
    fflush(stdout);
    return written == wire.size();
}

bool DbgTransportIsConnected() noexcept {
    return g_initialized && !feof(stdin);
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
