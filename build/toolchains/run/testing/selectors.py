from __future__ import annotations

from typing import Any


ENGINEERING_SELECTOR_KINDS = {
    "engineering-validation",
    "engineering-workload",
}

DECLARED_SELECTOR_KINDS = {
    "declared-unit-test",
    "declared-benchmark",
}


def suite_id(family: str, suite: str) -> str:
    return f"{family}/{suite}"


def module_id(module: str, profile: str) -> str:
    return f"module/{module}/{profile}"


def subject_id(subject: str) -> str:
    return f"subject/{subject}"


def system_id(scenario: str) -> str:
    return f"system/{scenario}"


def pipeline_id(pipeline: str) -> str:
    return f"pipeline/{pipeline}"


def engineering_id(family: str, subject: str, kind: str) -> str:
    return f"{family}/{subject}/{kind}"


def declared_id(family: str, stable_id: str) -> str:
    return f"{family}/{stable_id}"


def parse_suite_id(value: str) -> tuple[str, str]:
    family, suite = value.split("/", 1)
    if not family or not suite:
        raise ValueError(f"invalid suite id: {value}")
    return family, suite


def parse_module_id(value: str) -> tuple[str, str]:
    parts = value.split("/")
    if len(parts) != 3 or parts[0] != "module" or not parts[1] or not parts[2]:
        raise ValueError(f"invalid module id: {value}")
    return parts[1], parts[2]


def parse_subject_id(value: str) -> str:
    parts = value.split("/")
    if len(parts) != 2 or parts[0] != "subject" or not parts[1]:
        raise ValueError(f"invalid subject id: {value}")
    return parts[1]


def parse_system_id(value: str) -> str:
    parts = value.split("/")
    if len(parts) != 2 or parts[0] != "system" or not parts[1]:
        raise ValueError(f"invalid system id: {value}")
    return parts[1]


def parse_pipeline_id(value: str) -> str:
    parts = value.split("/")
    if len(parts) != 2 or parts[0] != "pipeline" or not parts[1]:
        raise ValueError(f"invalid pipeline id: {value}")
    return parts[1]


def parse_engineering_id(expected_family: str, value: str) -> tuple[str, str]:
    parts = value.split("/")
    if len(parts) != 3 or parts[0] != expected_family or not parts[1] or not parts[2]:
        raise ValueError(f"invalid {expected_family} id: {value}")
    return parts[1], parts[2]


def parse_declared_id(expected_family: str, value: str) -> str:
    parts = value.split("/", 1)
    if len(parts) != 2 or parts[0] != expected_family or not parts[1]:
        raise ValueError(f"invalid {expected_family} id: {value}")
    return parts[1]


def normalize_selector_options(kind: str, options: dict[str, Any]) -> dict[str, Any]:
    normalized = dict(options)
    object_id = normalized.get("id")

    if kind == "suite":
        if object_id:
            family, suite = parse_suite_id(str(object_id))
            normalized["family"] = family
            normalized["suite"] = suite
        else:
            family = str(normalized.get("family") or "")
            suite = str(normalized.get("suite") or "")
            if not family or not suite:
                raise ValueError("suite selector requires --id or both --family and --suite")
            normalized["id"] = suite_id(family, suite)
        return normalized

    if kind == "module":
        if object_id:
            module, profile = parse_module_id(str(object_id))
            normalized["module"] = module
            normalized["profile"] = profile
        else:
            module = str(normalized.get("module") or "")
            profile = str(normalized.get("profile") or "")
            if not module or not profile:
                raise ValueError("module selector requires --id or both --module and --profile")
            normalized["id"] = module_id(module, profile)
        return normalized

    if kind == "subject":
        if object_id:
            normalized["subject"] = parse_subject_id(str(object_id))
        else:
            subject = str(normalized.get("subject") or "")
            if not subject:
                raise ValueError("subject selector requires --id or --subject")
            normalized["id"] = subject_id(subject)
        return normalized

    if kind == "system":
        if object_id:
            normalized["scenario"] = parse_system_id(str(object_id))
        else:
            scenario = str(normalized.get("scenario") or "")
            if not scenario:
                raise ValueError("system selector requires --id or --scenario")
            normalized["id"] = system_id(scenario)
        return normalized

    if kind == "pipeline":
        if object_id:
            normalized["pipeline"] = parse_pipeline_id(str(object_id))
        else:
            pipeline = str(normalized.get("pipeline") or "")
            if not pipeline:
                raise ValueError("pipeline selector requires --id or --pipeline")
            normalized["id"] = pipeline_id(pipeline)
        return normalized

    if kind in ENGINEERING_SELECTOR_KINDS:
        if object_id:
            subject, engineering_kind = parse_engineering_id(kind, str(object_id))
            normalized["subject"] = subject
            normalized["kind"] = engineering_kind
        else:
            subject = str(normalized.get("subject") or "")
            engineering_kind = str(normalized.get("kind") or "")
            if not subject or not engineering_kind:
                raise ValueError(f"{kind} selector requires --id or both --subject and --kind")
            normalized["id"] = engineering_id(kind, subject, engineering_kind)
        return normalized

    if kind in DECLARED_SELECTOR_KINDS:
        if object_id:
            normalized["stable_id"] = parse_declared_id(kind, str(object_id))
        else:
            stable_id = str(
                normalized.get("stable_id")
                or normalized.get("stable-id")
                or normalized.get("stableId")
                or ""
            )
            if not stable_id:
                raise ValueError(f"{kind} selector requires --id or --stable-id")
            normalized["id"] = declared_id(kind, stable_id)
            normalized["stable_id"] = stable_id
        return normalized

    raise ValueError(f"unsupported selector kind: {kind}")
