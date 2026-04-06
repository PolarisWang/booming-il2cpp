from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
EXECUTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_executor.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class SubjectExecutorTests(unittest.TestCase):
    def test_executor_skips_reused_stage_and_runs_bucket_local_workers(self) -> None:
        executor_module = load_module(EXECUTOR_MODULE_PATH, "booming_subject_executor")
        worker_calls: list[dict] = []

        def analysis_worker(*, repo_root: Path, request: dict) -> dict:
            worker_calls.append({"kind": request["stage"]["kind"], "request": request})
            manifest_path = repo_root / request["paths"]["manifestPath"]
            manifest_path.parent.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text("{}", encoding="utf-8")
            report_path = repo_root / request["paths"]["reportPaths"][0]
            report_path.write_text("{}", encoding="utf-8")
            evidence_path = repo_root / request["paths"]["bucketRoot"] / "typed-il-ir.json"
            evidence_path.write_text("{}", encoding="utf-8")
            return {
                "status": "ok",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": list(request["paths"]["reportPaths"]),
                "primaryEvidencePaths": [str(evidence_path.relative_to(repo_root).as_posix())],
                "metrics": {"durationMs": 7},
                "diagnostics": {"stdoutPath": None, "stderrPath": None},
                "failure": None,
            }

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            source_manifest = repo_root / "artifacts" / "subjects" / "HelloWorldObject" / "shared" / "source" / "source.manifest.json"
            source_manifest.parent.mkdir(parents=True, exist_ok=True)
            source_manifest.write_text("{}", encoding="utf-8")

            plan = {
                "planVersion": "v1",
                "selection": {
                    "subjectId": "HelloWorldObject",
                    "goalId": "correctness.platform",
                    "matrixId": "windows-reference-trace",
                    "artifactPlan": {"evidenceTerminalBucket": "runtime"},
                },
                "stagePlan": [
                    {
                        "order": 1,
                        "stageId": "source-resolve",
                        "kind": "source-resolve",
                        "scope": "shared",
                        "bucket": "source",
                        "dependsOn": [],
                        "executionMode": "reused",
                        "fingerprint": "f-source",
                        "upstreamFingerprints": {},
                        "reuse": {"decision": "match", "reason": "fingerprint-match"},
                        "paths": {
                            "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/source",
                            "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
                            "reportPaths": [],
                        },
                    },
                    {
                        "order": 2,
                        "stageId": "analysis-frontend",
                        "kind": "analysis-frontend",
                        "scope": "shared",
                        "bucket": "analysis",
                        "dependsOn": ["source-resolve"],
                        "executionMode": "executed",
                        "fingerprint": "f-analysis",
                        "upstreamFingerprints": {"source": "f-source"},
                        "reuse": {"decision": "absent", "reason": "manifest-missing"},
                        "paths": {
                            "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis",
                            "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
                            "reportPaths": [
                                "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
                            ],
                        },
                    },
                    {
                        "order": 3,
                        "stageId": "report-assemble",
                        "kind": "report-assemble",
                        "scope": "matrix",
                        "bucket": "report",
                        "dependsOn": ["analysis-frontend"],
                        "executionMode": "executed",
                        "fingerprint": "f-report",
                        "upstreamFingerprints": {"analysis": "f-analysis"},
                        "reuse": {"decision": "absent", "reason": "manifest-missing"},
                        "paths": {
                            "bucketRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace",
                            "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json",
                            "reportPaths": [],
                        },
                    },
                ],
            }

            result = executor_module.execute_plan(
                repo_root,
                plan,
                worker_registry={"analysis-frontend": analysis_worker},
            )

            self.assertEqual("ok", result["status"])
            self.assertEqual("runtime", result["terminalBucket"])
            self.assertEqual(["source-resolve", "analysis-frontend", "report-assemble"], [stage["stageId"] for stage in result["stageResults"]])
            self.assertEqual("reused", result["stageResults"][0]["actionTaken"])
            self.assertEqual("executed", result["stageResults"][1]["actionTaken"])
            self.assertEqual("executed", result["stageResults"][2]["actionTaken"])
            self.assertEqual(1, len(worker_calls))
            self.assertEqual("analysis-frontend", worker_calls[0]["kind"])
            self.assertEqual(
                "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
                worker_calls[0]["request"]["upstream"]["source"]["manifestPath"],
            )

            report_path = repo_root / "artifacts" / "subjects" / "HelloWorldObject" / "matrices" / "windows-reference-trace" / "report.json"
            self.assertTrue(report_path.is_file(), msg=f"missing matrix report: {report_path}")
            report = json.loads(report_path.read_text(encoding="utf-8"))
            self.assertEqual("windows-reference-trace", report["matrixId"])
            self.assertEqual("runtime", report["terminalBucket"])


if __name__ == "__main__":
    unittest.main()
