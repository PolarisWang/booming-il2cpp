"""Multi-patch hotupdate project generator.

Generates managed/patch_first/ and managed/patch_second/ directories
for any family that has a capability-family-contract.json with methods.

Usage:
    python -m verification.tooling.generate_multi_patch --family convert-char
    python -m verification.tooling.generate_multi_patch --family convert-char,boxing-unboxing-casts
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def _family_class_name(slug: str) -> str:
    """Convert 'convert-char' -> 'ConvertChar', 'multi-patch' -> 'MultiPatch'."""
    return slug.title().replace("-", "").replace("_", "")


def _generate_patch_entry(
    family_slug: str,
    method_count: int,
    patch_variant: str,
    sentinel_base: int,
) -> str:
    """Generate a patch entry .cs file with sentinel return values."""
    class_name = f"{_family_class_name(family_slug)}Patch{patch_variant}Entry"

    lines = [
        f"// {patch_variant} patch entry for {family_slug} family.",
        f"// Returns sentinel value 0x{sentinel_base:X}000000 + N for each subject.",
        f"",
        f"public static partial class {class_name}",
        f"{{",
        f"    public static int _exitCode;",
        f"",
    ]

    for i in range(method_count):
        lines.append(f"    // Subject_{i}")
        lines.append(f"    public static int Subject_{i}()")
        lines.append(f"    {{")
        lines.append(f"        return unchecked((int)(0x{sentinel_base:X}000000u + {i}));")
        lines.append(f"    }}")
        lines.append(f"")

    lines.append(f"    public static void Run(int entryIndex)")
    lines.append(f"    {{")
    lines.append(f"        try")
    lines.append(f"        {{")
    lines.append(f"            switch (entryIndex)")
    lines.append(f"            {{")
    for i in range(method_count):
        lines.append(f"                case {i}: Subject_{i}(); break;")
    lines.append(f"            }}")
    lines.append(f"        }}")
    lines.append(f"        catch")
    lines.append(f"        {{")
    lines.append(f"            _exitCode = 1;")
    lines.append(f"        }}")
    lines.append(f"    }}")
    lines.append(f"}}")
    lines.append(f"")

    return "\n".join(lines)


def _generate_csproj(class_name: str) -> str:
    """Generate a .csproj for the patch entry."""
    return f"""<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Library</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <Nullable>enable</Nullable>
    <ImplicitUsings>disable</ImplicitUsings>
    <AssemblyName>{class_name}</AssemblyName>
    <RootNamespace>{class_name}</RootNamespace>
    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>
  </PropertyGroup>
  <ItemGroup>
    <Compile Include="{class_name}.cs" />
  </ItemGroup>
</Project>
"""


def generate_multi_patch_family(
    family_dir: Path,
) -> bool:
    """Generate multi-patch files for a single family directory."""
    contract_path = family_dir / "capability-family-contract.json"
    if not contract_path.exists():
        print(f"  [skip] no contract: {family_dir.name}")
        return False

    contract = json.loads(contract_path.read_text(encoding="utf-8"))
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])]
    method_count = len(mids)

    if method_count == 0:
        print(f"  [skip] no methods: {family_dir.name}")
        return False

    slug = family_dir.name
    print(f"  [{slug}] {method_count} methods")

    # Generate patch_first
    first_class = f"{_family_class_name(slug)}PatchFirstEntry"
    first_dir = family_dir / "managed" / "patch_first"
    first_dir.mkdir(parents=True, exist_ok=True)
    (first_dir / f"{first_class}.cs").write_text(
        _generate_patch_entry(slug, method_count, "First", 0xB1),
        encoding="utf-8",
    )
    (first_dir / f"{first_class}.csproj").write_text(
        _generate_csproj(first_class),
        encoding="utf-8",
    )
    print(f"    -> patch_first/{first_class}.cs + .csproj")

    # Generate patch_second
    second_class = f"{_family_class_name(slug)}PatchSecondEntry"
    second_dir = family_dir / "managed" / "patch_second"
    second_dir.mkdir(parents=True, exist_ok=True)
    (second_dir / f"{second_class}.cs").write_text(
        _generate_patch_entry(slug, method_count, "Second", 0xD2),
        encoding="utf-8",
    )
    (second_dir / f"{second_class}.csproj").write_text(
        _generate_csproj(second_class),
        encoding="utf-8",
    )
    print(f"    -> patch_second/{second_class}.cs + .csproj")

    return True


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate multi-patch hotupdate projects")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", default="", help="Family slug(s), comma-separated")
    parser.add_argument("--base-dir", default=None, help="Override base directory")
    args = parser.parse_args()

    here = Path(__file__).resolve().parent.parent.parent  # testing/foundation-dll/
    if args.base_dir:
        base_dir = Path(args.base_dir)
    else:
        base_dir = here / args.assembly

    if args.family:
        families = [s.strip() for s in args.family.split(",") if s.strip()]
    else:
        families = sorted(d.name for d in base_dir.iterdir() if d.is_dir() and (d / "capability-family-contract.json").exists())

    print(f"Generating multi-patch projects in {base_dir}")
    print(f"Families: {len(families)}")
    print()

    generated = 0
    for slug in families:
        family_dir = base_dir / slug
        if not family_dir.is_dir():
            print(f"  [skip] directory not found: {slug}")
            continue
        if generate_multi_patch_family(family_dir):
            generated += 1

    print(f"\nDone: {generated}/{len(families)} families generated")


if __name__ == "__main__":
    main()
