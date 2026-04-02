from __future__ import annotations

import json
from typing import Any


class CommandResult:
    def __init__(
        self,
        *,
        command: str,
        status: str,
        host_platform: str,
        target: str | None,
        duration_ms: int = 0,
        checks: list[Any] | None = None,
        errors: list[str] | None = None,
        payload: dict[str, Any] | None = None,
        text: str | None = None,
    ) -> None:
        self.command = command
        self.status = status
        self.host_platform = host_platform
        self.target = target
        self.duration_ms = duration_ms
        self.checks = list(checks or [])
        self.errors = list(errors or [])
        self.payload = dict(payload or {})
        self.text = text

    @classmethod
    def success(
        cls,
        command: str,
        host_platform: str,
        target: str | None,
        *,
        payload: dict[str, Any] | None = None,
        text: str | None = None,
        checks: list[Any] | None = None,
        duration_ms: int = 0,
    ) -> "CommandResult":
        return cls(
            command=command,
            status="ok",
            host_platform=host_platform,
            target=target,
            duration_ms=duration_ms,
            checks=checks,
            errors=[],
            payload=payload,
            text=text,
        )

    @classmethod
    def failure(
        cls,
        command: str,
        host_platform: str,
        target: str | None,
        *,
        errors: list[str],
        payload: dict[str, Any] | None = None,
        text: str | None = None,
        checks: list[Any] | None = None,
        duration_ms: int = 0,
    ) -> "CommandResult":
        return cls(
            command=command,
            status="error",
            host_platform=host_platform,
            target=target,
            duration_ms=duration_ms,
            checks=checks,
            errors=errors,
            payload=payload,
            text=text,
        )

    def to_dict(self) -> dict[str, Any]:
        serialized = {
            "command": self.command,
            "status": self.status,
            "hostPlatform": self.host_platform,
            "target": self.target,
            "durationMs": self.duration_ms,
            "checks": self.checks,
            "errors": self.errors,
        }
        serialized.update(self.payload)
        return serialized

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), ensure_ascii=False, indent=2)
