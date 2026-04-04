#!/usr/bin/env python3
from __future__ import annotations

import argparse
import base64
import json
from pathlib import Path


def read_trace_document(path: str) -> dict:
    resolved_path = Path(path).resolve()
    return json.loads(resolved_path.read_text(encoding="utf-8"))


def get_minimal_events(sample: dict) -> list[dict]:
    return [
        {
            "eventName": str(event["eventName"]),
            "phase": str(event["phase"]),
            "subjectKind": str(event["subjectKind"]),
            "subjectId": str(event["subjectId"]),
            "order": int(event["order"]),
            "status": str(event["status"]),
        }
        for event in sample["events"]
    ]


def get_warmup_session_marker(sample_id: str, event: dict) -> str:
    encoded_subject_id = base64.b64encode(str(event["subjectId"]).encode("utf-8")).decode("ascii")
    return (
        f"warmup-trace|sampleId={sample_id}|order={event['order']}|eventName={event['eventName']}"
        f"|phase={event['phase']}|subjectKind={event['subjectKind']}|subjectId={encoded_subject_id}|status={event['status']}"
    )


def assert_runtime_trace_provenance(actual: dict) -> None:
    if "traceSource" not in actual:
        raise RuntimeError("runtime trace provenance missing: traceSource")
    if actual["traceSource"] != "host-embedding-session":
        raise RuntimeError("runtime trace provenance mismatch: traceSource must be 'host-embedding-session'")
    if "sessionTrace" not in actual:
        raise RuntimeError("runtime trace provenance missing: sessionTrace")
    if actual["sessionTrace"] is None:
        raise RuntimeError("runtime trace provenance missing: sessionTrace")

    session_trace = list(actual["sessionTrace"])
    for required_entry in (
        "host:start",
        "registration:assemblies",
        "registration:managed-exports",
        "registration:callbacks",
        "managed-entry:begin",
        "managed-entry:end",
    ):
        if required_entry not in session_trace:
            raise RuntimeError(f"runtime trace provenance missing session entry: {required_entry}")

    for sample in actual["samples"]:
        for event in sample["events"]:
            expected_marker = get_warmup_session_marker(str(sample["sampleId"]), event)
            if expected_marker not in session_trace:
                raise RuntimeError(f"runtime trace provenance missing warmup session marker: {expected_marker}")


def compare_traces(expected: dict, actual: dict) -> None:
    assert_runtime_trace_provenance(actual)

    for field_name in ("formatVersion", "traceName", "targetPlatform"):
        if expected[field_name] != actual[field_name]:
            raise RuntimeError(
                f"trace field mismatch: {field_name} expected '{expected[field_name]}' actual '{actual[field_name]}'"
            )

    expected_samples = list(expected["samples"])
    actual_samples = list(actual["samples"])
    if len(expected_samples) != len(actual_samples):
        raise RuntimeError(f"sample count mismatch: expected {len(expected_samples)} actual {len(actual_samples)}")

    for sample_index, (expected_sample, actual_sample) in enumerate(zip(expected_samples, actual_samples, strict=True)):
        if expected_sample["sampleId"] != actual_sample["sampleId"]:
            raise RuntimeError(
                f"sampleId mismatch at index {sample_index}: expected '{expected_sample['sampleId']}' actual '{actual_sample['sampleId']}'"
            )
        if expected_sample["scenario"] != actual_sample["scenario"]:
            raise RuntimeError(
                f"scenario mismatch at index {sample_index}: expected '{expected_sample['scenario']}' actual '{actual_sample['scenario']}'"
            )

        expected_events = get_minimal_events(expected_sample)
        actual_events = get_minimal_events(actual_sample)
        if len(expected_events) != len(actual_events):
            raise RuntimeError(
                f"event count mismatch for sample '{expected_sample['sampleId']}': expected {len(expected_events)} actual {len(actual_events)}"
            )

        for event_index, (expected_event, actual_event) in enumerate(zip(expected_events, actual_events, strict=True)):
            for field_name in ("eventName", "phase", "subjectKind", "subjectId", "order", "status"):
                if expected_event[field_name] != actual_event[field_name]:
                    raise RuntimeError(
                        f"event mismatch for sample '{expected_sample['sampleId']}' at event index {event_index} "
                        f"field '{field_name}': expected '{expected_event[field_name]}' actual '{actual_event[field_name]}'"
                    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("expected_path")
    parser.add_argument("actual_path")
    args = parser.parse_args()

    expected = read_trace_document(args.expected_path)
    actual = read_trace_document(args.actual_path)
    compare_traces(expected, actual)
    print(f"Warmup trace compare passed: {Path(args.actual_path).resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
