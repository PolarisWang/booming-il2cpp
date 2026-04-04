from __future__ import annotations

from typing import Any


NON_ENFORCED_FAMILIES = {"workflow", "perf"}
TRAFFIC_LIGHT_BUCKETS = ("green", "red", "yellow")
CASE_STATUSES = ("ok", "fail", "skip", "aborted")


class TrafficLightConfigError(ValueError):
    pass


def _empty_status_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "fail": 0,
        "skip": 0,
        "aborted": 0,
    }


def _empty_traffic_light_counts() -> dict[str, dict[str, int]]:
    return {bucket: _empty_status_counts() for bucket in TRAFFIC_LIGHT_BUCKETS}


def _normalize_case(case: dict[str, Any]) -> dict[str, Any]:
    tags = sorted({str(tag) for tag in case.get("tags", [])})
    normalized = dict(case)
    normalized["caseId"] = str(case["caseId"])
    normalized["sourceId"] = str(case["sourceId"])
    normalized["tags"] = tags
    normalized["status"] = str(case.get("status", "ok"))
    return normalized


def _normalize_selector(selector: Any) -> dict[str, Any]:
    if selector is None:
        return {}
    if not isinstance(selector, dict):
        raise TrafficLightConfigError("traffic light selector must be an object")
    return selector


def _match_selector(case: dict[str, Any], selector: dict[str, Any]) -> bool:
    selector = _normalize_selector(selector)

    case_ids = selector.get("caseIds")
    if case_ids is not None and case["caseId"] not in case_ids:
        return False

    source_ids = selector.get("sourceIds")
    if source_ids is not None and case["sourceId"] not in source_ids:
        return False

    tags = selector.get("tags")
    if tags is not None and not set(tags).issubset(set(case["tags"])):
        return False

    include = selector.get("include")
    if include is not None:
        include_items = include if isinstance(include, list) else [include]
        if not any(_match_selector(case, _normalize_selector(item)) for item in include_items):
            return False

    exclude = selector.get("exclude")
    if exclude is not None:
        exclude_items = exclude if isinstance(exclude, list) else [exclude]
        if any(_match_selector(case, _normalize_selector(item)) for item in exclude_items):
            return False

    return True


def assign_traffic_lights(
    cases: list[dict[str, Any]],
    coverage_policy: dict[str, Any] | None,
    *,
    family: str,
) -> dict[str, Any]:
    normalized_cases = [_normalize_case(case) for case in cases]
    policy = dict(coverage_policy or {})
    enforced = bool(policy.get("enforced", family not in NON_ENFORCED_FAMILIES))
    bucket_selectors = {bucket: _normalize_selector(policy.get(bucket)) for bucket in TRAFFIC_LIGHT_BUCKETS}

    assigned_cases: list[dict[str, Any]] = []
    counts = _empty_traffic_light_counts()

    for case in normalized_cases:
        matched_buckets = [
            bucket
            for bucket, selector in bucket_selectors.items()
            if selector and _match_selector(case, selector)
        ]

        if not matched_buckets:
            matched_buckets = ["green"]

        if len(matched_buckets) != 1:
            raise TrafficLightConfigError(f"traffic light selector overlap for case: {case['caseId']}")

        bucket = matched_buckets[0]
        status = case["status"] if case["status"] in CASE_STATUSES else "aborted"
        counts[bucket]["total"] += 1
        counts[bucket][status] += 1

        assigned_case = dict(case)
        assigned_case["trafficLight"] = bucket
        assigned_cases.append(assigned_case)

    return {
        "enforced": enforced,
        "caseResults": assigned_cases,
        "trafficLightCounts": counts,
    }
