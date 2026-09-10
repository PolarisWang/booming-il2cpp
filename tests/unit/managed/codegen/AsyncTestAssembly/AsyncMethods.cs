// A real async subject assembly whose compiled <OneNumber>d__0::MoveNext
// is the input to full-pipeline (Loader -> SemanticWorld -> Linker ->
// MetadataWriter -> CodeGenStage) extraction tests.
//
// The method bodies deliberately mirror the minimal async shape the Phase 2
// translator targets: async Task<int> awaiting Task.Yield() once, returning a
// constant. Compiling this file produces the compiler-generated state machine
// type whose MoveNext IL is exactly what AsyncCoroutineEmitter + MethodEmission
// must translate to a native hand-built state machine.

using System;
using System.Threading.Tasks;

namespace AsyncTestAssembly;

public static class AsyncMethods
{
    // async Task<int> awaiting Task.Yield once. Returns 1.
    // Roslyn generates <GetOne>d__0 (a value type) with a MoveNext member.
    public static async Task<int> GetOne()
    {
        await Task.Yield();
        return 1;
    }

    // async Task (non-generic) awaiting Task.Yield once.
    // Generates <DoVoid>d__0 (void-returning builder).
    public static async Task DoVoid()
    {
        await Task.Yield();
    }

    // async Task<int> awaiting a TaskCompletionSource<int>.Task — externally
    // completed.  This exercises TCS in the codegen pipeline: TCS object
    // creation (newobj), get_Task, await the Task handle — all surface in
    // the AOT IR and must be correctly lowered.
    public static async Task<int> AwaitTcs()
    {
        var tcs = new TaskCompletionSource<int>();
        // Return the task so the awaiter (the async state machine) registers
        // a continuation on the TCS's inner Task.
        return await tcs.Task;
    }

    // async Task awaiting Task.WhenAll(Task[]) — exercises the combinator
    // in the codegen pipeline.  WhenAll returns a Task that completes when
    // all argument tasks complete.  The state machine awaits that aggregate.
    public static async Task AwaitWhenAll()
    {
        var t1 = Task.Run(() => { });
        var t2 = Task.Delay(1);
        await Task.WhenAll(t1, t2);
    }

    // async Task that returns 42 after awaiting Task.Delay(1) and then a
    // trivial await Task.Yield — exercises Delay+TCS+WhenAll+Task.Yield
    // composition in one subject for full coverage.
    public static async Task ComposeAll()
    {
        var tcs = new TaskCompletionSource<int>();
        var t1 = Task.Delay(1);
        var t2 = tcs.Task;
        await Task.Yield();
        await Task.WhenAll(t1, t2);
    }
}
