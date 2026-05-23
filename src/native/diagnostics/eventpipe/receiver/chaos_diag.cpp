// chaos_diag.cpp — EventPipe diagnostic receiver CLI
//
// Connects to a running Chaos IL2CPP runtime process via named pipe and
// receives diagnostic events in real-time.  Outputs events as JSON Lines
// to stdout or a file.
//
// Usage:
//   chaos-diag --pid <pid> [--filter GC|TP|EXC] [--output <file>]
//
// Examples:
//   chaos-diag --pid 1234
//   chaos-diag --pid 1234 --filter GC --output gc_events.jsonl
//   chaos-diag --pid 1234 2>nul | python analyze.py

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <windows.h>

#include "ep_receiver.h"

// ── CLI state ───────────────────────────────────────────────────────────

// ── Helpers ─────────────────────────────────────────────────────────────

static void PrintUsage() {
    fprintf(stderr,
        "Usage: chaos-diag --pid <pid> [options]\n"
        "Options:\n"
        "  --pid <pid>       Target process ID (required)\n"
        "  --filter <type>   Event filter: GC, TP, EXC (default: all)\n"
        "  --output <file>   Write JSONL to file instead of stdout\n"
        "  --help            Show this help\n"
    );
}

static bool ParseArgs(int argc, char** argv, DiagConfig& config) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--pid") == 0 && i + 1 < argc) {
            char* end = nullptr;
            long val = strtol(argv[++i], &end, 10);
            if (end == argv[i] || val <= 0) {
                fprintf(stderr, "Error: Invalid PID '%s'\n", argv[i]);
                return false;
            }
            config.pid = static_cast<uint32_t>(val);
        } else if (strcmp(argv[i], "--filter") == 0 && i + 1 < argc) {
            const char* filter = argv[++i];
            config.filter_gc = (strcmp(filter, "GC") == 0 || strcmp(filter, "all") == 0);
            config.filter_tp = (strcmp(filter, "TP") == 0 || strcmp(filter, "all") == 0);
            config.filter_exc = (strcmp(filter, "EXC") == 0 || strcmp(filter, "all") == 0);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            config.output_path = argv[++i];
            config.output_to_file = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            PrintUsage();
            return false;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            PrintUsage();
            return false;
        }
    }

    if (config.pid == 0) {
        fprintf(stderr, "Error: --pid is required\n");
        PrintUsage();
        return false;
    }
    return true;
}

const char* EventTypeToString(EpEventType type) {
    switch (type) {
    case EpEventType::GcStart:          return "GcStart";
    case EpEventType::GcEnd:            return "GcEnd";
    case EpEventType::GcYoungStart:     return "GcYoungStart";
    case EpEventType::GcYoungEnd:       return "GcYoungEnd";
    case EpEventType::GcFullStart:      return "GcFullStart";
    case EpEventType::GcFullEnd:        return "GcFullEnd";
    case EpEventType::GcOom:            return "GcOom";
    case EpEventType::GcGen1Collect:    return "GcGen1Collect";
    case EpEventType::GcAllocationTick: return "GcAllocationTick";
    case EpEventType::TpWorkerCreate:   return "TpWorkerCreate";
    case EpEventType::TpWorkerDestroy:  return "TpWorkerDestroy";
    case EpEventType::TpWorkItemQueue:  return "TpWorkItemQueue";
    case EpEventType::TpWorkItemDequeue:return "TpWorkItemDequeue";
    case EpEventType::TpWorkerAdjust:   return "TpWorkerAdjust";
    case EpEventType::TpIoCompletion:   return "TpIoCompletion";
    case EpEventType::ExceptionThrow:   return "ExceptionThrow";
    case EpEventType::ExceptionRethrow: return "ExceptionRethrow";
    case EpEventType::ExceptionCatch:   return "ExceptionCatch";
    default:                            return "Unknown";
    }
}

bool IsEventTypeCategory(EpEventType type, const DiagConfig& config) {
    uint32_t v = static_cast<uint32_t>(type);
    if ((v & 0xFF00) == 0x0100) return config.filter_gc;
    if ((v & 0xFF00) == 0x0200) return config.filter_tp;
    if ((v & 0xFF00) == 0x0300) return config.filter_exc;
    return true;
}

void WriteJsonEvent(FILE* out, const EpEventHeader& header,
                            const uint8_t* payload, uint32_t payload_size)
{
    fprintf(out, "{");
    fprintf(out, "\"event\":\"%s\",", EventTypeToString(header.event_type));
    fprintf(out, "\"timestamp\":%llu,", (unsigned long long)header.timestamp);
    fprintf(out, "\"payload_size\":%u", header.payload_size);

    // Include first few payload bytes as hex for diagnostics.
    if (payload_size > 0) {
        fprintf(out, ",\"payload_hex\":\"");
        uint32_t show = payload_size > 16 ? 16 : payload_size;
        for (uint32_t i = 0; i < show; ++i) {
            fprintf(out, "%02x", payload[i]);
        }
        if (payload_size > 16) fprintf(out, "...");
        fprintf(out, "\"");
    }

    fprintf(out, "}\n");
    fflush(out);
}

// ── Main ────────────────────────────────────────────────────────────────

#ifndef CHAOS_DIAG_UNIT_TEST

int main(int argc, char** argv) {
    DiagConfig config;
    if (!ParseArgs(argc, argv, config)) {
        return 1;
    }

    // Build pipe name.
    wchar_t pipe_name[128];
    swprintf_s(pipe_name, L"\\\\.\\pipe\\chaos-il2cpp-diag-%u", config.pid);

    fprintf(stderr, "Connecting to %ls ...\n", pipe_name);

    // Open the named pipe (client side).
    HANDLE hPipe = CreateFileW(
        pipe_name,
        GENERIC_READ,
        0,                          // no sharing
        nullptr,                    // default security
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        fprintf(stderr, "Error: Cannot connect to pipe (error %lu). "
                        "Is the target process running with EventPipe enabled?\n", err);
        return 1;
    }

    fprintf(stderr, "Connected. Receiving events... (Ctrl+C to stop)\n");

    // Open output file if requested.
    FILE* out = stdout;
    if (config.output_to_file) {
        fopen_s(&out, config.output_path.c_str(), "w");
        if (!out) {
            fprintf(stderr, "Error: Cannot open output file %s\n", config.output_path.c_str());
            CloseHandle(hPipe);
            return 1;
        }
    }

    // Read loop.
    uint8_t read_buf[sizeof(EpEventHeader) + 1024 + sizeof(uint32_t)];
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    bool running = true;
    uint64_t event_count = 0;

    while (running) {
        ResetEvent(overlapped.hEvent);

        // Read header (24 bytes).
        DWORD bytes_read = 0;
        BOOL ok = ReadFile(hPipe, read_buf, sizeof(EpEventHeader), &bytes_read, &overlapped);
        if (!ok) {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                // Async read in progress.
                DWORD wait = WaitForSingleObject(overlapped.hEvent, 5000);
                if (wait == WAIT_TIMEOUT) {
                    continue;  // timeout, try again
                }
                GetOverlappedResult(hPipe, &overlapped, &bytes_read, FALSE);
            } else if (err == ERROR_BROKEN_PIPE || err == ERROR_PIPE_NOT_CONNECTED) {
                fprintf(stderr, "\nPipe closed (runtime exited or disconnected).\n");
                break;
            } else if (err == ERROR_HANDLE_EOF) {
                break;
            } else {
                continue;
            }
        }

        if (bytes_read != sizeof(EpEventHeader)) {
            break;  // incomplete header
        }

        // Parse header.
        EpEventHeader header;
        memcpy(&header, read_buf, sizeof(EpEventHeader));

        // Validate magic.
        if (header.magic != kEpMagic) {
            fprintf(stderr, "Bad magic: 0x%08x (expected 0x%08x)\n",
                    header.magic, kEpMagic);
            continue;
        }

        // Check filter.
        if (!IsEventTypeCategory(header.event_type, config)) {
            // Skip payload.
            if (header.payload_size > 0) {
                DWORD skip = 0;
                OVERLAPPED skip_ov = {};
                skip_ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
                ReadFile(hPipe, read_buf,
                         header.payload_size + sizeof(uint32_t), &skip, &skip_ov);
                WaitForSingleObject(skip_ov.hEvent, 1000);
                CloseHandle(skip_ov.hEvent);
            }
            continue;
        }

        // Read payload + checksum.
        uint32_t remaining = header.payload_size + sizeof(uint32_t);
        if (remaining > 0 && remaining <= sizeof(read_buf)) {
            DWORD payload_read = 0;
            OVERLAPPED payload_ov = {};
            payload_ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            ResetEvent(payload_ov.hEvent);
            ok = ReadFile(hPipe, read_buf, remaining, &payload_read, &payload_ov);
            if (!ok) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    WaitForSingleObject(payload_ov.hEvent, 1000);
                    GetOverlappedResult(hPipe, &payload_ov, &payload_read, FALSE);
                }
            }
            CloseHandle(payload_ov.hEvent);
        }

        // Write JSON line.
        WriteJsonEvent(out, header, read_buf, header.payload_size);
        event_count++;

        // Periodic status to stderr.
        if (event_count % 100 == 0) {
            fprintf(stderr, "\rReceived %llu events...", (unsigned long long)event_count);
        }
    }

    fprintf(stderr, "\nTotal events received: %llu\n", (unsigned long long)event_count);

    CloseHandle(overlapped.hEvent);
    CloseHandle(hPipe);
    if (config.output_to_file && out != stdout) {
        fclose(out);
    }

    return 0;
}

#endif  // CHAOS_DIAG_UNIT_TEST
