// entry_stubs.cpp — link-time stubs for test framework symbols
//
// The Chaos.TestFramework.Sdk prebuilt lib does not export the assertion
// lifecycle entry points referenced by the TPG-generated runtime-entry.cpp, so
// this translation unit is compiled into the SDK runtime_stubs collection and
// pulled into every entry.exe.
//
// ── Why Assert_Complete is a TRAP stub and not `return 0` ─────────────────
//
// It used to be `return 0;`, which made the runner's assertion attribution
// structurally incapable of working.  Assert.Fail (Assert.cs) records the
// failure by setting Assert.s_exitCode and throwing; the runner reads it back
// through Assert_Complete() after each subject.  A stub that always answers
// "no failure" meant every assertion failure was reported as a plain
// non-assertion failure — indistinguishable from a crash before the assertion
// ran.  The subject-level `assertFailed` field was never once true.
//
// The real definition is emitted by codegen as a normal AOT function body
// (Chaos.TestFramework.Assert.Complete is part of the codegen closure whenever
// subjects can assert).  That definition is what must win.
//
// So this file deliberately does NOT define Assert_Complete as a strong
// `return 0` body — a second strong definition in the link would either be a
// duplicate-symbol error or, worse, silently shadow the real one depending on
// link order (exactly the bug this replaces).
//
// Reset stays here: it is a genuine no-op semantically (clearing thread state
// that the runner also clears via the real AOT body), and having it available
// keeps the runtime_stubs collection self-contained for non-test consumers.
//
// If a future chunk's codegen closure genuinely lacks Assert.Complete, the link
// fails loudly at that symbol — which is the correct, diagnosable outcome.

#include <cstdint>

extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset()
{
}
