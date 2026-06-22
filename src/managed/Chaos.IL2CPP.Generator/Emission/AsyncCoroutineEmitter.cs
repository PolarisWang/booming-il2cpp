// ── Async Coroutine Support (F1+F2+F3+F5) ──
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static bool IsAsyncStateMachineMoveNext(string? sid)
    {
        if (string.IsNullOrEmpty(sid)) return false;
        return sid.Contains(">d__") && (sid.Contains("::MoveNext") || sid.Contains("::MoveNext:"));
    }
    internal enum AsyncMethodKind { NotAsync, AsyncTask, AsyncTaskOfT, AsyncValueTask, AsyncValueTaskOfT, AsyncVoid, Complex }
    private AsyncMethodKind ClassifyAsyncMethod(AotCoreIrMethodArtifact m)
    {
        if (!IsAsyncStateMachineMoveNext(m.SubjectId)) return AsyncMethodKind.NotAsync;
        foreach (var i in m.Instructions)
        {
            var c = i.Callee; if (string.IsNullOrEmpty(c)) continue;
            if (c.Contains("AsyncTaskMethodBuilder")) return c.Contains("`1") ? AsyncMethodKind.AsyncTaskOfT : AsyncMethodKind.AsyncTask;
            if (c.Contains("AsyncValueTaskMethodBuilder")) return c.Contains("`1") ? AsyncMethodKind.AsyncValueTaskOfT : AsyncMethodKind.AsyncValueTask;
            if (c.Contains("AsyncVoidMethodBuilder")) return AsyncMethodKind.AsyncVoid;
        }
        return AsyncMethodKind.AsyncTask;
    }
    internal int AsyncMethodCount;
    internal int AsyncCoroutineMethodCount;
    internal int AsyncInterpreterFallbackCount;

    private StructuredIRNode? BuildAsyncStructuredBody(AotCoreIrMethodArtifact m)
    {
        if (!IsAsyncStateMachineMoveNext(m.SubjectId)) return null;
        if (ClassifyAsyncMethod(m) == AsyncMethodKind.Complex) return null;
        var instrs = m.Instructions;
        var pts = new List<(int S, int E, AsyncAwaiterKind K)>();
        for (int i = 0; i < instrs.Count; i++)
        {
            var c = instrs[i].Callee; if (string.IsNullOrEmpty(c) || !c.Contains("GetAwaiter")) continue;
            int end = i;
            for (int j = i + 1; j < instrs.Count; j++) { var c2 = instrs[j].Callee; if (!string.IsNullOrEmpty(c2) && c2.Contains("GetResult")) { end = j; break; } }
            if (end == i) end = instrs.Count - 1;
            var kind = AsyncAwaiterKind.TaskAwaiter;
            var map = AsyncAwaiterCatalog.Find(c);
            if (map != null) kind = map.CppWrapper switch { "TaskAwaiterWrapper" => AsyncAwaiterKind.TaskAwaiter, "ValueTaskAwaiterWrapper" => AsyncAwaiterKind.ValueTaskAwaiter, "YieldAwaiterWrapper" => AsyncAwaiterKind.YieldAwaitable, _ => AsyncAwaiterKind.TaskAwaiter };
            pts.Add((i, end, kind));
        }
        if (pts.Count == 0) return null;
        var nodes = new List<StructuredIRNode>(); int idx = 0;
        foreach (var (si, ei, k) in pts) { var pre = instrs.Skip(idx).Take(si - idx).ToList(); if (pre.Count > 0) nodes.Add(new IRBlock(pre, null)); nodes.Add(new IRAwait("(CHAOS_IL2CPP_INTPTR)0 /* " + instrs[si].Callee + " */", k, new IRSequence(new List<StructuredIRNode>()))); idx = ei + 1; }
        var post = instrs.Skip(idx).ToList(); if (post.Count > 0) nodes.Add(new IRBlock(post, null));
        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    internal static string GenPromise(string uid, bool hr)
    {
        var sb = new StringBuilder();
        // Forward-declare AsyncPromise so AsyncHandle can reference it,
        // then define AsyncHandle before AsyncPromise so get_return_object()
        // can use AsyncHandle without C2027 (incomplete type) / C2065 (undeclared).
        sb.Append("    struct AsyncPromise_" + uid + ";\n");
        sb.Append("    struct AsyncHandle_" + uid + " { using promise_type = AsyncPromise_" + uid + "; AsyncPromise_" + uid + "* p; void Start() noexcept { std::coroutine_handle<AsyncPromise_" + uid + ">::from_promise(*p).resume(); } };\n");
        sb.Append("    struct AsyncPromise_" + uid + " {\n");
        sb.Append("        CHAOS_IL2CPP_INTPTR _h[4]; CHAOS_IL2CPP_UINT32 _magic = 0x45524F43; CHAOS_IL2CPP_INTPTR _result = 0;\n");
        sb.Append("        auto get_return_object() noexcept { return AsyncHandle_" + uid + "{this}; }\n");
        sb.Append("        std::suspend_always initial_suspend() noexcept { return {}; }\n");
        sb.Append("        std::suspend_always final_suspend() noexcept { return {}; }\n");
        sb.Append(hr ? "        void return_value(CHAOS_IL2CPP_INTPTR v) noexcept { _result = v; }\n" : "        void return_void() noexcept {}\n");
        sb.Append("        void unhandled_exception() noexcept {}\n");
        sb.Append("        static void* operator new(std::size_t sz) noexcept { return CHAOS_IL2CPP_NEW_GC_PINNED(sz); }\n");
        sb.Append("        static void operator delete(void*, std::size_t) noexcept {}\n");
        sb.Append("    };\n");
        return sb.ToString();
    }

    internal string GenCoro(string uid, bool hr, string sid, StructuredIRNode body)
    {
        var sb = new StringBuilder();
        sb.Append("    // F3 coroutine: " + sid + "\n");
        sb.Append("    static AsyncHandle_" + uid + " " + uid + "_Coro() {\n");
        sb.Append("        try { co_await std::suspend_always{}; co_return; } catch (...) { }\n");
        sb.Append("    }\n");
        sb.Append("    extern \"C\" CHAOS_IL2CPP_INT64 Entry_" + uid + "() { auto h = " + uid + "_Coro(); h.Start(); return (CHAOS_IL2CPP_INT64)h.p; }\n");
        return sb.ToString();
    }

    private static string GetAsyncUid(AotCoreIrMethodArtifact m)
    {
        var s = m.SubjectId ?? m.NativeSymbol ?? "async";
        var sb = new StringBuilder();
        foreach (char c in s) { sb.Append(char.IsLetterOrDigit(c) || c == '_' ? c : '_'); }
        return sb.ToString();
    }
}
