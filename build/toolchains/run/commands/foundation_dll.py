from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Callable

try:
    from ..core.result import CommandResult
    from ..testing import foundation_dll_audit_generator as audit_generator_module
    from ..testing.foundation_dll import derive as denominator_derive_module
    from ..testing.foundation_dll import gap_analyzer as gap_analyzer_module
    from ..testing.foundation_dll import promote as denominator_promote_module
    from ..testing import inventory_generator as inventory_generator_module
    from ..testing.foundation_dll import consistency as projection_consistency_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult
    from testing import foundation_dll_audit_generator as audit_generator_module
    from testing.foundation_dll import derive as denominator_derive_module
    from testing.foundation_dll import gap_analyzer as gap_analyzer_module
    from testing.foundation_dll import promote as denominator_promote_module
    from testing import inventory_generator as inventory_generator_module
    from testing.foundation_dll import consistency as projection_consistency_module
    from testing.events import build_event


def _emit_event(
    progress_callback: Callable[[dict[str, Any]], None] | None,
    *,
    event_type: str,
    completed: int,
    total: int,
    active_unit: str,
    step_status: str | None = None,
) -> None:
    if progress_callback is None:
        return

    payload: dict[str, Any] = {
        "completedUnits": completed,
        "totalUnits": total,
        "activeUnit": active_unit,
    }
    if step_status is not None:
        payload["suiteStatus"] = step_status

    progress_callback(build_event(event_type, payload, status=step_status or "running"))


def _success(command_text: str, host_platform: str, payload: dict[str, Any], text: str | None = None) -> CommandResult:
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target="foundation-dll",
        payload=payload,
        text=text or (command_text + "\n"),
    )


def _failure(command_text: str, host_platform: str, error: str) -> CommandResult:
    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target="foundation-dll",
        errors=[error],
        payload={"artifacts": [], "importantOutputs": [], "consoleText": error},
        text=f"Run failed: {command_text}\n- {error}\n",
    )


def _option_is_true(value: Any) -> bool:
    if isinstance(value, bool):
        return value
    text = str(value or "").strip().lower()
    return text in {"1", "true", "yes", "y", "on"}


def _get_option(options: dict[str, Any], name: str) -> Any:
    if name in options:
        return options[name]
    dashed = name.replace("_", "-")
    if dashed in options:
        return options[dashed]
    underscored = name.replace("-", "_")
    if underscored in options:
        return options[underscored]
    return None


def _handle_derive(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = str(options.get("scope") or "all")
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll derive")
    payload = denominator_derive_module.generate_candidate_ledger(repo_root, scope=scope)
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll derive", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_analyze_gaps(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = str(_get_option(options, "scope") or "all")
    update_ledger = _option_is_true(_get_option(options, "update_ledger"))
    auto_generate = _option_is_true(_get_option(options, "auto_generate"))
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll analyze-gaps")
    payload = gap_analyzer_module.analyze_gaps(
        repo_root,
        scope=scope,
        update_ledger=update_ledger,
        auto_generate=auto_generate,
    )
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll analyze-gaps", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_check_family(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    family = str(_get_option(options, "family") or "").strip()
    if not family:
        return _failure(command_text, host_platform, "family flag is required for foundation-dll check-family")
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll check-family")
    payload = gap_analyzer_module.analyze_gaps(repo_root, scope=family, update_ledger=False)
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll check-family", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_promote(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = str(_get_option(options, "scope") or "all")
    promote_approved = _option_is_true(_get_option(options, "promote_approved"))
    if not promote_approved:
        return _failure(command_text, host_platform, "promote-approved flag is required for foundation-dll promote")
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll promote")
    payload = denominator_promote_module.promote_candidate_ledger(
        repo_root,
        scope=scope,
        promote_approved=promote_approved,
    )
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll promote", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_refresh(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll refresh")
    payload = audit_generator_module.write_foundation_dll_audit_outputs(repo_root)
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll refresh", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_verify_consistency(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    mode = str(_get_option(options, "mode") or "quick")
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="foundation-dll verify-consistency")
    payload = projection_consistency_module.verify_projection_consistency(repo_root, mode=mode)
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="foundation-dll verify-consistency", step_status="ok")
    return _success(command_text, host_platform, payload)


def _handle_full(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = str(_get_option(options, "scope") or "all")
    promote_approved = _option_is_true(_get_option(options, "promote_approved"))
    mode = str(_get_option(options, "mode") or "full")
    if not promote_approved:
        return _failure(command_text, host_platform, "promote-approved flag is required for foundation-dll full")

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=4, active_unit="foundation-dll derive")
    derive_payload = denominator_derive_module.generate_candidate_ledger(repo_root, scope=scope)
    _emit_event(progress_callback, event_type="progress", completed=1, total=4, active_unit="foundation-dll promote", step_status="running")

    promote_payload = denominator_promote_module.promote_candidate_ledger(
        repo_root,
        scope=scope,
        promote_approved=promote_approved,
    )
    _emit_event(progress_callback, event_type="progress", completed=2, total=4, active_unit="foundation-dll refresh", step_status="running")

    refresh_payload = audit_generator_module.write_foundation_dll_audit_outputs(repo_root)
    _emit_event(progress_callback, event_type="progress", completed=3, total=4, active_unit="verification-v1 formal refresh", step_status="running")

    output_root = inventory_generator_module.resolve_inventory_output_root(repo_root, None)
    verification_payload = inventory_generator_module.refresh_inventory_outputs(
        repo_root,
        host_platform=host_platform,
        output_root=output_root,
    )
    consistency_payload = projection_consistency_module.verify_projection_consistency(repo_root, mode=mode)
    _emit_event(progress_callback, event_type="progress", completed=4, total=4, active_unit="foundation-dll full", step_status="ok")

    return _success(
        command_text,
        host_platform,
        {
            "derive": derive_payload,
            "promote": promote_payload,
            "refresh": refresh_payload,
            "verification": verification_payload,
            "consistency": consistency_payload,
            "artifacts": list(dict.fromkeys(
                list(refresh_payload.get("artifacts") or []) + list(verification_payload.get("artifacts") or [])
            )),
        },
    )


def _handle_onboard(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    scope = str(_get_option(options, "scope") or "all")
    promote_approved = _option_is_true(_get_option(options, "promote_approved"))
    mode = str(_get_option(options, "mode") or "full")
    if not promote_approved:
        return _failure(command_text, host_platform, "promote-approved flag is required for foundation-dll onboard")

    return _handle_full(
        repo_root,
        host_platform,
        command_text,
        {
            "scope": scope,
            "promote_approved": promote_approved,
            "mode": mode,
        },
        progress_callback=progress_callback,
    )


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any] | None = None,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    opts = dict(options or {})
    command_id = str(command.get("id") or "")
    try:
        if command_id == "foundation-dll-derive":
            return _handle_derive(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-analyze-gaps":
            return _handle_analyze_gaps(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-check-family":
            return _handle_check_family(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-promote":
            return _handle_promote(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-refresh":
            return _handle_refresh(repo_root, host_platform, command_text, progress_callback=progress_callback)
        if command_id == "foundation-dll-verify-consistency":
            return _handle_verify_consistency(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-full":
            return _handle_full(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        if command_id == "foundation-dll-onboard":
            return _handle_onboard(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        return _failure(command_text, host_platform, f"unsupported foundation-dll command: {command_id}")
    except Exception as error:
        return _failure(command_text, host_platform, str(error))
