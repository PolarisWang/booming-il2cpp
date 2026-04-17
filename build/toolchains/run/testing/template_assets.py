from __future__ import annotations

from pathlib import Path


def read_template(*, owner_file: str | Path, relative_template_path: str) -> str:
    template_path = Path(owner_file).resolve().parent / relative_template_path
    return template_path.read_text(encoding="utf-8")


def render_template(
    *,
    owner_file: str | Path,
    relative_template_path: str,
    replacements: dict[str, str],
) -> str:
    rendered = read_template(owner_file=owner_file, relative_template_path=relative_template_path)
    for token, value in replacements.items():
        rendered = rendered.replace(f"@@{token}@@", value)
    return rendered
