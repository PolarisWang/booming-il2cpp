#!/usr/bin/env python3
"""Generate IR opcode enum and name files from schemas/ir_opcodes.yaml.

Outputs:
  - src/native/interpreter/generated/ir_opcodes.h       (C++ enum)
  - src/native/interpreter/generated/ir_opcode_names.h  (C++ name array)
  - src/managed/Chaos.IL2CPP.Contracts/generated/InstructionOpCode.cs  (C# enum)
"""

import os
import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    print("ERROR: PyYAML is required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)


REPO_ROOT = Path(__file__).resolve().parent.parent.parent

YAML_PATH = REPO_ROOT / "schemas" / "ir_opcodes.yaml"
CPP_ENUM_PATH = (
    REPO_ROOT
    / "src"
    / "native"
    / "interpreter"
    / "generated"
    / "ir_opcodes.h"
)
CPP_NAMES_PATH = (
    REPO_ROOT
    / "src"
    / "native"
    / "interpreter"
    / "generated"
    / "ir_opcode_names.h"
)
CSHARP_ENUM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Contracts"
    / "generated"
    / "InstructionOpCode.cs"
)


def load_opcodes(path: Path):
    with open(path, "r") as f:
        data = yaml.safe_load(f)
    return data["opcodes"]


def generate_cpp_enum(opcodes, path: Path):
    count = len(opcodes)
    lines = [
        "// Auto-generated from schemas/ir_opcodes.yaml -- DO NOT EDIT",
        "#ifndef CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "#define CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "",
        '#include <cstdint>',
        "",
        "namespace chaos::il2cpp::interpreter {",
        "",
        "enum class IROpCode : std::int32_t {",
    ]
    last_val = opcodes[-1]["value"]
    for op in opcodes:
        comma = "," if op["value"] != last_val else " "
        lines.append(f"    {op['name']} = {op['value']}{comma}")
    lines += [
        "};",
        "",
        "}  // namespace chaos::il2cpp::interpreter",
        "",
        "#endif  // CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "",
    ]
    text = "\n".join(lines)
    with open(path, "w") as f:
        f.write(text)
    print(f"  [ok] {path.relative_to(REPO_ROOT)} ({count} opcodes)")


def generate_cpp_names(opcodes, path: Path):
    count = len(opcodes)
    lines = [
        "// Auto-generated from schemas/ir_opcodes.yaml -- DO NOT EDIT",
        "#ifndef CHAOS_IL2CPP_GENERATED_IR_OPCODE_NAMES_H_",
        "#define CHAOS_IL2CPP_GENERATED_IR_OPCODE_NAMES_H_",
        "",
        '#include "ir_opcodes.h"',
        "",
        "namespace chaos::il2cpp::interpreter {",
        "",
        "static constexpr const char* kIROpCodeNames[] = {",
    ]
    for op in opcodes:
        lines.append(f'    "{op["name"]}",')
    lines += [
        "};",
        "",
        f"static_assert(sizeof(kIROpCodeNames) / sizeof(kIROpCodeNames[0]) == {count},",
        f'              "kIROpCodeNames must cover all {count} opcodes");',
        "",
        "}  // namespace chaos::il2cpp::interpreter",
        "",
        "#endif  // CHAOS_IL2CPP_GENERATED_IR_OPCODE_NAMES_H_",
        "",
    ]
    text = "\n".join(lines)
    with open(path, "w") as f:
        f.write(text)
    print(f"  [ok] {path.relative_to(REPO_ROOT)} ({count} names)")


def generate_csharp_enum(opcodes, path: Path):
    count = len(opcodes)
    lines = [
        "// Auto-generated from schemas/ir_opcodes.yaml -- DO NOT EDIT",
        "#pragma warning disable CS1591",
        "namespace Chaos.IL2CPP.Contracts;",
        "",
        "public enum InstructionOpCode : int",
        "{",
    ]
    for op in opcodes:
        lines.append(f"    {op['name']} = {op['value']},")
    lines += [
        "}",
        "#pragma warning restore CS1591",
        "",
    ]
    text = "\n".join(lines)
    with open(path, "w") as f:
        f.write(text)
    print(f"  [ok] {path.relative_to(REPO_ROOT)} ({count} opcodes)")


def main():
    print("Loading opcodes from schemas/ir_opcodes.yaml ...")
    opcodes = load_opcodes(YAML_PATH)
    print(f"  loaded {len(opcodes)} opcodes (0..{opcodes[-1]['value']})")
    print()

    print("Generating C++ enum  -> ir_opcodes.h")
    generate_cpp_enum(opcodes, CPP_ENUM_PATH)

    print("Generating C++ names -> ir_opcode_names.h")
    generate_cpp_names(opcodes, CPP_NAMES_PATH)

    print("Generating C# enum   -> InstructionOpCode.cs")
    generate_csharp_enum(opcodes, CSHARP_ENUM_PATH)

    print()
    print("All generated files updated.")


if __name__ == "__main__":
    main()
