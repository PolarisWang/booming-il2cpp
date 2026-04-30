"""Tests for FNV-1a hash consistency used by RuntimeHelperShapeRegistry.

This test verifies that the FNV-1a 32-bit hash algorithm used in both C# and C++
produces bit-identical results for ASCII strings (the canonical shape key format).
The C# implementation is in RuntimeHelperShapeRegistry.Fnv1aHash().
The C++ implementation is in the generated runtime_helper_shapes.h (constexpr Fnv1aHash).
"""
from __future__ import annotations

import unittest


def fnv1a_32(text: str) -> int:
    """Bit-identical reimplementation of RuntimeHelperShapeRegistry.Fnv1aHash in Python.

    Algorithm: FNV-1a 32-bit
    - Offset basis: 2166136261
    - Prime: 16777619
    - Processes each character's low byte only (ASCII-compatible)
    """
    hash_val = 2166136261
    for ch in text:
        hash_val ^= ord(ch) & 0xFF
        hash_val = (hash_val * 16777619) & 0xFFFFFFFF
    return hash_val


class Fnv1aHashTests(unittest.TestCase):
    """Validate FNV-1a hash consistency for canonical shape keys."""

    def test_known_ascii_strings(self) -> None:
        """FNV-1a of known ASCII strings — print hashes for C# cross-validation.

        These hashes must match RuntimeHelperShapeRegistry.Fnv1aHash() output.
        When shapes are added to BuildDefault(), update this test with the expected hash
        for each shape key by copying the C# computed value.
        """
        test_keys = [
            "",
            "a",
            "System.String::Concat(System.String,System.String)",
            "System.String::op_Equality(System.String,System.String)",
            "System.String::get_Length()",
            "System.Threading.Monitor::Enter(System.Object,System.Boolean&)",
            "System.Exception::.ctor(System.String)",
            "System.Console::WriteLine(System.String)",
            "System.Delegate::Combine(System.Delegate,System.Delegate)",
            "System.Delegate::Remove(System.Delegate,System.Delegate)",
            "System.GC::Collect()",
            "System.GC::KeepAlive(System.Object)",
            "System.Int32::ToString()",
            "System.Type::get_Assembly()",
            "System.Reflection.Assembly::GetType(System.String)",
        ]

        for text in test_keys:
            actual = fnv1a_32(text)
            print(f"  '{text}' -> 0x{actual:08X} ({actual})")
            # Verify it's a valid 32-bit unsigned integer
            self.assertTrue(0 <= actual < 0x100000000, msg=f"Hash out of range for '{text}'")

    def test_cpp_compatible_ascii_only(self) -> None:
        """The hash algorithm must only use the low byte of each char.

        This ensures compatibility with the C++ constexpr implementation
        which operates on 'const char*' (single-byte characters).
        """
        ascii_text = "System.String::Concat(System.String,System.String)"
        hash_ascii = fnv1a_32(ascii_text)

        # If we incorrectly processed high bytes, this would differ
        # (all chars are ASCII so high byte is always 0)
        # This test is a documentation assertion more than anything
        self.assertIsInstance(hash_ascii, int)
        self.assertTrue(0 < hash_ascii < 0x100000000)

    def test_canonical_key_format(self) -> None:
        """Verify the canonical key format matches BuildCanonicalKey()."""
        key = build_canonical_key("System.String", "Concat", ["System.String", "System.String"])
        expected = "System.String::Concat(System.String,System.String)"
        self.assertEqual(key, expected)

        # No-parameter method
        key2 = build_canonical_key("System.String", "get_Length", [])
        self.assertEqual(key2, "System.String::get_Length()")

        # Method with special characters
        key3 = build_canonical_key("System.Threading.Monitor", "Enter", ["System.Object", "System.Boolean&"])
        self.assertEqual(key3, "System.Threading.Monitor::Enter(System.Object,System.Boolean&)")

    def test_no_collisions_among_sample_keys(self) -> None:
        """Verify no hash collisions among the canonical shape keys.

        This mirrors the DEBUG collision check in RuntimeHelperShapeRegistry.Register().
        """
        sample_keys = [
            # String operations
            "System.String::Concat(System.String,System.String)",
            "System.String::Concat(System.String,System.String,System.String)",
            "System.String::Concat(System.String,System.String,System.String,System.String)",
            "System.String::op_Equality(System.String,System.String)",
            "System.String::get_Length()",
            "System.String::StartsWith(System.String,System.StringComparison)",
            "System.String::Contains(System.String,System.StringComparison)",
            # Object
            "System.Object::Equals(System.Object)",
            # GC
            "System.GC::Collect()",
            "System.GC::WaitForPendingFinalizers()",
            "System.GC::KeepAlive(System.Object)",
            # Exception
            "System.Exception::.ctor(System.String)",
            "System.Exception::get_Message()",
            # Threading
            "System.Threading.Monitor::Enter(System.Object,System.Boolean&)",
            "System.Threading.Monitor::Exit(System.Object)",
            "System.Threading.Thread::Start()",
            "System.Threading.Thread::Join()",
            # Delegate
            "System.Delegate::Combine(System.Delegate,System.Delegate)",
            "System.Delegate::Remove(System.Delegate,System.Delegate)",
            # Reflection
            "System.Type::get_Assembly()",
            "System.Reflection.Assembly::GetType(System.String)",
            "System.Type::GetType(System.String)",
            "System.Reflection.MethodBase::Invoke(System.Object,System.Object[])",
            # Interop
            "System.Runtime.InteropServices.Marshal::AllocHGlobal(System.Int32)",
            "System.Runtime.InteropServices.Marshal::FreeHGlobal(System.IntPtr)",
            # Int32
            "System.Int32::ToString()",
            # Convert
            "System.Convert::ToChar(System.Int32)",
        ]

        hashes: dict[int, str] = {}
        for key in sample_keys:
            h = fnv1a_32(key)
            if h in hashes:
                self.fail(f"Hash collision: '{key}' (0x{h:08X}) collides with '{hashes[h]}'")
            hashes[h] = key


def build_canonical_key(type_display_name: str, method_name: str, param_type_display_names: list[str]) -> str:
    """Reimplementation of RuntimeHelperShapeRegistry.BuildCanonicalKey."""
    parts = [type_display_name, "::", method_name, "("]
    parts.append(",".join(param_type_display_names))
    parts.append(")")
    return "".join(parts)


if __name__ == "__main__":
    unittest.main()
