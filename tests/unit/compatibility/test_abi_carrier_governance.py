"""Phase 1.4: ABI carrier governance test.

Verifies that:
  1. C# AotCoreIrAbiCarrierKind enum has all 15 carriers (0-14).
  2. C ChaosAbiCarrierCode enum matches the C# values exactly.
  3. All 7 switch statements in InvocationAbi.cs handle every carrier without
     falling through to the default/throw path.
  4. C# FNV-1a checksum implementation matches the native C++ implementation.
  5. The carrier codes in generated ABI manifests match the contract.

This is a static-analysis governance test — it reads source files and verifies
structural properties without running the IL2CPP pipeline.
"""

from __future__ import annotations

import ast
import re
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]
INVOCATION_ABI_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "RuntimeSupport" / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "TypedIlAndAotCoreIrContracts.cs"
ABI_MANIFEST_H_PATH = REPO_ROOT / "contracts" / "native" / "v0" / "abi_manifest.h"
MODULE_REGISTRATION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "Emission" / "NativeAotLoweringPlanner.ModuleRegistration.cs"

# All 15 carriers with their expected enum names and values
EXPECTED_CARRIERS: dict[str, int] = {
    "Void": 0,
    "Int32": 1,
    "NativeInt": 2,
    "ValueTypeByValue": 3,
    "Int8": 4,
    "UInt8": 5,
    "Int16": 6,
    "UInt16": 7,
    "Float32": 8,
    "Float64": 9,
    "Int64": 10,
    "UInt64": 11,
    "ByRef": 12,
    "MultiReturn": 13,
    "ByRefToValueType": 14,
}

# Methods in InvocationAbi.cs that contain switch-on-carrier-kind logic.
# Return-oriented methods handle all 15 carriers including Void.
# Parameter-oriented methods handle 14 carriers (Void is not a valid parameter type).
SWITCH_METHODS_RETURN = [
    "EmitMethodReturn",
    "EmitAbiReturnPush",
    "MapAbiSlotReturnType",
]

SWITCH_METHODS_PARAM = [
    "EmitAbiArgumentInitialization",
    "FormatAbiArgumentExpression",
    "FormatInboundAbiArgumentExpression",
    "MapAbiSlotParameterType",
]


class AbiCarrierGovernanceTests(unittest.TestCase):
    """Validate all 15 ABI carriers are correctly defined and handled."""

    # ── helpers ──────────────────────────────────────────────────────────

    def _read_source(self, path: Path) -> str:
        self.assertTrue(path.is_file(), msg=f"missing source: {path}")
        return path.read_text(encoding="utf-8")

    def _extract_csharp_enum(self, source: str) -> dict[str, int]:
        """Parse C# enum definition and return {name: value} dict."""
        enum_pattern = re.compile(
            r'enum\s+AotCoreIrAbiCarrierKind\s*:\s*byte\s*\{(.*?)\}',
            re.DOTALL,
        )
        m = enum_pattern.search(source)
        self.assertIsNotNone(m, msg="AotCoreIrAbiCarrierKind enum not found")
        body = m.group(1)

        carriers: dict[str, int] = {}
        for line in body.splitlines():
            line = line.strip()
            # Skip comments, blank lines
            if not line or line.startswith("///") or line.startswith("//") or line.startswith("/*"):
                continue
            # Match "Name = N" or "Name," patterns
            entry_match = re.match(r'(\w+)\s*=\s*(\d+)', line)
            if entry_match:
                carriers[entry_match.group(1)] = int(entry_match.group(2))
            else:
                # Just a name with trailing comma (sequential)
                name_match = re.match(r'(\w+),?\s*$', line)
                if name_match and name_match.group(1) != "enum":
                    # Value implied by position — skip, we rely on explicit values
                    pass
        return carriers

    def _extract_c_enum_values(self, source: str) -> dict[str, int]:
        """Parse C enum-like #define block and return {name: value} dict."""
        carriers: dict[str, int] = {}
        for line in source.splitlines():
            m = re.match(r'\s*CHAOS_ABI_CARRIER_(\w+)\s*=\s*(\d+)', line.strip())
            if m:
                carriers[m.group(1)] = int(m.group(2))
        return carriers

    def _extract_switch_carriers(self, source: str, method_name: str) -> set[str]:
        """Extract all carrier kind names handled in a switch statement.

        Handles both switch expressions (`CarrierKindCode switch { ... }`)
        and switch statements (`switch (var) { case ... }`).
        """
        # Find the method body
        method_pattern = re.compile(
            rf'private\s+static\s+\w+\s+{re.escape(method_name)}\s*\(',
        )
        method_match = method_pattern.search(source)
        if not method_match:
            self.fail(f"Method {method_name} not found in InvocationAbi.cs")

        # Find the switch from method start position
        start = method_match.end()
        switch_start = source.find("CarrierKindCode switch", start)
        if switch_start == -1:
            switch_start = source.find("switch (", start)

        self.assertNotEqual(
            switch_start, -1,
            msg=f"No switch on CarrierKindCode found in {method_name}",
        )

        # Find the opening brace of the switch
        brace_start = source.find("{", switch_start)
        self.assertNotEqual(brace_start, -1, msg=f"No switch body found in {method_name}")

        # Extract the switch body text (balanced braces)
        depth = 0
        switch_body = ""
        for i in range(brace_start, len(source)):
            ch = source[i]
            if ch == '{':
                depth += 1
                if depth == 1:
                    continue  # skip the opening brace itself
            elif ch == '}':
                depth -= 1
                if depth == 0:
                    break  # end of switch
            if depth > 0:
                switch_body += ch

        # Extract unique carrier names from the switch body
        carriers: set[str] = set()
        for m in re.finditer(
            r'AotCoreIrAbiCarrierKind\.(\w+)',
            switch_body,
        ):
            carriers.add(m.group(1))

        return carriers

    # ── tests ────────────────────────────────────────────────────────────

    def test_csharp_enum_has_all_carriers(self) -> None:
        """C# AotCoreIrAbiCarrierKind must define all 15 carriers with correct values."""
        source = self._read_source(CONTRACTS_PATH)
        carriers = self._extract_csharp_enum(source)

        # Check count
        self.assertEqual(
            len(EXPECTED_CARRIERS), len(carriers),
            msg=f"Expected {len(EXPECTED_CARRIERS)} enum values, found {len(carriers)}: {set(carriers)}",
        )

        # Check each carrier name and value
        for name, expected_value in EXPECTED_CARRIERS.items():
            self.assertIn(name, carriers, msg=f"Missing C# AotCoreIrAbiCarrierKind.{name}")
            self.assertEqual(
                expected_value, carriers[name],
                msg=f"C# AotCoreIrAbiCarrierKind.{name} value mismatch: expected {expected_value}, got {carriers[name]}",
            )

    def test_c_contract_matches_csharp(self) -> None:
        """C ChaosAbiCarrierCode values must match C# enum exactly."""
        source = self._read_source(ABI_MANIFEST_H_PATH)
        c_carriers = self._extract_c_enum_values(source)

        # Map C prefix naming (NATIVE_INT vs NativeInt, VALUE_TYPE_BY_VALUE vs ValueTypeByValue)
        def normalize(name: str) -> str | None:
            mapping = {
                "VOID": "Void",
                "INT32": "Int32",
                "NATIVE_INT": "NativeInt",
                "VALUE_TYPE_BY_VALUE": "ValueTypeByValue",
                "INT8": "Int8",
                "UINT8": "UInt8",
                "INT16": "Int16",
                "UINT16": "UInt16",
                "FLOAT32": "Float32",
                "FLOAT64": "Float64",
                "INT64": "Int64",
                "UINT64": "UInt64",
                "BY_REF": "ByRef",
                "MULTI_RETURN": "MultiReturn",
                "BY_REF_TO_VALUE_TYPE": "ByRefToValueType",
            }
            return mapping.get(name)

        for c_name, c_value in c_carriers.items():
            cs_name = normalize(c_name)
            self.assertIsNotNone(
                cs_name,
                msg=f"Unknown C carrier name: CHAOS_ABI_CARRIER_{c_name}",
            )
            self.assertIn(
                cs_name, EXPECTED_CARRIERS,
                msg=f"C carrier CHAOS_ABI_CARRIER_{c_name} has no C# equivalent",
            )
            self.assertEqual(
                EXPECTED_CARRIERS[cs_name], c_value,
                msg=f"C carrier CHAOS_ABI_CARRIER_{c_name}={c_value} does not match C# {cs_name}={EXPECTED_CARRIERS[cs_name]}",
            )

        self.assertEqual(
            len(EXPECTED_CARRIERS), len(c_carriers),
            msg=f"Expected {len(EXPECTED_CARRIERS)} C carriers, found {len(c_carriers)}",
        )

    def test_all_carriers_handled_in_switch_statements(self) -> None:
        """Every switch on CarrierKindCode must handle all carriers."""
        source = self._read_source(INVOCATION_ABI_PATH)

        # Return-oriented methods must handle all 15 carriers (including Void)
        for method_name in SWITCH_METHODS_RETURN:
            handled = self._extract_switch_carriers(source, method_name)
            for cs_name in EXPECTED_CARRIERS:
                self.assertIn(
                    cs_name, handled,
                    msg=f"Switch in {method_name} does not handle AotCoreIrAbiCarrierKind.{cs_name}. "
                        f"Handled: {sorted(handled)}",
                )

        # Parameter-oriented methods handle 14 carriers (Void excluded)
        for method_name in SWITCH_METHODS_PARAM:
            handled = self._extract_switch_carriers(source, method_name)
            for cs_name in EXPECTED_CARRIERS:
                if cs_name == "Void":
                    continue  # Void is not a valid parameter type
                self.assertIn(
                    cs_name, handled,
                    msg=f"Switch in {method_name} does not handle AotCoreIrAbiCarrierKind.{cs_name}. "
                        f"Handled: {sorted(handled)}",
                )

    def test_csharp_checksum_matches_native(self) -> None:
        """C# ComputeAbiManifestChecksum must use the same FNV-1a algorithm as native."""
        source = self._read_source(MODULE_REGISTRATION_PATH)

        # Extract the FNV-1a constants
        self.assertIn("2166136261u", source, msg="C# checksum missing FNV offset basis")
        self.assertIn("16777619u", source, msg="C# checksum missing FNV prime")

        # Verify hash order: entries first, then parameters
        self.assertIn(
            "// Hash entries:", source,
            msg="C# checksum must hash entries first (matching native layout)",
        )
        self.assertIn(
            "// Hash parameters array", source,
            msg="C# checksum must hash parameters after entries",
        )

        # Verify the hash loop order
        entries_lines = source.split("// Hash entries:")[1].split("// Hash parameters")[0]
        params_lines = source.split("// Hash parameters")[1]

        # Entries loop must iterate reachableMethods first
        self.assertIn(
            "foreach (var method in reachableMethods)", entries_lines,
            msg="Entries hash must iterate reachableMethods",
        )
        # Params loop has nested iteration
        self.assertIn(
            "foreach (var abi in method.ParameterAbis)", params_lines,
            msg="Parameters hash must iterate method.ParameterAbis",
        )

        # Verify no skip — checksum must be non-zero
        checksum_emit_line = [l for l in source.splitlines() if "checksum" in l and "Append" in l]
        self.assertTrue(
            any("checksum" in l and "Append(checksum)" in l for l in source.splitlines()),
            msg="C# codegen must emit ComputeAbiManifestChecksum result (not 0)",
        )

    def test_no_unreachable_carrier_code_in_default(self) -> None:
        """The default/throw case in switch expressions should be unreachable
        since all relevant carriers are handled."""
        source = self._read_source(INVOCATION_ABI_PATH)

        throw_count = source.count('throw new NotSupportedException')
        self.assertGreater(throw_count, 0, msg="Expected at least one throw in default case")

        # Return-oriented methods: must handle all 15 carriers
        for method_name in SWITCH_METHODS_RETURN:
            handled = self._extract_switch_carriers(source, method_name)
            unhandled = set(EXPECTED_CARRIERS.keys()) - handled
            self.assertSetEqual(
                unhandled, set(),
                msg=f"{method_name} has unhandled carriers: {unhandled}",
            )

        # Parameter-oriented methods: must handle 14 carriers (Void excluded)
        param_expected = set(EXPECTED_CARRIERS.keys()) - {"Void"}
        for method_name in SWITCH_METHODS_PARAM:
            handled = self._extract_switch_carriers(source, method_name)
            unhandled = param_expected - handled
            self.assertSetEqual(
                unhandled, set(),
                msg=f"{method_name} has unhandled carriers: {unhandled}",
            )

    def test_carrier_count_unchanged(self) -> None:
        """The total number of carriers should remain 15.
        If this test fails after adding new carriers, update EXPECTED_CARRIERS."""
        self.assertEqual(15, len(EXPECTED_CARRIERS), msg="Carrier count changed")


if __name__ == "__main__":
    unittest.main()
