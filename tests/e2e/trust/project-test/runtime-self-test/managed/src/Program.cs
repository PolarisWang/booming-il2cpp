using RuntimeSelfTest;

// Runtime Self Test — entry point for complete-project il2cpp verification.
//
// Each module produces a deterministic RTS_<CATEGORY>=<value> line on stdout.
// The project-test fact stage captures and validates these lines.
//
// To add a new verification: add a new source file, call it from here,
// and update expected/fact-output.txt.

// ── Arithmetic Proof ──────────────────────────────────────────────────
Console.WriteLine("RTS_ARITH_ADD=" + ArithmeticProof.Add(2, 3).ToString());
Console.WriteLine("RTS_ARITH_SUB=" + ArithmeticProof.Subtract(10, 4).ToString());
Console.WriteLine("RTS_ARITH_MUL=" + ArithmeticProof.Multiply(3, 4).ToString());

// ── GC Allocation Proof ───────────────────────────────────────────────
var gcAlloc = new GcAllocationProof();
Console.WriteLine("RTS_GC_ALLOC=" + gcAlloc.AllocateAndReturn(42).ToString());
Console.WriteLine("RTS_GC_ALLOC_ATOMIC=" + gcAlloc.AllocateAtomic(100).ToString());

// ── String Operations Proof ───────────────────────────────────────────
var strOps = new StringOperations();
Console.WriteLine("RTS_STR_FORMAT=" + strOps.FormatString("hello", 3));
Console.WriteLine("RTS_STR_CONCAT=" + strOps.ConcatStrings("hello", "world"));

// ── Array Operations Proof ────────────────────────────────────────────
var arrOps = new ArrayOperations();
Console.WriteLine("RTS_ARR_SUM=" + arrOps.CreateAndSum(5).ToString());
Console.WriteLine("RTS_ARR_LEN=" + arrOps.CreateArray(10).Length.ToString());

// ── Exception Handling Proof ──────────────────────────────────────────
// (run inside try-catch; net8 console returns 0 even on unhandled in some configs)
try
{
    // Division by zero should throw in checked context
    int zero = 0;
    int result = 5 / zero;
    Console.WriteLine("RTS_EXC_UNEXPECTED=reached");
}
catch
{
    Console.WriteLine("RTS_EXC_HANDLED=-1");
}

// ── Integration Proof — Cross-Module Tests ────────────────────────────
// Each test exercises multiple runtime subsystems simultaneously:
//   GC + Delegate, GC + Finalizer, GC + Interpreter dispatch, GC + Threading

Console.WriteLine("RTS_INT_DELEGATE=" + IntegrationProof.GcAndDelegateInvoke(21).ToString());
// 21 * 2 = 42

Console.WriteLine("RTS_INT_FINALIZER=" + IntegrationProof.FinalizerAndNewAllocation(99).ToString());
// New object with Value=99, returns 99

Console.WriteLine("RTS_INT_GC_ACROSS=" + IntegrationProof.GcAcrossMethodCalls(4).ToString());
// 4 phases: 120 + 528 + 1224 + 2208 = 4080

Console.WriteLine("RTS_INT_THREADED=" + IntegrationProof.ThreadedGcAllocation(10).ToString());
// sum(0..9) = 45

// ── Exit ──────────────────────────────────────────────────────────────
Console.WriteLine("RTS_DONE=ok");
