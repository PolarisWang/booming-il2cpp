from __future__ import annotations

import json

from tests.unit.performance.testing_inventory_test_support import (
    cleanup_repo_root,
    inventory_fixture,
    load_inventory_generator_module,
    make_temp_repo_root,
    write_inventory_fixture_repo,
)


def test_write_inventory_outputs_also_materializes_verification_formal_source() -> None:
    generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_verification_cutover")
    fixture = inventory_fixture()
    repo_root = make_temp_repo_root("verification", "inventory-cutover")
    write_inventory_fixture_repo(repo_root, fixture)

    try:
        payload = generator_module.write_inventory_outputs(
            repo_root,
            host_platform="windows",
            output_root=repo_root / "docs" / "testing-inventory",
        )

        assert (repo_root / "docs" / "testing-inventory" / "verification" / "latest" / "result-snapshot.json").is_file()
        assert (
            repo_root / "docs" / "testing-inventory" / "verification" / "latest" / "evidence-claims-snapshot.json"
        ).is_file()
        assert (repo_root / "docs" / "testing-inventory" / "verification" / "master" / "result-master.json").is_file()
        assert (
            repo_root / "docs" / "testing-inventory" / "verification" / "master" / "evidence-claims-master.json"
        ).is_file()
        assert (
            repo_root
            / "docs"
            / "testing-inventory"
            / "verification"
            / "reports"
            / "completed"
            / "testing-inventory"
            / "summary.md"
        ).is_file()
        assert "docs/testing-inventory/verification/latest/result-snapshot.json" in payload["artifacts"]
        assert "verification" in payload
        assert payload["verification"]["outputRoot"] == "docs/testing-inventory/verification"
        assert payload["validated"] is True
        assert payload["verificationData"]["latestResultPath"] == "docs/testing-inventory/verification/latest/result-snapshot.json"
        assert payload["verificationData"]["masterResultPath"] == "docs/testing-inventory/verification/master/result-master.json"
        assert payload["verificationData"]["reportSummaryPath"] == (
            "docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md"
        )
        assert payload["sourceSummary"]["codegenStubCount"] == 0
        assert payload["verificationData"]["codegenStubPaths"] == []
    finally:
        cleanup_repo_root(repo_root)


def test_write_inventory_outputs_projects_capability_table_from_merged_formal_master() -> None:
    generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_verification_merged_master")
    fixture = inventory_fixture()
    repo_root = make_temp_repo_root("verification", "inventory-merged-master")
    write_inventory_fixture_repo(repo_root, fixture)

    master_root = repo_root / "docs" / "testing-inventory" / "verification" / "master"
    master_root.mkdir(parents=True, exist_ok=True)
    (master_root / "capability-master.json").write_text(
        json.dumps(
            {
                "schemaVersion": 1,
                "generatedAtUtc": "2026-04-22T07:00:00Z",
                "producerId": "verification-v1",
                "sourceSpecVersion": "verification-v1",
                "mergeKey": "capabilityId",
                "items": [
                    {
                        "capabilityId": "capability/42/88",
                        "displayName": "Legacy Capability",
                        "ownerSubjectId": "LegacyPack",
                        "defaultRoutes": ["managed"],
                        "defaultPlatforms": ["windows-x64"],
                        "defaultDeviceProfiles": ["bridge/windows-x64/legacy-device"],
                        "requiredEvidenceKinds": ["contract"],
                        "verificationState": "planned",
                        "supportState": "supported",
                        "blockers": [],
                        "latestClosureId": "closure/completed/testing-inventory",
                        "latestObligationClaimIds": ["obligation/42/88"],
                        "projectionMeta": {
                            "ownerSubjectId": "LegacyPack",
                            "capabilityFamily": 42,
                            "capabilityFamilyLabel": "Legacy Family",
                            "capabilityItem": 88,
                            "capabilityItemLabel": "Legacy Capability",
                            "supportStates": [1],
                            "supportStateLabels": ["NativeGenerated"],
                            "proofRequired": True,
                            "benchmarkRequired": False,
                            "declaredUnitCount": 1,
                            "declaredBenchmarkCount": 0,
                            "contractStatus": "ok",
                            "statusReason": "Merged from previous formal master.",
                            "scope": "capability",
                            "rowKey": "capability/42/88",
                        },
                    }
                ],
            },
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )

    try:
        generator_module.write_inventory_outputs(
            repo_root,
            host_platform="windows",
            output_root=repo_root / "docs" / "testing-inventory",
        )

        capability_payload = json.loads(
            (repo_root / "docs" / "testing-inventory" / "capability-inventory.json").read_text(encoding="utf-8")
        )
        capability_items = {row["capabilityItem"]: row for row in capability_payload["rows"]}
        assert len(capability_payload["rows"]) == 3
        assert capability_items[88]["ownerSubjectId"] == "LegacyPack"
        assert capability_items[88]["statusReason"] == "Merged from previous formal master."
    finally:
        cleanup_repo_root(repo_root)


def test_write_inventory_outputs_renders_fixed_homepage_views_from_formal_master() -> None:
    generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_verification_fixed_views")
    fixture = inventory_fixture()
    repo_root = make_temp_repo_root("verification", "inventory-fixed-views")
    write_inventory_fixture_repo(repo_root, fixture)

    try:
        generator_module.write_inventory_outputs(
            repo_root,
            host_platform="windows",
            output_root=repo_root / "docs" / "testing-inventory",
        )

        html_document = (repo_root / "docs" / "testing-inventory" / "inventory.html").read_text(encoding="utf-8")

        assert "Closure Strip" in html_document
        assert "Responsibility View" in html_document
        assert "Route Topology View" in html_document
        assert "Platform-Device Matrix" in html_document
        assert html_document.index("Closure Strip") < html_document.index("Capability Inventory")
    finally:
        cleanup_repo_root(repo_root)


def test_validate_inventory_outputs_fails_when_required_formal_artifact_is_missing() -> None:
    generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_verification_validation")
    repo_root = make_temp_repo_root("verification", "inventory-validation-failure")

    try:
        (repo_root / "docs" / "testing-inventory" / "verification" / "master").mkdir(parents=True, exist_ok=True)
        (repo_root / "docs" / "testing-inventory" / "verification" / "reports" / "completed" / "testing-inventory").mkdir(
            parents=True,
            exist_ok=True,
        )
        (repo_root / "docs" / "testing-inventory" / "verification" / "master" / "result-master.json").write_text(
            "{}",
            encoding="utf-8",
        )
        (repo_root / "docs" / "testing-inventory" / "verification" / "reports" / "completed" / "testing-inventory" / "summary.md").write_text(
            "# summary\n",
            encoding="utf-8",
        )

        try:
            generator_module.validate_inventory_outputs(
                repo_root,
                {
                    "outputRoot": "docs/testing-inventory",
                    "artifacts": [
                        "docs/testing-inventory/verification/master/result-master.json",
                        "docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md",
                    ],
                    "verification": {
                        "outputRoot": "docs/testing-inventory/verification",
                    },
                },
            )
            raise AssertionError("validate_inventory_outputs should fail when latest result is missing")
        except RuntimeError as error:
            assert "verification/latest/result-snapshot.json" in str(error)
    finally:
        cleanup_repo_root(repo_root)


def test_validate_inventory_outputs_collects_codegen_stub_paths_when_required() -> None:
    generator_module = load_inventory_generator_module("chaos_testing_inventory_generator_verification_codegen_stub_paths")
    repo_root = make_temp_repo_root("verification", "inventory-codegen-stub-paths")

    try:
        (repo_root / "docs" / "testing-inventory" / "verification" / "latest").mkdir(parents=True, exist_ok=True)
        (repo_root / "docs" / "testing-inventory" / "verification" / "master").mkdir(parents=True, exist_ok=True)
        (repo_root / "docs" / "testing-inventory" / "verification" / "reports" / "completed" / "testing-inventory").mkdir(
            parents=True,
            exist_ok=True,
        )
        (
            repo_root
            / "subjects"
            / "FixtureSubject"
            / "verification"
            / "codegen-stubs"
            / "capability"
            / "7"
            / "31"
            / "native"
        ).mkdir(parents=True, exist_ok=True)

        (repo_root / "docs" / "testing-inventory" / "verification" / "latest" / "result-snapshot.json").write_text(
            "{}",
            encoding="utf-8",
        )
        (repo_root / "docs" / "testing-inventory" / "verification" / "master" / "result-master.json").write_text(
            "{}",
            encoding="utf-8",
        )
        (
            repo_root / "docs" / "testing-inventory" / "verification" / "reports" / "completed" / "testing-inventory" / "summary.md"
        ).write_text(
            "# summary\n",
            encoding="utf-8",
        )
        (
            repo_root
            / "subjects"
            / "FixtureSubject"
            / "verification"
            / "codegen-stubs"
            / "capability"
            / "7"
            / "31"
            / "native"
            / "stub-index.json"
        ).write_text(
            "{}",
            encoding="utf-8",
        )

        payload = generator_module.validate_inventory_outputs(
            repo_root,
            {
                "outputRoot": "docs/testing-inventory",
                "artifacts": [
                    "docs/testing-inventory/verification/latest/result-snapshot.json",
                    "docs/testing-inventory/verification/master/result-master.json",
                    "docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md",
                    "subjects/FixtureSubject/verification/codegen-stubs/capability/7/31/native/stub-index.json",
                ],
                "verification": {
                    "outputRoot": "docs/testing-inventory/verification",
                },
            },
            required_codegen_stub_count=1,
        )

        assert payload["validated"] is True
        assert payload["verificationData"]["codegenStubPaths"] == [
            "subjects/FixtureSubject/verification/codegen-stubs/capability/7/31/native/stub-index.json"
        ]
    finally:
        cleanup_repo_root(repo_root)
