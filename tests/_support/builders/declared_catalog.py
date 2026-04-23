from __future__ import annotations


def declared_catalog_fixture(subject_id: str = "FixtureSubject") -> dict[str, object]:
    return {
        "subjectId": subject_id,
        "frameworkReferenced": True,
        "subjectKind": "declared-test",
        "warningCodes": [],
        "declaredUnitTests": [
            {
                "stableId": f"{subject_id}::FixtureSubject::FixtureSubject.Proofs::Verify()",
                "entryIndex": 0,
                "alias": "fixture-proof",
                "assemblyName": subject_id,
                "declaringType": "FixtureSubject.Proofs",
                "methodName": "Verify",
                "methodSignature": "Verify()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "evidence": 1,
                "priority": 1,
            }
        ],
        "declaredBenchmarks": [
            {
                "stableId": f"{subject_id}::FixtureSubject::FixtureSubject.Benchmarks::Run()",
                "entryIndex": 0,
                "alias": "fixture-benchmark",
                "assemblyName": subject_id,
                "declaringType": "FixtureSubject.Benchmarks",
                "methodName": "Run",
                "methodSignature": "Run()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "metrics": 1,
                "modes": 1,
                "warmupCount": 2,
                "iterationCount": 5,
                "invocationCount": 10,
            }
        ],
    }


def hotupdate_declared_catalog_fixture(subject_id: str = "FixtureHotUpdateSubject") -> dict[str, object]:
    patch_assembly_name = f"{subject_id}.Patch"
    return {
        "subjectId": subject_id,
        "frameworkReferenced": True,
        "subjectKind": "declared-test",
        "warningCodes": [],
        "declaredUnitTests": [
            {
                "stableId": f"{subject_id}::{patch_assembly_name}::{patch_assembly_name}.Proofs::Verify()",
                "entryIndex": 0,
                "alias": "hotupdate-proof",
                "assemblyName": patch_assembly_name,
                "declaringType": f"{patch_assembly_name}.Proofs",
                "methodName": "Verify",
                "methodSignature": "Verify()",
                "category": 5,
                "capabilityFamily": 13,
                "capabilityItem": 54,
                "archetype": 8,
                "hotUpdateCapability": 1,
                "requires": 32,
                "evidence": 1,
                "priority": 1,
            }
        ],
        "declaredBenchmarks": [
            {
                "stableId": f"{subject_id}::{patch_assembly_name}::{patch_assembly_name}.Benchmarks::RunWorkload()",
                "entryIndex": 0,
                "alias": "hotupdate-benchmark",
                "assemblyName": patch_assembly_name,
                "declaringType": f"{patch_assembly_name}.Benchmarks",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
                "category": 4,
                "capabilityFamily": 13,
                "capabilityItem": 54,
                "archetype": 8,
                "hotUpdateCapability": 1,
                "requires": 32,
                "metrics": 1,
                "modes": 5,
                "warmupCount": 1,
                "iterationCount": 5,
                "invocationCount": 1,
            }
        ],
    }
