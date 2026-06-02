// chaos_diag_posix.cpp — EventPipe diagnostic receiver CLI (POSIX)
//
// Connects to a running Chaos IL2CPP runtime process via Unix domain socket
// and receives diagnostic events in real-time.

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ep_receiver.h"

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

static bool ReadExact(int fd, uint8_t* buf, size_t size) noexcept {
    while (size > 0) {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;
        int ret = ::poll(&pfd, 1, 5000);
        if (ret <= 0) {
            if (ret == 0) continue;  // timeout, retry
            return false;
        }
        ssize_t n = ::read(fd, buf, size);
        if (n <= 0) return false;
        buf += n;
        size -= static_cast<size_t>(n);
    }
    return true;
}

int main(int argc, char** argv) {
    DiagConfig config;
    if (!ParseArgs(argc, argv, config)) {
        return 1;
    }

    char pipe_path[128];
    ::snprintf(pipe_path, sizeof(pipe_path),
               "/tmp/chaos-il2cpp-diag-%u.sock", config.pid);

    fprintf(stderr, "Connecting to %s ...\n", pipe_path);

    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Error: Cannot create socket.\n");
        return 1;
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, pipe_path, sizeof(addr.sun_path) - 1);

    if (::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        fprintf(stderr, "Error: Cannot connect to socket. "
                        "Is the target process running with EventPipe enabled?\n");
        ::close(fd);
        return 1;
    }

    fprintf(stderr, "Connected. Receiving events... (Ctrl+C to stop)\n");

    FILE* out = stdout;
    if (config.output_to_file) {
        out = fopen(config.output_path.c_str(), "w");
        if (!out) {
            fprintf(stderr, "Error: Cannot open output file %s\n", config.output_path.c_str());
            ::close(fd);
            return 1;
        }
    }

    uint8_t read_buf[sizeof(EpEventHeader) + 1024 + sizeof(uint32_t)];
    bool running = true;
    uint64_t event_count = 0;

    while (running) {
        if (!ReadExact(fd, read_buf, sizeof(EpEventHeader))) {
            fprintf(stderr, "\nSocket closed (runtime exited or disconnected).\n");
            break;
        }

        EpEventHeader header;
        std::memcpy(&header, read_buf, sizeof(EpEventHeader));

        if (header.magic != kEpMagic) {
            fprintf(stderr, "Bad magic: 0x%08x (expected 0x%08x)\n",
                    header.magic, kEpMagic);
            continue;
        }

        if (!IsEventTypeCategory(header.event_type, config)) {
            if (header.payload_size > 0) {
                ReadExact(fd, read_buf,
                          header.payload_size + sizeof(uint32_t));
            }
            continue;
        }

        uint32_t remaining = header.payload_size + sizeof(uint32_t);
        if (remaining > 0 && remaining <= sizeof(read_buf)) {
            if (!ReadExact(fd, read_buf, remaining)) {
                break;
            }
        }

        WriteJsonEvent(out, header, read_buf, header.payload_size);
        event_count++;

        if (event_count % 100 == 0) {
            fprintf(stderr, "\rReceived %llu events...", (unsigned long long)event_count);
        }
    }

    fprintf(stderr, "\nTotal events received: %llu\n", (unsigned long long)event_count);

    ::close(fd);
    if (config.output_to_file && out != stdout) {
        fclose(out);
    }

    return 0;
}
