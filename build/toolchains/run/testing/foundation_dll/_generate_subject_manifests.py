"""Generate owner.manifest.json for each foundation-dll family.

Scans verification/foundation-dll/<Assembly>/<Family>/capability-family-contract.json
and creates verification/catalog/owners/foundation-dll-<assembly>-<family>/owner.manifest.json.

Run:  py -3 -m testing.foundation_dll._generate_subject_manifests
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]  # build/toolchains/run/testing/foundation_dll -> repo root

FOUNDATION_DLL_BASE = _REPO_ROOT / "verification" / "foundation-dll"
CATALOG_OWNERS = _REPO_ROOT / "verification" / "catalog" / "owners"

MANIFEST_TEMPLATE = {
    "subjectId": None,  # filled per family
    "displayName": None,  # filled per family
    "category": "foundation-dll-family",
    "defaultGoal": "correctness.dev",
    "defaultMatrix": "windows-verify",
    "defaultValidationProfile": "verify-dev",
    "source": {
        "type": "foundation-dll-family",
        "path": "family",  # registry requires a non-empty path
        "primaryProjectPath": "family",  # registry requires primaryProjectPath for all subjects
        "familySlug": None,  # filled per family
        "assembly": None,  # filled per family
    },
    "validationProfiles": {
        "verify-dev": ["verify"],
    },
    "validation": {
        "verify": {
            "kind": "proof",
            "defaultVariant": "CHECK",
        },
    },
    "tags": [
        "foundation-dll",
        "family",
    ],
    "executionPipelines": [
        {
            "pipelineId": "family-verify",
            "displayName": "Foundation-DLL Family Verification",
            "stages": [
                {
                    "stageId": "family-verify",
                    "kind": "family-verify-foundation-dll",
                    "scope": "matrix",
                    "bucket": "report",
                    "dependsOn": [],
                },
            ],
        },
    ],
    "environmentMatrices": [
        {
            "matrixId": "windows-verify",
            "pipelineId": "family-verify",
            "supportedGoals": ["correctness.dev"],
            "executionContext": {
                "hostPlatform": "windows",
                "targetPlatform": "windows",
                "toolchainProfile": "native-aot",
                "runtimeProfile": "native",
            },
            "validationIntent": {
                "validationMode": "report",
                "adaptationLevel": "full",
                "expectedOutcome": "pass",
            },
            "artifactPlan": {
                "requiredBuckets": ["report"],
                "evidenceTerminalBucket": "report",
            },
        },
    ],
}


def sanitize_subject_id(assembly: str, family_slug: str) -> str:
    """Create a filesystem-safe subject ID."""
    return f"foundation-dll-{assembly}-{family_slug}"


def generate_manifests(dry_run: bool = False) -> list[Path]:
    if not FOUNDATION_DLL_BASE.is_dir():
        print(f"ERROR: foundation-dll base not found: {FOUNDATION_DLL_BASE}")
        return []

    generated: list[Path] = []

    for assembly_dir in sorted(FOUNDATION_DLL_BASE.iterdir()):
        if not assembly_dir.is_dir():
            continue
        assembly = assembly_dir.name

        for family_dir in sorted(assembly_dir.iterdir()):
            if not family_dir.is_dir():
                continue
            contract_path = family_dir / "capability-family-contract.json"
            if not contract_path.is_file():
                continue

            family_slug = family_dir.name

            # Skip families whose slug contains commas (composite families)
            if "," in family_slug:
                print(f"  SKIP (composite): {assembly}/{family_slug}")
                continue

            subject_id = sanitize_subject_id(assembly, family_slug)
            display_name = contract_path.read_text(encoding="utf-8")
            try:
                contract = json.loads(display_name)
                display_name = contract.get("displayName") or subject_id
            except (json.JSONDecodeError, ValueError):
                display_name = subject_id

            manifest = dict(MANIFEST_TEMPLATE)
            manifest["subjectId"] = subject_id
            manifest["displayName"] = str(display_name)
            manifest["source"] = {
                "type": "foundation-dll-family",
                "path": f"verification/foundation-dll/{assembly}/{family_slug}",
                "primaryProjectPath": f"verification/foundation-dll/{assembly}/{family_slug}",
                "familySlug": family_slug,
                "assembly": assembly,
            }

            owner_dir = CATALOG_OWNERS / subject_id
            manifest_path = owner_dir / "owner.manifest.json"

            if dry_run:
                print(f"  WOULD CREATE: {manifest_path}")
                generated.append(manifest_path)
                continue

            owner_dir.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text(
                json.dumps(manifest, indent=2, ensure_ascii=False) + "\n",
                encoding="utf-8",
            )
            print(f"  CREATED: {manifest_path}")
            generated.append(manifest_path)

    return generated


def main() -> int:
    print("Generating foundation-dll family subject manifests...")
    print(f"  Source: {FOUNDATION_DLL_BASE}")
    print(f"  Target: {CATALOG_OWNERS}")
    print()

    paths = generate_manifests(dry_run="--dry-run" in sys.argv)

    print(f"\nDone. {len(paths)} manifests generated.")
    if paths:
        print(f"\nTo verify a family via subject framework:")
        print(f"  run test --subject {Path(paths[0]).parent.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
