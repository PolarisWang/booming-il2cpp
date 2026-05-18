#!/usr/bin/env python3
"""Generate C# and C++ opcode enums from schemas/ir_opcodes.yaml."""
import os
import sys
import yaml

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_schema() -> list[dict]:
    schema_path = os.path.join(REPO_ROOT, "schemas", "ir_opcodes.yaml")
    with open(schema_path, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    return data["opcodes"]


def generate_csharp(opcodes: list[dict]) -> str:
    lines = [
        "// Auto-generated from schemas/ir_opcodes.yaml -- DO NOT EDIT",
        "#pragma warning disable CS1591",
        "namespace Chaos.IL2CPP.Contracts;",
        "",
        "public enum InstructionOpCode : int",
        "{",
    ]
    for i, op in enumerate(opcodes):
        comma = "," if i < len(opcodes) - 1 else ","
        lines.append(f"    {op['name']} = {op['value']}{comma}")
    lines.extend([
        "}",
        "#pragma warning restore CS1591",
        "",
    ])
    return "\n".join(lines) + "\n"


def generate_cpp_header(opcodes: list[dict]) -> str:
    lines = [
        "// Auto-generated from schemas/ir_opcodes.yaml -- DO NOT EDIT",
        "#ifndef CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "#define CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "",
        "#include <cstdint>",
        "",
        "namespace chaos::il2cpp::interpreter {",
        "",
        "enum class IROpCode : std::int32_t {",
    ]
    for i, op in enumerate(opcodes):
        comma = "," if i < len(opcodes) - 1 else ","
        lines.append(f"    {op['name']} = {op['value']}{comma}")
    lines.extend([
        "};",
        "",
        "}  // namespace chaos::il2cpp::interpreter",
        "",
        "#endif  // CHAOS_IL2CPP_GENERATED_IR_OPCODES_H_",
        "",
    ])
    return "\n".join(lines) + "\n"


def generate_cpp_names(opcodes: list[dict]) -> str:
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
    for i, op in enumerate(opcodes):
        comma = "," if i < len(opcodes) - 1 else ","
        lines.append(f'    "{op["name"]}"{comma}')
    lines.extend([
        "};",
        "",
        "static_assert(sizeof(kIROpCodeNames) / sizeof(kIROpCodeNames[0]) == " + str(len(opcodes)) + ",",
        '              "kIROpCodeNames must cover all ' + str(len(opcodes)) + ' opcodes");',
        "",
        "}  // namespace chaos::il2cpp::interpreter",
        "",
        "#endif  // CHAOS_IL2CPP_GENERATED_IR_OPCODE_NAMES_H_",
        "",
    ])
    return "\n".join(lines) + "\n"


def main():
    opcodes = load_schema()
    print(f"Loaded {len(opcodes)} opcodes from schema")

    # C# output
    cs_dir = os.path.join(REPO_ROOT, "src", "managed", "Chaos.IL2CPP.Contracts", "generated")
    os.makedirs(cs_dir, exist_ok=True)
    cs_path = os.path.join(cs_dir, "InstructionOpCode.cs")
    with open(cs_path, "w", encoding="utf-8") as f:
        f.write(generate_csharp(opcodes))
    print(f"Wrote {cs_path}")

    # C++ output
    cpp_dir = os.path.join(REPO_ROOT, "src", "native", "interpreter", "generated")
    os.makedirs(cpp_dir, exist_ok=True)
    cpp_path = os.path.join(cpp_dir, "ir_opcodes.h")
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(generate_cpp_header(opcodes))
    print(f"Wrote {cpp_path}")

    cpp_names_path = os.path.join(cpp_dir, "ir_opcode_names.h")
    with open(cpp_names_path, "w", encoding="utf-8") as f:
        f.write(generate_cpp_names(opcodes))
    print(f"Wrote {cpp_names_path}")


if __name__ == "__main__":
    main()