// wct_deadlock_spy.cpp — WCT wait-chain deadlock sampler for a hung native process.
//
// Uses Windows Wait Chain Traversal (WaitChain.dll) to non-invasively report, for
// every thread in a target process, what synchronization object it's blocked on
// and which thread holds it.  Does NOT suspend the target, so it doesn't perturb
// a timing-sensitive deadlock (the exact case cdb-attach fails on).
//
// Usage:  wct_deadlock_spy.exe <pid>
// Outputs per-thread:  TID <id>  block -> <object> [type] ; held-by? link
// Then a summary of the blocked set + any holder-thread links (AB-BA ring).
//
// This is a DIAGNOSTIC tool (task#16 S2-B mark-hang global-deadlock capture);
// not part of the runtime, no production impact.

#include <windows.h>
#include <wct.h>
#include <tlhelp32.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <mutex>

typedef HWCT  (WINAPI *OpenThreadWaitChainSessionFn)(DWORD, PVOID);
typedef VOID  (WINAPI *CloseThreadWaitChainSessionFn)(HWCT);
typedef BOOL  (WINAPI *GetThreadWaitChainFn)(HWCT, DWORD_PTR, DWORD, DWORD, PDWORD, PWAITCHAIN_NODE_INFO, LPBOOL);

static std::mutex g_print;

static const char* ObjectTypeName(WCT_OBJECT_TYPE t) {
    switch (t) {
        case WctCriticalSectionType: return "CriticalSection";
        case WctSendMessageType:     return "SendMessage";
        case WctMutexType:           return "Mutex";
        case WctAlpcType:            return "ALPC";
        case WctComType:             return "COM";
        case WctThreadWaitType:      return "ThreadWait";
        case WctProcessWaitType:     return "ProcessWait";
        case WctThreadType:          return "Thread";
        case WctComActivationType:   return "COMActivation";
        case WctUnknownType:         return "Unknown";
        case WctSocketIoType:        return "SocketIO";
        default: return "Other";
    }
}

static std::vector<DWORD> EnumerateThreadIds(DWORD pid) {
    std::vector<DWORD> tids;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
    if (snap == INVALID_HANDLE_VALUE) return tids;
    THREADENTRY32 te{ sizeof(te) };
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) tids.push_back(te.th32ThreadID);
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
    return tids;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: wct_deadlock_spy <pid>\n");
        return 1;
    }
    DWORD pid = (DWORD)std::strtoul(argv[1], nullptr, 10);

    // WCT (Wait Chain Traversal) functions are exported from Advapi32.dll
    // (WINADVAPI) — always loaded, no API-set name needed.
    HMODULE wct = LoadLibraryA("Advapi32.dll");
    if (!wct) {
        std::printf("ERROR: cannot load Advapi32.dll (WCT provider)\n");
        return 2;
    }
    auto fnOpen  = (OpenThreadWaitChainSessionFn)GetProcAddress(wct, "OpenThreadWaitChainSession");
    auto fnClose = (CloseThreadWaitChainSessionFn)GetProcAddress(wct, "CloseThreadWaitChainSession");
    auto fnGet   = (GetThreadWaitChainFn)GetProcAddress(wct, "GetThreadWaitChain");
    if (!fnOpen || !fnClose || !fnGet) {
        std::printf("ERROR: WCT entry points not found (Open=%p Close=%p Get=%p)\n",
                    fnOpen, fnClose, fnGet);
        return 3;
    }

    // Synchronous session (Flags=0).  Walk the TARGET process's threads with the
    // out-of-process flag so cross-process waits resolve.
    HWCT session = fnOpen(0 /*synchronous*/, nullptr);
    if (!session) {
        std::printf("ERROR: OpenThreadWaitChainSession failed\n");
        return 4;
    }

    auto tids = EnumerateThreadIds(pid);
    std::printf("target pid=%lu threads=%zu\n", pid, tids.size());

    // Node 0 of each thread's chain = the thread itself; node 1.. = what it waits on.
    for (DWORD tid : tids) {
        DWORD nodeCount = WCT_MAX_NODE_COUNT;   // 16
        WAITCHAIN_NODE_INFO nodes[WCT_MAX_NODE_COUNT]{};
        BOOL isCycle = FALSE;
        BOOL ok = fnGet(session, 0, WCT_OUT_OF_PROC_CS_FLAG, tid, &nodeCount, nodes, &isCycle);
        // DIAG: always print ok + nodeCount so we can distinguish "no tracked wait"
        // (ok=true, nodeCount==1) from a real query failure (ok=false) — and detect
        // WCT genuinely failing to resolve a CS AB-BA we know is blocked.
        std::printf("TID %lu: ok=%d nodeCount=%u", (unsigned long)tid, ok ? 1 : 0,
                    static_cast<unsigned>(nodeCount));
        if (!ok) { std::printf(" [QUERY_FAIL err=%lu]", (unsigned long)GetLastError()); }
        if (isCycle) std::printf(" [CYCLE]");
        std::printf("\n");
        if (!ok) continue;
        if (nodeCount <= 1) continue;  // thread unblocked, no wait

        std::lock_guard<std::mutex> lg(g_print);
        std::printf("TID %lu: ", (unsigned long)tid);
        for (DWORD i = 1; i < nodeCount; i++) {
            auto& n = nodes[i];
            if (n.ObjectType == WctThreadType) {
                std::printf("blocked on thread pid=%lu tid=%lu",
                            (unsigned long)n.ThreadObject.ProcessId,
                            (unsigned long)n.ThreadObject.ThreadId);
            } else if (n.ObjectType) {
                std::printf("blocked on %s '%.*ls'", ObjectTypeName(n.ObjectType),
                            WCT_OBJNAME_LENGTH, n.LockObject.ObjectName);
            }
            if (isCycle) std::printf(" [CYCLE]");
            std::printf("\n");
            break; // just show the immediate wait object
        }
    }

    fnClose(session);
    std::printf("-- wct_deadlock_spy done --\n");
    return 0;
}
