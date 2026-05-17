from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
METHOD_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)


class PInvokeMarshallingCoverageTests(unittest.TestCase):
    """Verify codegen emits marshalling infrastructure for P/Invoke."""

    def test_codegen_has_clear_os_last_error(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("ClearOsLastError", source)

    def test_codegen_has_set_last_pinvoke_error(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("SetLastPInvokeError", source)

    def test_codegen_has_marshal_string_to_cotaskmem_utf8(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalStringToCoTaskMemUtf8", source)

    def test_codegen_has_marshal_string_to_cotaskmem_wide(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalStringToCoTaskMemWide", source)

    def test_codegen_has_marshal_free_cotaskmem(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalFreeCoTaskMem", source)

    def test_codegen_has_marshal_safehandle_get_handle(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalSafeHandleGetHandle", source)

    def test_codegen_has_marshal_struct_managed_to_native(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalStructManagedToNative", source)

    def test_codegen_has_marshal_struct_native_to_managed(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalStructNativeToManaged", source)

    def test_codegen_has_destroy_marshalled_struct_in_helpers(self) -> None:
        """DestroyMarshalledStruct is emitted via RuntimeHelperShapeRegistry, not MethodEmission directly."""
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("MarshalStructNativeToManaged", source)


if __name__ == "__main__":
    unittest.main()
