from tests.unit.run.repo_layout_test_support import *


class RepoLayoutPythonToolingTests(RepoLayoutTestSupport):
    def test_subject_native_reference_cmake_treats_generated_cpp_as_existing_input(self) -> None:
        native_reference_cmake = (
            REPO_ROOT
            / "build"
            / "toolchains"
            / "run"
            / "subject"
            / "templates"
            / "native-generated.cmake.tmpl"
        ).read_text(encoding="utf-8")

        self.assertIn("if(NOT CHAOS_SUBJECT_GENERATED_INPUT_SOURCES)", native_reference_cmake)
        self.assertIn("foreach(chaos_generated_source IN LISTS CHAOS_SUBJECT_GENERATED_INPUT_SOURCES)", native_reference_cmake)
        self.assertIn('if(NOT EXISTS "${chaos_generated_source}")', native_reference_cmake)
        self.assertNotIn("GENERATED TRUE", native_reference_cmake)

    def test_python_tests_do_not_hardcode_user_specific_temp_paths(self) -> None:
        offenders: list[str] = []
        for test_path in sorted((REPO_ROOT / "tests").rglob("*.py")):
            content = test_path.read_text(encoding="utf-8")
            for snippet in FORBIDDEN_TEST_PATH_SNIPPETS:
                if snippet in content:
                    offenders.append(f"{test_path.relative_to(REPO_ROOT).as_posix()}: {snippet}")

        self.assertEqual([], offenders)

    def test_active_python_sources_do_not_use_chaos_prefix(self) -> None:
        search_roots = [
            REPO_ROOT / "build" / "toolchains" / "run",
            REPO_ROOT / "tests",
        ]
        offenders: list[str] = []

        for root in search_roots:
            for source_path in sorted(root.rglob("*.py")):
                if "__pycache__" in source_path.parts:
                    continue

                content = source_path.read_text(encoding="utf-8")
                for snippet in FORBIDDEN_ACTIVE_PREFIX_SNIPPETS:
                    if snippet in content:
                        offenders.append(f"{source_path.relative_to(REPO_ROOT).as_posix()}: {snippet}")

        self.assertEqual([], offenders)

    def test_run_tooling_uses_domain_based_core_layout(self) -> None:
        run_root = REPO_ROOT / "build" / "toolchains" / "run"
        core_root = run_root / "core"
        subject_root = run_root / "subject"

        self.assertTrue(core_root.is_dir(), msg="build/toolchains/run/core/ must exist")
        self.assertTrue(subject_root.is_dir(), msg="build/toolchains/run/subject/ must exist")
        self.assertTrue((core_root / "common.py").is_file())
        self.assertTrue((core_root / "manifest.py").is_file())
        self.assertTrue((core_root / "result.py").is_file())
        self.assertTrue((core_root / "tooling.py").is_file())
        self.assertTrue((subject_root / "project_workspace.py").is_file())
        self.assertFalse((run_root / "common.py").exists(), msg="common.py must be in core/")
        self.assertFalse((run_root / "manifest.py").exists(), msg="manifest.py must be in core/")
        self.assertFalse((run_root / "result.py").exists(), msg="result.py must be in core/")
        self.assertFalse((run_root / "tooling.py").exists(), msg="tooling.py must be in core/")
        self.assertFalse((run_root / "project_workspace.py").exists(), msg="project_workspace.py must be in subject/")
