"""Core types for the principle check plugin system."""

from __future__ import annotations

import re
from abc import ABC, abstractmethod
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any, ClassVar

_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE.parent  # <...>/foundation_dll/
_REPO_ROOT = _FOUNDATION_DLL.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"


@dataclass
class CheckMeta:
    """Check metadata — declarative registration for each PrincipleCheck subclass."""
    check_id: str
    principle: str                     # "P1" | "P2" | "P3"
    severity: str = "blocking"         # "blocking" | "monitoring"
    scope: str = "family"              # "family" | "cross_family"
    description: str = ""
    dependencies: list[str] = field(default_factory=list)


@dataclass
class CheckResult:
    """Result of a single principle check execution."""
    check_id: str
    status: str                        # ALIGNED | CONCERN | VIOLATION | NOT_APPLICABLE | SKIP
    summary: str = ""
    details: list[dict] = field(default_factory=list)
    evidence: dict = field(default_factory=dict)
    per_method: list[dict] | None = None

    def to_dict(self) -> dict[str, Any]:
        d = asdict(self)
        if d.get("per_method") is None:
            del d["per_method"]
        return d


# ── Helpers (shared across all checks) ─────────────────────────────────

def _family_dir(assembly: str, family_slug: str) -> Path:
    return _VERIFICATION_BASE / assembly / family_slug


def _generated_cpp(assembly: str, family_slug: str) -> Path | None:
    family_dir = _family_dir(assembly, family_slug)
    cpp_candidates = sorted(family_dir.glob("codegen/*/generated/native-aot.generated.cpp"))
    if cpp_candidates:
        return cpp_candidates[0]
    path = family_dir / "codegen" / "generated" / "native-aot.generated.cpp"
    return path if path.exists() else None


def _class_name_from_slug(family_slug: str) -> str:
    """Derive the C++ class name from a family slug.
    'convert-char' -> 'ConvertChar'
    'boxing-unboxing-casts' -> 'BoxingUnboxingCasts'
    """
    return ''.join(word.capitalize() for word in family_slug.replace('_', '-').split('-'))


def _native_entry_re(family_slug: str) -> str:
    """Build a regex matching any native entry function in generated C++."""
    cls = _class_name_from_slug(family_slug)
    patterns = [
        f'{cls}_NativeEntry_',
        f'{cls}_',
        'RunNativeAot_',
        'NativeReferenceStub_',
        'BenchmarkEntry_',
        'NativeEntry_',
    ]
    return '(?:' + '|'.join(patterns) + r')\w+\s*\('


def _method_count_in_cpp(cpp: str, family_slug: str = "") -> int:
    """Count the number of native entry methods in generated C++."""
    if family_slug:
        return len(re.findall(_native_entry_re(family_slug), cpp))
    return len(re.findall(
        r'(?:NativeReferenceStub_|\w+NativeEntry_|\w+_NativeEntry_|RunNativeAot_|BenchmarkEntry_)\w+\s*\(',
        cpp,
    ))


def _parse_method_names(cpp: str, family_slug: str = "") -> list[str]:
    """Extract method full-names (e.g. 'ConvertChar_NativeEntry_ToChar') from generated C++."""
    if family_slug:
        return [m.group(0).rstrip('(') for m in re.finditer(_native_entry_re(family_slug), cpp)]
    return [
        m.group(0).rstrip('(')
        for m in re.finditer(
            r'(?:NativeReferenceStub_|\w+NativeEntry_|\w+_NativeEntry_|RunNativeAot_)(\w+)\s*\(',
            cpp,
        )
    ]


def _detect_class_name(cpp_content: str, family_slug: str, family_dir: Path) -> str:
    """Detect the native entry class name from generated C++.

    Prefers the codegen subdirectory name (source of truth).
    Falls back to deriving from family_slug, then to generic patterns.
    """
    # Method 1: read from codegen subdirectory
    cpp_candidates = sorted(family_dir.glob("codegen/*/generated/native-aot.generated.cpp"))
    if cpp_candidates:
        cn = cpp_candidates[0].parent.parent.name
        if cn:
            return cn

    # Method 2: derive from family slug
    derived = _class_name_from_slug(family_slug)

    # Method 3: scan C++ for functions matching known patterns
    if cpp_content:
        for suffix in ["NativeEntry", "PatchEntry", "Subjects"]:
            candidate = f"{derived}{suffix}"
            if re.search(rf'\b{re.escape(candidate)}_', cpp_content):
                return candidate
    return derived


def _build_native_entry_re(class_name: str) -> str:
    """Build a regex matching native entry function definitions."""
    return (
        rf'(?:{re.escape(class_name)}_{re.escape(class_name)}_|'
        rf'NativeReferenceStub_|RunNativeAot_|BenchmarkEntry_)(\w+)\s*\('
    )


def _method_has_lowering_in_body(cpp: str, method_full_name: str, class_name: str) -> bool:
    """Check whether a specific method's body contains real native AOT lowering.

    Two lowering modes are accepted:
      1. chaos_eval_stack — array-based eval stack (IRFlatRegion / evalStackSize path)
      2. _s0{}; — structured slot declaration (StructuredIR path)

    Uses brace matching to extract each function body. Iterates ALL matches
    (forward declarations + actual implementation) and returns True if ANY
    body contains lowering — this handles codegen files where the same function
    name appears in multiple locations (data tables, hotpatch entries, etc.)
    and the real implementation may appear last.
    """
    fn_pattern = rf'\b{re.escape(class_name)}_{re.escape(class_name)}_{re.escape(method_full_name)}\s*\([^{{]*{{'
    for fn_match in re.finditer(fn_pattern, cpp):
        body_start = fn_match.end() - 1
        brace_depth = 1
        pos = body_start + 1
        while pos < len(cpp) and brace_depth > 0:
            if cpp[pos] == '{':
                brace_depth += 1
            elif cpp[pos] == '}':
                brace_depth -= 1
            pos += 1
        body = cpp[body_start:pos]
        if "chaos_eval_stack" in body or "_s0{};" in body:
            return True
    return False


# ── FamilyContext ──────────────────────────────────────────────────────

@dataclass
class FamilyContext:
    """Context provided to each PrincipleCheck.run() for a specific family."""
    assembly: str
    family_slug: str
    family_dir: Path
    cpp_path: Path | None = None
    class_name: str = ""
    _generated_cpp_content: str = ""
    _cross_ctx: Any = None      # CrossFamilyContext reference (set by aggregator)
    _changed_files: list[str] | None = None

    def __post_init__(self):
        if self.cpp_path is None:
            self.cpp_path = _generated_cpp(self.assembly, self.family_slug)
        if not self.class_name and self.cpp_path:
            self.class_name = _detect_class_name(
                self.generated_cpp_content, self.family_slug, self.family_dir
            )

    @property
    def generated_cpp_content(self) -> str:
        if not self._generated_cpp_content and self.cpp_path and self.cpp_path.exists():
            self._generated_cpp_content = self.cpp_path.read_text(encoding="utf-8")
        return self._generated_cpp_content

    @property
    def contract_methods(self) -> list[str]:
        """Return method names from the family contract."""
        contract_path = self.family_dir / "capability-family-contract.json"
        if not contract_path.exists():
            return []
        import json
        try:
            contract = json.loads(contract_path.read_bytes())
            # methodSubjectIds at top level or inside methodContracts
            mids = contract.get("methodSubjectIds", [])
            if not mids:
                mids = [m["methodSubjectId"] for m in contract.get("methodContracts", [])
                        if m.get("methodSubjectId")]
            return mids
        except (OSError, json.JSONDecodeError):
            return []

    def cpp_method_full_names(self) -> list[str]:
        """Extract method full-names from generated C++ using the detected class name."""
        if not self.generated_cpp_content:
            return []
        if self.class_name:
            namespaced_re = _build_native_entry_re(self.class_name)
            return [m.group(0).rstrip('(') for m in re.finditer(namespaced_re, self.generated_cpp_content)]
        return _parse_method_names(self.generated_cpp_content, self.family_slug)

    def cpp_method_names(self) -> list[str]:
        """Extract short method names from generated C++."""
        full = self.cpp_method_full_names()
        cls = self.class_name
        if cls:
            # Strip classname_classname_ prefix
            result = []
            prefix = f"{cls}_{cls}_"
            for f in full:
                name = f.removeprefix(prefix)
                # Also try classname_NativeEntry_
                name = name.removeprefix(f"{cls}_NativeEntry_")
                result.append(name)
            return result
        return full

    def has_lowering(self, method_full_name: str | None = None) -> bool:
        """Check lowering at file or per-method level.

        Accepts two lowering modes:
          - chaos_eval_stack          (IRFlatRegion / evalStackSize path)
          - _s0{};  (StructuredIR path)
        """
        cpp = self.generated_cpp_content
        if not cpp:
            return False
        if method_full_name and self.class_name:
            return _method_has_lowering_in_body(cpp, method_full_name, self.class_name)
        return "chaos_eval_stack" in cpp or "_s0{};" in cpp

    def list_changed_files(self) -> list[str]:
        """Read git diff for this family's codegen directory."""
        if self._changed_files is not None:
            return self._changed_files
        codegen_dir = self.family_dir / "codegen"
        if not codegen_dir.exists():
            self._changed_files = []
            return self._changed_files

        import subprocess
        try:
            result = subprocess.run(
                ["git", "diff", "--name-only", "HEAD"],
                capture_output=True, encoding="utf-8", cwd=_REPO_ROOT, timeout=30,
            )
            if result.stdout is None:
                self._changed_files = []
                return self._changed_files
            changed = [f for f in result.stdout.strip().split("\n") if f]
            # Filter to files in this family's codegen dir
            codegen_str = str(codegen_dir.resolve())
            self._changed_files = [
                f for f in changed
                if codegen_str in str((_REPO_ROOT / f).resolve())
            ]
            return self._changed_files
        except (subprocess.SubprocessError, OSError):
            self._changed_files = []
            return self._changed_files

    def is_expected_layer(self, layer: str) -> bool:
        """Check whether this family is expected to touch a given architecture layer.

        Stub — in practice this would read family metadata; currently returns True
        for all known layers since we don't have per-family layer expectations yet.
        """
        known_layers = {"Emitter", "Planner", "RuntimeSupport", "RuntimeCore", "Interpreter"}
        return layer in known_layers


# ── Base class ─────────────────────────────────────────────────────────

class PrincipleCheck(ABC):
    """Base class for all principle checks.  Subclasses must set `meta`."""

    meta: ClassVar[CheckMeta]

    @abstractmethod
    def run(self, ctx: FamilyContext) -> CheckResult:
        """Execute the check against the given family context."""
        ...
