from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROGRAM_PATH = REPO_ROOT / "tests" / "proof" / "input" / "HelloWorldObject" / "Program.cs"
MANIFEST_PATH = REPO_ROOT / "tests" / "proof" / "hello-world-object" / "suite.manifest.json"
OVERVIEW_PATH = REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md"
SPEC_DOC_PATH = REPO_ROOT / "docs" / "architecture" / "roadmap-0" / "hello-world-proof-spec-v0.md"
TYPED_IL_EXAMPLE_PATH = REPO_ROOT / "contracts" / "examples" / "v0" / "artifacts" / "typed-il-ir.min.json"
SUITE_MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "suite_manifest.py"


def load_suite_manifest_module():
    if not SUITE_MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"suite manifest module missing: {SUITE_MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_suite_manifest_stage2", SUITE_MANIFEST_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load suite manifest module: {SUITE_MANIFEST_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class Stage2HelloWorldProofSpecTests(unittest.TestCase):
    def test_proof_program_uses_canonical_main_signature_and_world_constant(self) -> None:
        program_text = PROGRAM_PATH.read_text(encoding="utf-8")

        self.assertIn("private static int Main(string[] args)", program_text)
        self.assertIn('new Greeter("World")', program_text)
        self.assertIn("return 0;", program_text)
        self.assertNotIn('"object-world"', program_text)

    def test_proof_suite_manifest_remains_stage2_build_only(self) -> None:
        suite_manifest_module = load_suite_manifest_module()

        manifest = suite_manifest_module.load_suite_manifest(MANIFEST_PATH)

        self.assertEqual("proof", manifest.family)
        self.assertEqual("hello-world-object", manifest.suite_id)
        self.assertEqual("HelloWorldObject", manifest.display_name)
        self.assertEqual(["build"], manifest.supported_stages)
        self.assertIn("proof", manifest.tags)
        self.assertIn("object", manifest.tags)

    def test_spec_doc_exists_and_is_linked_from_overview(self) -> None:
        self.assertTrue(SPEC_DOC_PATH.is_file(), msg=f"missing spec doc: {SPEC_DOC_PATH}")

        spec_text = SPEC_DOC_PATH.read_text(encoding="utf-8")
        overview_text = OVERVIEW_PATH.read_text(encoding="utf-8")

        self.assertIn("Hello, World!", spec_text)
        self.assertIn("return 0", spec_text)
        self.assertIn("contracts/examples/v0/artifacts/typed-il-ir.min.json", spec_text)
        self.assertIn("contracts/native/examples/v0/bridge/direct-call-landing.cpp", spec_text)
        self.assertIn("docs/architecture/roadmap-0/hello-world-proof-spec-v0.md", overview_text)

    def test_typed_il_example_keeps_the_exclamation_in_build_message(self) -> None:
        payload = json.loads(TYPED_IL_EXAMPLE_PATH.read_text(encoding="utf-8"))
        methods = payload["methods"]
        build_message = next(
            method for method in methods if method["subjectId"] == "HelloWorldObject/Greeter::BuildMessage()"
        )
        instructions = build_message["blocks"][0]["instructions"]

        self.assertIn(
            {
                "op": "ldstr",
                "operand": "!",
                "resultType": "System.String",
            },
            instructions,
        )
        self.assertGreaterEqual(
            sum(
                1
                for instruction in instructions
                if instruction.get("callee") == "System.Private.CoreLib/System.String::Concat(System.String,System.String)"
            ),
            2,
        )


if __name__ == "__main__":
    unittest.main()
