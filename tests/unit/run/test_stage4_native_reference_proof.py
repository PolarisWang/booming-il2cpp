from __future__ import annotations

import json
import shutil
import subprocess
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
CODEGEN_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Chaos.IL2CPP.CodeGen.csproj"
CODEGEN_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeReferenceProof.cpp.scriban"
STAGE4_SRC_SCAN_ROOTS = [
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.MetadataWriter",
    REPO_ROOT / "src" / "native" / "bootstrap",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "src" / "native" / "support",
]
STAGE4_SRC_TEXT_SUFFIXES = {".cs", ".csproj", ".cpp", ".h", ".scriban"}
FORBIDDEN_STAGE4_SRC_SNIPPETS = [
    "HelloWorldObject",
    "Greeter",
    "ClosedGenericEcho",
    "first-proof",
    "tests/proof/input/HelloWorldObject",
    "artifacts/proof/managed-closure/HelloWorldObject",
    "artifacts/proof/native-reference/HelloWorldObject",
]
MANAGED_CLOSURE_ROOT = REPO_ROOT / "artifacts" / "proof" / "managed-closure" / "HelloWorldObject"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "native-reference" / "HelloWorldObject"


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


class Stage4NativeReferenceProofTests(unittest.TestCase):
    maxDiff = None

    def test_stage4_src_stays_sample_agnostic(self) -> None:
        scanned_files = 0

        for root in STAGE4_SRC_SCAN_ROOTS:
            self.assertTrue(root.exists(), msg=f"missing Stage 4 source scan root: {root}")

            for source_path in root.rglob("*"):
                if not source_path.is_file() or source_path.suffix.lower() not in STAGE4_SRC_TEXT_SUFFIXES:
                    continue

                scanned_files += 1
                source_text = source_path.read_text(encoding="utf-8")
                for forbidden_snippet in FORBIDDEN_STAGE4_SRC_SNIPPETS:
                    self.assertNotIn(
                        forbidden_snippet,
                        source_text,
                        msg=f"test-project-specific logic leaked into {source_path}: {forbidden_snippet}",
                    )

        self.assertGreater(scanned_files, 0, msg="expected to scan at least one Stage 4 source file")

    def test_codegen_project_wires_scriban_template_asset(self) -> None:
        self.assertTrue(CODEGEN_TEMPLATE_PATH.is_file(), msg=f"missing Scriban template: {CODEGEN_TEMPLATE_PATH}")

        project_text = CODEGEN_PROJECT_PATH.read_text(encoding="utf-8")
        self.assertIn("Scriban.csproj", project_text)

    def test_driver_emits_native_reference_skeleton_from_managed_closure_bundle(self) -> None:
        if OUTPUT_ROOT.exists():
            shutil.rmtree(OUTPUT_ROOT)

        self.assertTrue((MANAGED_CLOSURE_ROOT / "closure.manifest.json").is_file(), msg="missing Stage 3 closure bundle input")

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "--",
                "emit-native-reference",
                str(MANAGED_CLOSURE_ROOT),
                str(OUTPUT_ROOT),
            ],
            cwd=REPO_ROOT,
        )

        generated_cpp = OUTPUT_ROOT / "generated" / "native-reference.generated.cpp"
        manifest_path = OUTPUT_ROOT / "native-proof.manifest.json"
        plan_path = OUTPUT_ROOT / "native-proof.plan.json"

        self.assertTrue(generated_cpp.is_file(), msg=f"missing generated cpp: {generated_cpp}")
        self.assertTrue(manifest_path.is_file(), msg=f"missing native proof manifest: {manifest_path}")
        self.assertTrue(plan_path.is_file(), msg=f"missing native proof plan: {plan_path}")

        manifest = load_json(manifest_path)
        plan = load_json(plan_path)
        self.assertEqual("v0", manifest["formatVersion"])
        self.assertEqual("nativeReferenceProofManifest", manifest["artifactKind"])
        self.assertEqual("HelloWorldObject", manifest["assemblyName"])
        self.assertEqual("HelloWorldObject/Program::Main(System.String[])", manifest["entrySubjectId"])
        self.assertEqual("native-proof.plan.json", manifest["planArtifactPath"])
        self.assertEqual(
            "artifacts/proof/managed-closure/HelloWorldObject",
            manifest["managedClosureRootPath"],
        )
        self.assertEqual(
            ["generated/native-reference.generated.cpp"],
            [artifact["path"] for artifact in manifest["generatedArtifacts"]],
        )
        self.assertEqual("v0", plan["formatVersion"])
        self.assertEqual("nativeReferenceLoweringPlan", plan["artifactKind"])
        self.assertEqual("constructorThenInstanceCall", plan["planKind"])
        self.assertEqual("HelloWorldObject", plan["assemblyName"])
        self.assertEqual("HelloWorldObject/Program::Main(System.String[])", plan["entrySubjectId"])
        self.assertEqual("HelloWorldObject_Program_Main", plan["entrySymbol"])
        self.assertEqual("HelloWorldObject_Greeter__ctor", plan["constructorSymbol"])
        self.assertEqual("HelloWorldObject_Greeter_BuildMessage", plan["instanceMethodSymbol"])
        self.assertEqual('"World"', plan["constructorLiteral"])
        self.assertEqual('"Hello, "', plan["messagePrefixLiteral"])
        self.assertEqual('"!"', plan["messageSuffixLiteral"])

        generated_text = generated_cpp.read_text(encoding="utf-8")
        self.assertIn('#include "codegen_bridge.h"', generated_text)
        self.assertIn("bridge->register_codegen", generated_text)
        self.assertIn("bridge->bootstrap_runtime", generated_text)
        self.assertIn("CHAOS_RUNTIME_ABI_CALL", generated_text)
        self.assertIn("CHAOS_BRIDGE_STATUS_OK", generated_text)
        self.assertIn("HelloWorldObject_Program_Main", generated_text)
        self.assertIn("HelloWorldObject_Greeter__ctor", generated_text)
        self.assertIn("HelloWorldObject_Greeter_BuildMessage", generated_text)
        self.assertIn("struct NativeReferenceProofContext", generated_text)
        self.assertIn("resolve_type_by_token", generated_text)
        self.assertIn("resolve_method_by_token", generated_text)
        self.assertIn("resolve_field_by_token", generated_text)
        self.assertIn("bridge->resolve_icall", generated_text)
        self.assertIn("field_set_value", generated_text)
        self.assertIn("field_get_value", generated_text)
        self.assertIn("constructor_argument = g_proof_context.abi->string_new_utf8", generated_text)
        self.assertIn("message_prefix = g_proof_context.abi->string_new_utf8", generated_text)
        self.assertIn("message_suffix = g_proof_context.abi->string_new_utf8", generated_text)
        self.assertIn('"World"', generated_text)
        self.assertIn('"Hello, "', generated_text)
        self.assertIn('"!"', generated_text)
        self.assertIn("HelloWorldObject_Greeter__ctor(runtime, thread,", generated_text)
        self.assertIn("g_proof_context.write_line_string(runtime, thread, message)", generated_text)
        self.assertNotIn("const auto ctor_symbol = &HelloWorldObject_Greeter__ctor;", generated_text)
        self.assertNotIn("(void)ctor_symbol;", generated_text)
        self.assertNotIn("BO" + "OM_", generated_text)
        self.assertNotIn("method_invoke", generated_text)


if __name__ == "__main__":
    unittest.main()
