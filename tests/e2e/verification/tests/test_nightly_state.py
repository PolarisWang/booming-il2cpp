"""Tests for nightly/state.py — error_class classifier with real failure text.

Uses real failure excerpts from the last nightly run (captured in
nightly-build-report/latest-run-raw-output.txt) to verify the classifier
correctly distinguishes each error category.
"""

from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
for _d in (_HERE.parent.parent, _HERE.parent, _HERE):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from verification.nightly.state import classify_exit


# ── Real failure excerpts from the last nightly run ──

# diagnostics: native codegen missing symbol (C2065)
_TAIL_NATIVE_CODEGEN_MISSING_SYM = (
    'native-aot.generated.cpp(7182): error C2065: '
    "'chaos_type_System_Private_CoreLib_System_Collections_ObjectModel_"
    "ReadOnlyCollection_System_Int32_': undeclared identifier"
)

# diagnostics: native codegen syntax error (C2061/C2143/C3536)
_TAIL_NATIVE_CODEGEN_SYNTAX = (
    'native-aot.generated.cpp(7182): error C2061: '
    "syntax error: identifier 'chaos_type_SomeType_'"
)

# runtime-compiler: platform diagnostic SYSLIB5007
_TAIL_PLATFORM_DIAG = (
    'CombinedSubjects.cs(5098): error SYSLIB5007: '
    "'System.Runtime.CompilerServices.AsyncHelpers' is for evaluation purposes only"
)

# system-xml-schema: ATG combined CS compile error (CS1503)
_TAIL_ATG_COMBINED = (
    'CombinedSubjects.cs(744,127): error CS1503: '
    'Argument 3: cannot convert from '
    "'System.Globalization.CultureInfo' to "
    "'System.Xml.IXmlNamespaceResolver'"
)

# 0xC0000005 native crash
_TAIL_NATIVE_CRASH = (
    "Fatal error. Internal CLR error. (0x80131506)"
)

# Generic csharp-error (not in CombinedSubjects.cs)
_TAIL_CSHARP = (
    'some_project.cs(42,10): error CS1001: Identifier expected'
)

# Success
_TAIL_SUCCESS = (
    "All stages passed.  ->  Pipeline complete: passed"
)

# OOM (exit 137)
_TAIL_OOM = ""


class TestClassifyExit:
    def test_pass(self):
        assert classify_exit(0, _TAIL_SUCCESS) == "pass"

    def test_native_codegen_missing_symbol(self):
        assert classify_exit(1, _TAIL_NATIVE_CODEGEN_MISSING_SYM) == "native-codegen-missing-symbol"

    def test_native_codegen_syntax(self):
        assert classify_exit(1, _TAIL_NATIVE_CODEGEN_SYNTAX) == "native-codegen-syntax"

    def test_atg_combined_cs(self):
        assert classify_exit(1, _TAIL_ATG_COMBINED) == "atg-combined-cs"

    def test_platform_diagnostic(self):
        assert classify_exit(1, _TAIL_PLATFORM_DIAG) == "platform-diagnostic"

    def test_native_crash_exit_code(self):
        assert classify_exit(0xC0000005, "") == "native-crash"

    def test_native_crash_text(self):
        assert classify_exit(1, _TAIL_NATIVE_CRASH) == "native-crash"

    def test_csharp_error(self):
        assert classify_exit(1, _TAIL_CSHARP) == "csharp-error"

    def test_oom(self):
        assert classify_exit(137, _TAIL_OOM) == "oom"

    def test_killed(self):
        assert classify_exit(-9, "") == "killed"

    def test_hotupdate_patch_arm(self):
        """hotupdate stale-executable false-positive → hotupdate-patch-arm, no retry."""
        text = (
            "incremental rebuild failed after host-array regen — "
            "stale executable cannot apply patch (false-positive risk)"
        )
        assert classify_exit(1, text) == "hotupdate-patch-arm"

    def test_unknown(self):
        assert classify_exit(99, "") == "unknown"

    # ── Precedence: narrow match wins over broad ──
    def test_atg_precedes_csharp(self):
        """atg-combined-cs (CombinedSubjects.cs + error CS) precedes csharp-error."""
        text = "CombinedSubjects.cs(1): error CS1001: Identifier expected"
        assert classify_exit(1, text) == "atg-combined-cs"

    def test_missing_sym_precedes_codegen_syntax(self):
        """chaos_type_ + undeclared identifier + C2065 should be missing-symbol, not syntax."""
        text = (
            'native-aot.generated.cpp(7182): error C2065: '
            "'chaos_type_SomeType_': undeclared identifier"
        )
        assert classify_exit(1, text) == "native-codegen-missing-symbol"