from tests.tooling.run.command_manifest_test_support import *


class CommandManifestCliTests(CommandManifestTestSupport):
    def test_parse_cli_supports_dynamic_unified_test_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        smoke_spec = select_public_suite_spec(
            "chaos_manifest_parse_cli_smoke",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "run"],
        )
        family = str(smoke_spec["family"])
        suite_name = str(smoke_spec["suite"])
        suite_id = str(smoke_spec["id"])
        subject_id = "FixtureSubject"

        suite = manifest_module.parse_cli(["test", family, suite_name], False, manifest, "macos")
        self.assertEqual("test-family-suite", suite["command"]["id"])
        self.assertEqual(suite_id, suite["target"])
        self.assertEqual(family, suite["options"]["family"])
        self.assertEqual(suite_name, suite["options"]["suite"])

        explicit_suite = manifest_module.parse_cli(
            ["test", "suite", "--family", family, "--suite", suite_name],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-suite", explicit_suite["command"]["id"])
        self.assertEqual(suite_id, explicit_suite["target"])
        self.assertEqual(family, explicit_suite["options"]["family"])
        self.assertEqual(suite_name, explicit_suite["options"]["suite"])

        explicit_suite_id = manifest_module.parse_cli(
            ["test", "suite", "--id", suite_id],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-suite", explicit_suite_id["command"]["id"])
        self.assertEqual(suite_id, explicit_suite_id["target"])
        self.assertEqual(suite_id, explicit_suite_id["options"]["id"])

        subject_case = manifest_module.parse_cli(
            ["test", "subject", "--id", f"subject/{subject_id}"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-subject", subject_case["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", subject_case["target"])
        self.assertEqual(f"subject/{subject_id}", subject_case["options"]["id"])

        managed_closure_contract = manifest_module.parse_cli(
            ["test", "contract", "managed-closure-bundle"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-family-suite", managed_closure_contract["command"]["id"])
        self.assertEqual("contract/managed-closure-bundle", managed_closure_contract["target"])
        self.assertEqual("contract", managed_closure_contract["options"]["family"])
        self.assertEqual("managed-closure-bundle", managed_closure_contract["options"]["suite"])

        module_case = manifest_module.parse_cli(
            ["test", "module", "--module", "managed-smoke", "--profile", "basic"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-module", module_case["command"]["id"])
        self.assertEqual("module/managed-smoke/basic", module_case["target"])
        self.assertEqual("managed-smoke", module_case["options"]["module"])
        self.assertEqual("basic", module_case["options"]["profile"])

        system_case = manifest_module.parse_cli(
            ["test", "system", "--id", "system/hosted-runtime-smoke"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-system", system_case["command"]["id"])
        self.assertEqual("system/hosted-runtime-smoke", system_case["target"])

        inventory_case = manifest_module.parse_cli(
            ["test", "inventory", "--output", "verification/projections/testing-inventory"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-inventory", inventory_case["command"]["id"])
        self.assertEqual("testing-inventory", inventory_case["target"])
        self.assertEqual("verification/projections/testing-inventory", inventory_case["options"]["output"])

        removed_verification = manifest_module.parse_cli(
            ["verify", "verification-v1", "--output", "verification/projections/testing-inventory"],
            False,
            manifest,
            "windows",
        )
        self.assertIsNone(removed_verification["command"])

        registry_refresh = manifest_module.parse_cli(["test", "registry", "refresh"], False, manifest, "macos")
        self.assertEqual("test-registry-refresh", registry_refresh["command"]["id"])

        registry_list = manifest_module.parse_cli(["test", "registry", "list"], False, manifest, "macos")
        self.assertEqual("test-registry-list", registry_list["command"]["id"])

        registry_check = manifest_module.parse_cli(
            ["test", "registry", "check-consistency"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-registry-check-consistency", registry_check["command"]["id"])

        list_family = manifest_module.parse_cli(["test", "list", "smoke"], False, manifest, "macos")
        self.assertEqual("test-list", list_family["command"]["id"])
        self.assertEqual("smoke", list_family["target"])
        self.assertEqual("smoke", list_family["options"]["family"])

        watch = manifest_module.parse_cli(["test", "watch"], False, manifest, "macos")
        self.assertEqual("test-watch", watch["command"]["id"])

        summary = manifest_module.parse_cli(["test", "summary"], False, manifest, "macos")
        self.assertEqual("test-summary", summary["command"]["id"])

        removed_pipeline = manifest_module.parse_cli(
            ["test", "pipeline", "--pipeline", "completion-runtime-core"],
            False,
            manifest,
            "macos",
        )
        self.assertIsNone(removed_pipeline["command"])
        self.assertEqual("test pipeline", removed_pipeline["command_text"])

    def test_parse_cli_rejects_removed_verify_entrypoint(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        parsed = manifest_module.parse_cli(["verify", "roadmap-0", "--host", "macos"], False, manifest, "macos")

        self.assertIsNone(parsed["command"])
        self.assertEqual("verify roadmap-0", parsed["command_text"])

    def test_parse_cli_supports_runtime_baseline_prepare_and_workflow_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        prepare_runtime_baseline = manifest_module.parse_cli(
            ["prepare", "workflow", "runtime-baseline", "--host", "macos"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("prepare-workflow-runtime-baseline-macos", prepare_runtime_baseline["command"]["id"])
        self.assertEqual("macos", prepare_runtime_baseline["options"]["host"])

        prepare_android_host = manifest_module.parse_cli(
            ["prepare", "android-host"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("prepare-android-host", prepare_android_host["command"]["id"])

        workflow_runtime_baseline = manifest_module.parse_cli(
            ["test", "workflow", "runtime-baseline-macos"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-family-suite", workflow_runtime_baseline["command"]["id"])
        self.assertEqual("workflow/runtime-baseline-macos", workflow_runtime_baseline["target"])

    def test_parse_cli_supports_project_and_deploy_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_id = "FixtureSubject"

        generate_all = manifest_module.parse_cli(
            ["generate", "project", "all", "--host", "windows", "--refresh-generated"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-all", generate_all["command"]["id"])
        self.assertEqual("windows", generate_all["options"]["host"])
        self.assertTrue(generate_all["options"]["refresh-generated"])

        generate_subject = manifest_module.parse_cli(
            [
                "generate",
                "project",
                "subject",
                "--id",
                f"subject/{subject_id}",
                "--open-native-target",
                "generated",
            ],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-subject", generate_subject["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", generate_subject["options"]["id"])
        self.assertEqual("generated", generate_subject["options"]["open-native-target"])

        generate_core = manifest_module.parse_cli(
            ["generate", "project", "core", "--host", "windows", "--all-targets"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-core", generate_core["command"]["id"])
        self.assertEqual("windows", generate_core["options"]["host"])
        self.assertTrue(generate_core["options"]["all-targets"])

        build_subject = manifest_module.parse_cli(
            [
                "build",
                "project",
                "subject",
                "--id",
                f"subject/{subject_id}",
                "--matrix",
                "windows-managed-trace",
                "--native-target",
                "proof",
            ],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("build-project-subject", build_subject["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", build_subject["options"]["id"])
        self.assertEqual("windows-managed-trace", build_subject["options"]["matrix"])
        self.assertEqual("proof", build_subject["options"]["native-target"])

        build_core = manifest_module.parse_cli(
            ["build", "project", "core", "--host", "windows", "--target", "windows-x64-reference"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("build-project-core", build_core["command"]["id"])
        self.assertEqual("windows-x64-reference", build_core["options"]["target"])

        deploy_core = manifest_module.parse_cli(
            ["deploy", "core", "--host", "windows", "--all-targets"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("deploy-core", deploy_core["command"]["id"])
        self.assertTrue(deploy_core["options"]["all-targets"])
