// ── Async Awaiter Mapping Catalog (F4) ──
using System.Collections.Generic;

namespace Chaos.IL2CPP.Generator;

internal static class AsyncAwaiterCatalog
{
    public sealed record Mapping(string DotNetType, string CppWrapper, bool HasResult, bool AlwaysSuspend);
    public static readonly List<Mapping> All = new()
    {
        new("TaskAwaiter", "TaskAwaiterWrapper", false, false),
        new("TaskAwaiter`1", "TaskAwaiterOfTWrapper", true, false),
        new("ValueTaskAwaiter", "ValueTaskAwaiterWrapper", false, false),
        new("YieldAwaiter", "YieldAwaiterWrapper", false, true),
    };
    public static Mapping? Find(string type) { foreach (var m in All) if (type.Contains(m.DotNetType)) return m; return null; }
}
