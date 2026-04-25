from tests.unit.run.repo_layout_test_support import *


class RepoLayoutSubjectContractsTests(RepoLayoutTestSupport):
    def test_subject_manifests_keep_validation_expected_and_baselines_inside_subject_root(self) -> None:
        manifest_paths = sorted((REPO_ROOT / "verification" / "catalog" / "owners").rglob("owner.manifest.json"))
        self.assertGreater(len(manifest_paths), 0)

        for manifest_path in manifest_paths:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            subject_id = str(manifest.get("subjectId") or "")
            validation_profiles = dict(manifest.get("validationProfiles") or {})
            validation = dict(manifest.get("validation") or {})

            self.assertTrue(subject_id)
            self.assertEqual(subject_id, manifest_path.parent.name)
            self.assertTrue(str(manifest.get("defaultValidationProfile") or ""))
            self.assertIn(str(manifest["defaultValidationProfile"]), validation_profiles)

            for validation_spec in validation.values():
                project_path = str(dict(validation_spec).get("project") or "")
                if not project_path:
                    continue
                self.assertTrue(
                    project_path.startswith(f"verification/catalog/owners/{subject_id}/")
                    or project_path.startswith("src/tools/Chaos.IL2CPP.Tools.")
                )
                self.assertNotIn("/tests/", project_path.replace("\\", "/"))

            for expected_path in dict(manifest.get("expected") or {}).values():
                self.assertTrue(
                    str(expected_path).startswith(f"verification/evidence/owners/{subject_id}/expected/")
                )

            for baseline_path in dict(manifest.get("baselines") or {}).values():
                self.assertTrue(
                    str(baseline_path).startswith(f"verification/catalog/owners/{subject_id}/benchmark-baselines/")
                )

    def test_phase8_subject_root_tree_does_not_keep_legacy_redirect_shells(self) -> None:
        legacy_shells: list[str] = []
        for manifest_path in sorted((REPO_ROOT / "verification" / "catalog" / "owners").glob("*/owner.manifest.json")):
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            compatibility = dict(manifest.get("compatibility") or {})
            if (
                str(compatibility.get("redirectToSubject") or "").strip()
                or str(compatibility.get("redirectSubjectId") or "").strip()
                or str(compatibility.get("redirectSubjectPath") or "").strip()
            ):
                legacy_shells.append(str(manifest_path.parent.relative_to(REPO_ROOT).as_posix()))

        self.assertEqual([], legacy_shells)
        self.assertFalse((REPO_ROOT / "subjects" / "golden").exists())

    def test_root_tests_tree_does_not_keep_legacy_phase_or_stage_entries(self) -> None:
        offenders: list[str] = []
        for file_name in RETIRED_RUN_TEST_NAMES:
            path = REPO_ROOT / "tests" / "unit" / "run" / file_name
            if path.exists():
                offenders.append(str(path.relative_to(REPO_ROOT).as_posix()))

        self.assertEqual([], offenders)

    def test_contracts_dir_contains_only_formal_definitions(self) -> None:
        contracts_root = REPO_ROOT / "contracts"
        forbidden_subdirs = ["samples", "snapshots", "examples"]
        offenders: list[str] = []
        for subdir in contracts_root.rglob("*"):
            if subdir.is_dir() and subdir.name in forbidden_subdirs:
                offenders.append(str(subdir.relative_to(REPO_ROOT).as_posix()))

        self.assertEqual([], offenders, msg="contracts/ must only contain formal definitions, not concrete fixtures")
