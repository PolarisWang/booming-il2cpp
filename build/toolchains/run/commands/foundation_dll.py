from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Callable

try:
    from ..core.result import CommandResult
    from ..testing import foundation_dll_audit_generator as audit_generator_module
    from ..testing.foundation_dll import execution_entry as execution_entry_module
    from ..testing.foundation_dll import candidate_ledger_generator as denominator_derive_module
    from ..testing.foundation_dll import candidate_ledger_promoter as denominator_promote_module
    from ..testing import inventory_generator as inventory_generator_module
    from ..testing.foundation_dll import projection_consistency_verifier as projection_consistency_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult
    from testing import foundation_dll_audit_generator as audit_generator_module
    from testing.foundation_dll import execution_entry as execution_entry_module
    from testing.foundation_dll import candidate_ledger_generator as denominator_derive_module
    from testing.foundation_dll import candidate_ledger_promoter as denominator_promote_module
    from testing import inventory_generator as inventory_generator_module
    from testing.foundation_dll import projection_consistency_verifier as projection_consistency_module
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
    _emit_event(progress_callback, event_type="progress", completed=3, total=4, active_unit="formal refresh", step_status="running")

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


def _handle_execute(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
) -> CommandResult:
    assembly_name = str(_get_option(options, "assembly") or "").strip()
    family_id = str(_get_option(options, "family") or "").strip()
    kind = str(_get_option(options, "kind") or "test").strip()
    if not assembly_name or not family_id:
        return _failure(command_text, host_platform, "assembly and family are required for foundation-dll execute")
    payload = execution_entry_module.execute_entry(
        repo_root,
        assembly_name=assembly_name,
        family_id=family_id,
        kind=kind,
    )
    return _success(command_text, host_platform, payload)


def _handle_verify_family(
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, Any],
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    family_slug = str(_get_option(options, "family") or "").strip()
    assembly = str(_get_option(options, "assembly") or "System.Private.CoreLib")
    mode = str(_get_option(options, "mode") or "standard")
    skip_raw = _get_option(options, "skip")
    skip_stages = [s.strip() for s in skip_raw.split(",") if s.strip()] if skip_raw else None
    codegen_mode = str(_get_option(options, "codegen_mode") or "").strip() or None

    if not family_slug:
        return _failure(command_text, host_platform, "--family is required (e.g. --family convert-char)")

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=7, active_unit=f"foundation-dll verify-family {family_slug}")

    try:
        from ..testing.foundation_dll.family_verification_orchestrator import verify_family
    except ImportError:
        root = Path(__file__).resolve().parents[1]
        sys.path.insert(0, str(root))
        from testing.foundation_dll.family_verification_orchestrator import verify_family

    payload = verify_family(family_slug, assembly=assembly, mode=mode, skip_stages=skip_stages, codegen_mode=codegen_mode)
    report = payload.get("unifiedReport", payload)
    overall = report.get("overall_status", "failed")

    _emit_event(progress_callback, event_type="progress", completed=7, total=7, active_unit=f"foundation-dll verify-family {family_slug}", step_status=overall)

    text_lines = [f"Family: {family_slug}", f"Overall: {overall}"]
    for stage_name, stage in report.get("stages", {}).items():
        text_lines.append(f"  {stage_name}: {stage.get('status', '?')} — {stage.get('summary', '')}")
    text_lines.append(f"Coverage: {report.get('coverage', {})}")
    text = "\n".join(text_lines) + "\n"

    if overall == "passed":
        return _success(command_text, host_platform, payload, text=text)
    else:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target="foundation-dll",
            errors=[f"verify-family {family_slug}: {overall}"],
            payload=payload,
            text=text,
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
        if command_id == "foundation-dll-execute":
            return _handle_execute(repo_root, host_platform, command_text, opts)
        if command_id == "foundation-dll-verify-family":
            return _handle_verify_family(repo_root, host_platform, command_text, opts, progress_callback=progress_callback)
        return _failure(command_text, host_platform, f"unsupported foundation-dll command: {command_id}")
    except Exception as error:
        return _failure(command_text, host_platform, str(error))
