// ── EH L3: exception type-matching tests ─────────────────────────────────
//
// WHY THIS EXISTS
// ---------------
// The mode-equivalence suite (eh_mode_equivalence_test.cpp) proves that a
// raised exception *reaches* a handler in all three delivery modes.  It says
// nothing about whether the handler that runs is the CORRECT one.
//
// C# `catch (SomeType)` lowers to a filtered catch — codegen emits, inside
// CHAOS_EH_CATCH_BEGIN:
//
//     if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_SomeType))
//         CHAOS_EH_RETHROW;
//
// (see NativeAotLoweringPlanner.StructuredIR.Emit.cs:1348).  The failure mode
// this guards against is that a mismatched catch silently *takes* the
// exception: the wrong handler runs, the right one never sees it, and the
// test that should have failed passes.  That is exactly the class of bug the
// project keeps rediscovering (see the "guard test must not satisfy itself"
// and "operand-less catchall false green" entries in the memory index).
//
// What is asserted here is a property of the MATCHER, not of a delivery mode:
// given an exception object carrying type T, matching against T (or a base of
// T) succeeds and matching against an unrelated type fails — identically in
// CPP_THROW, WIN32_SEH and SETJMP.
//
// TYPE GRAPH USED
//   ArgumentNullException → ArgumentException → SystemException → Exception
// This mirrors the parent chains the emitter actually produces (verified in
// generated output: chaos_mt_System_..._ArgumentNullException carries
// ArgumentException as its parent).
//
// SCOPE NOTE: this suite constructs TypeInfo values by hand rather than
// linking the real emitted MethodTables.  That is deliberate — the property
// under test is the matcher's walk up the parent chain, and hand-built
// TypeInfo keeps the test independent of codegen availability so it can run
// as a standalone CI gate (same rationale as the mode-equivalence suite).
// It does NOT prove that codegen emits correct parent links; that is covered
// separately by inspecting generated output.

#include <generated_code_compat.h>
#include <exception_jmp.h>
#include <chaos/native_types.h>
#include <chaos/eh.h>
// Provides chaos_eh_match_type / chaos_is_type_compatible / chaos_object_get_type_info.
// It is the same prelude generated code includes, so the matcher under test is
// the one codegen actually calls — not a reimplementation.
#include <ChaosGeneratedRuntimePrelude.h>

// The prelude defines the matcher inside `namespace chaos::il2cpp::jit` —
// generated code reaches it via the enclosing codegen namespace plus a
// `using namespace` (see the preamble the emitter writes).  Repeating that
// here is what makes this test exercise the real implementation rather than
// a local re-creation of it.
using namespace chaos::il2cpp::jit;

#include <cstdio>
#include <cstring>

// ── Test harness (same shape as eh_mode_equivalence_test.cpp) ────────────

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST(name) do { ++g_tests_run; std::printf("  TEST: %s ... ", name); } while (0)
#define PASS()     do { ++g_tests_passed; std::puts("PASS"); } while (0)
#define FAIL(msg)  do { std::puts("FAIL"); std::printf("    %s\n", msg); } while (0)
#define CHECK(cond, msg) do { if (cond) { PASS(); } else { FAIL(msg); } } while (0)

// ── TypeInfo fixtures ────────────────────────────────────────────────────
//
// stable_id is what chaos_is_type_compatible actually compares (plus pointer
// identity), so distinct values per type are what make the graph meaningful.
// parent == nullptr terminates the walk.

static TypeInfoHot g_ti_Exception{};
static TypeInfoHot g_ti_SystemException{};
static TypeInfoHot g_ti_ArgumentException{};
static TypeInfoHot g_ti_ArgumentNullException{};
static TypeInfoHot g_ti_Unrelated{};

static void InitTypeGraph() {
    g_ti_Exception.parent       = nullptr;
    g_ti_Exception.stable_id    = 1001;

    g_ti_SystemException.parent       = &g_ti_Exception;
    g_ti_SystemException.stable_id    = 1002;

    g_ti_ArgumentException.parent       = &g_ti_SystemException;
    g_ti_ArgumentException.stable_id    = 1003;

    g_ti_ArgumentNullException.parent       = &g_ti_ArgumentException;
    g_ti_ArgumentNullException.stable_id    = 1004;

    // Deliberately NOT in the chain — stands in for "some other exception type".
    g_ti_Unrelated.parent       = nullptr;
    g_ti_Unrelated.stable_id    = 9999;
}

// ── Exception object fixture ─────────────────────────────────────────────
//
// chaos_eh_match_type reads the object header's type_info pointer, so a
// minimal header + TypeInfo pointer is a faithful stand-in.  The value is
// passed around as CHAOS_IL2CPP_INTPTR exactly as codegen does.

static ThinLockableHeader g_exc_obj{};

static CHAOS_IL2CPP_INTPTR MakeExceptionOf(const TypeInfoHot* ti) {
    g_exc_obj.type_info = ti;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_exc_obj);
}

// ── Case 1: exact type match succeeds ────────────────────────────────────
static void test_exact_match() {
    TEST("matcher accepts the exception's own type");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    CHECK(chaos_eh_match_type(exc, &g_ti_ArgumentNullException),
          "exact type did not match — the handler for its own type would be skipped");
}

// ── Case 2: base type match succeeds (C# catch(Base) catches Derived) ────
static void test_base_match() {
    TEST("matcher accepts a base type of the exception's type");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    CHECK(chaos_eh_match_type(exc, &g_ti_ArgumentException),
          "derived exception not matched by catch of its immediate base");
}

// ── Case 3: transitive base match succeeds ───────────────────────────────
// The walk must span the whole chain, not just one hop.
static void test_transitive_base_match() {
    TEST("matcher accepts a transitive base (4 levels up)");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    CHECK(chaos_eh_match_type(exc, &g_ti_Exception),
          "derived exception not matched by catch of a transitive base — parent walk is too shallow");
}

// ── Case 4: unrelated type does NOT match ────────────────────────────────
// THE negative control that matters.  If this passed, every filtered catch
// would take every exception and handler dispatch would be meaningless.
static void test_unrelated_does_not_match() {
    TEST("matcher rejects an unrelated type (negative control)");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    CHECK(!chaos_eh_match_type(exc, &g_ti_Unrelated),
          "unrelated type matched — filtered catches would swallow foreign exceptions");
}

// ── Case 5: a BASE-typed exception does NOT match a DERIVED catch ────────
// Direction matters: catch(ArgumentNullException) must not catch an
// ArgumentException.  Getting this backwards would make C#'s specificity
// rules unenforceable.
static void test_base_exception_not_matched_by_derived_catch() {
    TEST("base-typed exception is NOT caught by a derived-type catch");
    auto exc = MakeExceptionOf(&g_ti_ArgumentException);
    CHECK(!chaos_eh_match_type(exc, &g_ti_ArgumentNullException),
          "base exception matched a derived catch — catch specificity is inverted");
}

// ── Case 6: the throw/catch round trip, filtered ─────────────────────────
// End-to-end over the macro surface: raise an ArgumentNullException, have a
// catch(ArgumentException) handler take it, and confirm the handler body saw
// the right object.  This is the shape codegen emits for a successful
// typed catch and is the closest this suite gets to generated code.
static void test_filtered_catch_takes_matching_exception() {
    TEST("filtered catch takes a matching exception (end-to-end)");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    int taken = 0;
    CHAOS_IL2CPP_INTPTR observed = 0;

    CHAOS_EH_TRY
        chaos::il2cpp::runtime_core::chaos_raise_exception(exc);
    CHAOS_EH_CATCH_BEGIN
        if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, &g_ti_ArgumentException)) {
            CHAOS_EH_RETHROW;
        }
        observed = CHAOS_EH_EXCEPTION_OBJ;
        ++taken;
    CHAOS_EH_END

    CHECK(taken == 1 && observed == exc,
          "matching handler did not run, or saw a different object");
}

// ── Case 7: the mismatched catch declines, outer handler takes it ────────
// The complement to case 6, and the one that catches a broken rethrow: a
// non-matching handler must hand the exception on rather than consuming it.
static void test_filtered_catch_declines_and_outer_takes() {
    TEST("non-matching handler declines; outer handler receives it");
    auto exc = MakeExceptionOf(&g_ti_ArgumentNullException);
    int inner = 0;
    int outer = 0;

    CHAOS_EH_TRY
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::chaos_raise_exception(exc);
        CHAOS_EH_CATCH_BEGIN
            if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, &g_ti_Unrelated)) {
                CHAOS_EH_RETHROW;
            }
            ++inner;   // must never run
        CHAOS_EH_END
    CHAOS_EH_CATCH_BEGIN
        ++outer;
    CHAOS_EH_END

    CHECK(inner == 0 && outer == 1,
          "wrong handler consumed the exception (inner must be 0, outer 1)");
}

// ── Case 8: raised value 0 (the "nobody resolved the type" case) ─────────
// ResolveTypeByName returns 0 when an exception type is not registered, and
// the runtime raises a NULL object.  chaos_eh_match_type must reject that
// against any concrete type — otherwise an unregistered type would be caught
// by whatever handler happened to come first.
static void test_null_exception_never_matches() {
    TEST("null exception object never matches a concrete type");
    auto nullExc = MakeExceptionOf(nullptr);
    CHECK(!chaos_eh_match_type(nullExc, &g_ti_ArgumentException),
          "a NULL exception object matched a concrete type — unregistered exception types become uncatchable-by-design");
}

// ── Case 9: negative sentinel values are rejected ───────────────────────
// ThreadAbort / ThreadInterrupt are delivered as negative sentinels.  They
// carry no managed object, so no typed catch may claim them.
static void test_sentinel_values_never_match() {
    TEST("negative sentinel exception values never match a concrete type");
    const CHAOS_IL2CPP_INTPTR sentinelAbort =
        static_cast<CHAOS_IL2CPP_INTPTR>(kManagedExceptionThreadAbort);
    CHECK(!chaos_eh_match_type(sentinelAbort, &g_ti_ArgumentException),
          "a negative sentinel matched a concrete type — sentinels would be swallowed by ordinary catches");
}

// ── Entry point ──────────────────────────────────────────────────────────

int main() {
#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
    const char* mode = "WIN32_SEH";
#elif defined(CHAOS_IL2CPP_EH_SETJMP)
    const char* mode = "SETJMP";
#else
    const char* mode = "CPP_THROW";
#endif
    std::printf("EH L3 type-matching suite [%s]\n", mode);

    InitTypeGraph();

    test_exact_match();
    test_base_match();
    test_transitive_base_match();
    test_unrelated_does_not_match();
    test_base_exception_not_matched_by_derived_catch();
    test_filtered_catch_takes_matching_exception();
    test_filtered_catch_declines_and_outer_takes();
    test_null_exception_never_matches();
    test_sentinel_values_never_match();

    std::printf("\n%s: %d/%d passed (mode=%s)\n",
                (g_tests_passed == g_tests_run) ? "PASS" : "FAIL",
                g_tests_passed, g_tests_run, mode);
    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
