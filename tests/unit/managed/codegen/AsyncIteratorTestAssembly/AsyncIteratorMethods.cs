// ASYNC-P2-8 A1 fixture: real async ITERATOR subjects (async IAsyncEnumerable<T>).
//
// These compile to state machines that are structurally different from the
// async Task state machines AsyncTestAssembly exercises:
//   * the state machine is a CLASS (async Task ones are valuetypes)
//   * the builder is AsyncIteratorMethodBuilder, driven from the state machine's
//     OWN MoveNextAsync — there is no AsyncTaskMethodBuilder::Start entry
//   * it implements IAsyncEnumerable<T>/IAsyncEnumerator<T> + IValueTaskSource<bool>
//     + IAsyncDisposable, and carries <>v__promiseOfValueOrEnd / <>w__disposeMode
//
// A1's job is NOT to lower these — it is to make the pipeline FAIL LOUDLY when it
// meets them, instead of degrading to a chaos_external_runtime_* stub that returns 0
// (which surfaces much later as a null iterable or a crash inside await foreach).
//
// Requires net9.0. See docs/dev/in-progress/async-task-industrialization/
// async-iterator-recon-2026-09-11.md.

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;

namespace AsyncIteratorTestAssembly;

public static class AsyncIteratorMethods
{
    // Minimal async iterator: one yield return, no await. Its compiler-generated
    // state machine ('<YieldOne>d__0') still uses AsyncIteratorMethodBuilder.
    public static async IAsyncEnumerable<int> YieldOne()
    {
        yield return 1;
    }

    // Async iterator that both awaits and yields — the shape Task.WhenEach's
    // WhenEachState.Iterate<T> uses.
    public static async IAsyncEnumerable<int> YieldAfterAwait()
    {
        await Task.Yield();
        yield return 42;
    }

    // Async iterator with [EnumeratorCancellation] — exercises the token-flow
    // parameter the real Task.WhenEach consumer path carries.
    public static async IAsyncEnumerable<int> YieldWithCancellation(
        [EnumeratorCancellation] CancellationToken cancellationToken = default)
    {
        await Task.Yield();
        cancellationToken.ThrowIfCancellationRequested();
        yield return 7;
    }

    // A plain (non-iterator) async method in the same assembly. This is the
    // COUNTEREXAMPLE anchor: it must keep lowering normally after A1 lands, proving
    // the new hard-failure is scoped to iterator state machines and did not turn
    // into a blanket rejection of all async shapes.
    public static async Task<int> PlainAsync()
    {
        await Task.Yield();
        return 5;
    }
}
