"""Phase 5: fake-green gate — unit tests.

Asserts the gate rejects stubs and accepts real lowered code.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest

_HERE = Path(__file__).resolve().parent
for _d in (_HERE, _HERE.parent, _HERE.parent.parent):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from verification.tools.fake_green_gate import check_fake_green


def test_real_lowered_code_passes() -> None:
    """A real iterator MoveNext body must pass the gate."""
    source = '''
extern "C" void Foo_MoveNext(CHAOS_IL2CPP_INTPTR arg) {
    CHAOS_IL2CPP_INT32 chaos_pc = 0;
    while (chaos_pc >= 0) {
        switch (chaos_pc) {
        case 0: {
            chaos_continuation = 128;
            chaos_pc = -1;
            break;
        }
        }
    }
    chaos_async_iterator_source_set_result(arg, 1);
}
'''
    issues = check_fake_green(source, "real")
    assert len(issues) == 0, f"real lowered code should have no issues, got: {issues}"


def test_stub_raises_issue() -> None:
    """A chaos_external_runtime_fallback stub must trigger a gate failure."""
    source = '''
extern "C" void Foo(CHAOS_IL2CPP_INTPTR arg) {
    (void)ChaosExternalRuntimeFallback("test");
    return {};
}
'''
    issues = check_fake_green(source, "stub")
    assert any("ChaosExternalRuntimeFallback" in i for i in issues), (
        f"expected ChaosExternalRuntimeFallback issue, got: {issues}"
    )


def test_empty_body_is_stub() -> None:
    """A method with only return {} and no control flow features must fail."""
    source = '''
extern "C" CHAOS_IL2CPP_INT32 Foo(void) {
    return {};
}
'''
    issues = check_fake_green(source, "empty")
    assert any("ALL methods are stub" in i for i in issues), (
        f"expected 'ALL methods are stub' warning, got: {issues}"
    )


def test_unsupported_async_iterator_count_flags() -> None:
    """kUnsupportedAsyncIteratorCount > 0 must produce a gate failure."""
    source = 'static constexpr CHAOS_IL2CPP_INT32 kUnsupportedAsyncIteratorCount = 3;'
    issues = check_fake_green(source, "unsupported")
    assert any("kUnsupportedAsyncIteratorCount" in i for i in issues), (
        f"expected unsupported count issue, got: {issues}"
    )


def test_mixed_code_passes_on_real_methods() -> None:
    """A file containing both stubs and real lowering passes as long as
    at least some methods lower. The gate flags the fallback calls but
    does not reject the whole file — the build-time assertion is the
    _Count, not the absense of every fallback."""
    source = '''
// Real method
extern "C" void Real(CHAOS_IL2CPP_INTPTR arg) {
    CHAOS_IL2CPP_INT32 chaos_pc = 0;
    while (chaos_pc >= 0) {
        switch (chaos_pc) {
        case 0: { chaos_continuation = 42; chaos_pc = -1; break; }
        }
    }
}
// Stub
extern "C" void Stub(void) {
    (void)ChaosExternalRuntimeFallback("test");
    return {};
}
'''
    issues = check_fake_green(source, "mixed")
    fallback_issues = [i for i in issues if "ChaosExternalRuntimeFallback" in i]
    assert len(fallback_issues) >= 1, (
        f"ChaosExternalRuntimeFallback in mixed code should be flagged: {issues}"
    )