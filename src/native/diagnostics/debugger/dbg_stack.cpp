// dbg_stack.cpp — Call stack inspection
//
// Reads the frame snapshot captured by FastExecute at breakpoint time
// and returns DAP stack frames for the DAP stackTrace request.

#include "dbg_stack.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"
#include "dbg_runtime.h"

#include <cstdio>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::diagnostics {

void DbgStackInitialize() noexcept {
}

void DbgStackShutdown() noexcept {
}

std::vector<DapStackFrame> DbgGetStackTrace(int /*thread_id*/,
                                             int start_frame,
                                             int levels) noexcept
{
    std::vector<DapStackFrame> result;
    const auto& snap = DbgGetFrameSnapshot();

    int end = start_frame + levels;
    if (end > snap.frame_count) end = snap.frame_count;

    for (int i = start_frame; i < end; ++i) {
        DapStackFrame sf;
        sf.id = i - start_frame;

        // Format frame name from method token.
        char name_buf[64];
        std::snprintf(name_buf, sizeof(name_buf), "method_0x%08x+0x%x",
                      snap.frames[i].method_token,
                      snap.frames[i].il_offset);
        sf.name = name_buf;
        sf.source_path = "";
        sf.line = static_cast<int>(snap.frames[i].il_offset);
        sf.column = 0;

        result.push_back(sf);
    }

    return result;
}

int DbgGetFrameDepth() noexcept {
    return DbgGetFrameSnapshot().frame_count;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
