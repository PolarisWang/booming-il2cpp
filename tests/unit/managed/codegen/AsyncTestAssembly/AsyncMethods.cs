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

    // Task.Run(Action) — fire-and-forget scheduling on the native ThreadPool.
    // Exercises the Task::Run static overload routing (ASYNC-P1-1): the call
    // must lower to the native async_task_run symbol rather than the
    // interpreter fallback.
    public static Task RunAction(Action work)
    {
        return Task.Run(work);
    }

    // ALIVE_ACTION_SENTINEL: a non-async method whose body is a plain constant
    // return, used by the loader-coverage diagnostic to prove that plain (non-
    // async) methods do reach the AOT IR.  If this one is also missing from a
    // loaded world while the async methods are present, the drop is not about
    // `async` at all.
    public static int AliveSentinel()
    {
        return 0xA11FE;
    }

    // await Task.Run(Action) — the awaited form, combining the Run entry with
    // the standard TaskAwaiter resume path.
    public static async Task AwaitRunAction(Action work)
    {
        await Task.Run(work);
    }

    // Task.ContinueWith(Action<Task>) — the single ContinueWith overload whose
    // full semantics the native helper honours (ASYNC-P2-4).  A non-async
    // method so the subject reaches the AOT IR as a plain lowered call.
    public static Task ContinueWithAction(Task antecedent, Action<Task> body)
    {
        return antecedent.ContinueWith(body);
    }

    // Task.ContinueWith(Action<Task>, TaskContinuationOptions) — NOT wired: the
    // native helper discards the options argument, so routing it would run the
    // body when the caller may have asked it not to.  The registry resolver
    // deliberately returns null for this shape, so codegen must fall through to
    // the interpreter rather than emit a call to chaos_task_continue_with.
    public static Task ContinueWithOptions(Task antecedent, Action<Task> body,
                                           TaskContinuationOptions options)
    {
        return antecedent.ContinueWith(body, options);
    }
}
