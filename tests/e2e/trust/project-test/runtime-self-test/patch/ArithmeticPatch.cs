using System;
using Chaos.TestFramework;

// NOTE: No namespace — the AOT type registry for ArithmeticProof uses an
// empty (global) namespace.  The patch type must also use the global
// namespace so HotpatchNameRegistry::LookupMethod finds the match.
// A namespace mismatch ("RuntimeSelfTest" vs "") would cause lookup failure
// and "Applied patches to 0 methods".

/// <summary>
/// HotUpdate patch for ArithmeticProof.Add
///
/// Original: a + b
/// Patched:  a * b
///
/// When patched, RTS_ARITH_ADD changes from 5 (2+3) to 6 (2*3).
/// </summary>
public static class ArithmeticPatch
{
    public static int Add(int a, int b)
    {
        return a * b;
    }
}
