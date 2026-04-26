from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
DERIVE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "derive.py"


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def test_generate_candidate_ledger_uses_authority_inputs_and_demotes_status() -> None:
    derive_module = load_module(DERIVE_MODULE_PATH, "chaos_foundation_dll_denominator_derive")
    repo_root = make_temp_repo_root("verification", "foundation-dll-denominator-derive")

    try:
        write_json(
            repo_root / "verification" / "catalog" / "programs" / "foundation-dll-translation-audit.program.json",
            {
                "assemblies": [
                    {"assemblyName": "System.Private.CoreLib"},
                    {"assemblyName": "System.Collections.Immutable"},
                ]
            },
        )
        write_json(
            repo_root / "docs" / "dev" / "completed" / "20260418-03-phase-1-contract-facade-and-shim-certification-lane" / "public-surface-manifest-v1-01.json",
            {
                "targetFrameworks": {
                    "net10.0": {
                        "assemblies": {
                            "System.Collections.Immutable": {
                                "publicTypeDefinitionCount": 36,
                                "publicMemberCount": 240,
                                "publicTypes": {
                                    "System.Collections.Immutable.ImmutableArray": {},
                                    "System.Collections.Immutable.ImmutableDictionary": {},
                                },
                            }
                        }
                    }
                }
            },
        )
        write_json(
            repo_root / "docs" / "dev" / "completed" / "20260418-05-phase-3-system-private-corelib-semantic-family-nativeization" / "semantic-family-plan-v1-01.json",
            {
                "targetFrameworks": {
                    "net10.0": {
                        "familyCount": 6,
                        "families": {"managed-cil": {"memberCount": 10}},
                    }
                }
            },
        )
        write_json(
            repo_root / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json",
            {
                "schemaVersion": "1.0",
                "snapshotId": "snap-formal",
                "snapshotAt": "2026-04-26T00:00:00+08:00",
                "authoritySnapshotOf": "foundation-dll-translation-audit",
                "denominatorStrategy": "capability-family",
                "denominatorStatus": "audit-confirmed",
                "dlls": [
                    {
                        "assemblyName": "System.Private.CoreLib",
                        "denominatorStatus": "audit-confirmed",
                        "families": [
                            {
                                "familyId": "family/System.Private.CoreLib/convert/char",
                                "displayName": "Convert.Char Conversions",
                                "denominatorStatus": "audit-confirmed",
                                "closureStatus": "in-progress",
                                "verificationGates": {"audit-input-and-ledger": "passed"},
                            }
                        ],
                    },
                    {
                        "assemblyName": "System.Collections.Immutable",
                        "denominatorStatus": "audit-confirmed",
                        "families": [
                            {
                                "familyId": "family/System.Collections.Immutable/immutable-array",
                                "displayName": "Immutable Array",
                                "denominatorStatus": "audit-confirmed",
                                "closureStatus": "in-progress",
                                "verificationGates": {"audit-input-and-ledger": "pending"},
                            }
                        ],
                    },
                ],
            },
        )

        result = derive_module.generate_candidate_ledger(repo_root, scope="all")

        candidate_path = repo_root / result["candidateLedgerPath"]
        payload = json.loads(candidate_path.read_text(encoding="utf-8"))
        assert payload["denominatorStatus"] == "candidate-derived"
        assert payload["authorityInputs"]["programManifest"].endswith("foundation-dll-translation-audit.program.json")
        assert payload["authorityInputs"]["surfaceManifest"].endswith("public-surface-manifest-v1-01.json")
        assert payload["authorityInputs"]["semanticFamilyPlan"].endswith("semantic-family-plan-v1-01.json")
        dlls = {entry["assemblyName"]: entry for entry in payload["dlls"]}
        assert dlls["System.Private.CoreLib"]["denominatorStatus"] == "candidate-derived"
        assert dlls["System.Private.CoreLib"]["authorityInputs"]["semanticFamilyCount"] == 6
        assert dlls["System.Collections.Immutable"]["denominatorStatus"] == "candidate-derived"
        assert dlls["System.Collections.Immutable"]["authorityInputs"]["surfacePublicTypeCount"] == 36
        assert dlls["System.Collections.Immutable"]["families"][0]["denominatorStatus"] == "candidate-derived"
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
