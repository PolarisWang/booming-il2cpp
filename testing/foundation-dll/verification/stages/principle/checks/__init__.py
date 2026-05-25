"""Auto-discovery of PrincipleCheck subclasses via pkgutil."""

from __future__ import annotations

import importlib
import pkgutil

from ..base import PrincipleCheck

_registry: dict[str, type[PrincipleCheck]] = {}
_discovered = False


def discover_checks() -> dict[str, type[PrincipleCheck]]:
    """Auto-discover all PrincipleCheck subclasses in checks/ directory.

    Returns a dict mapping check_id -> PrincipleCheck subclass.
    """
    global _discovered
    if _discovered:
        return _registry

    for importer, modname, is_pkg in pkgutil.iter_modules(__path__):
        if modname.startswith("_"):
            continue
        module = importlib.import_module(f".{modname}", __package__ or __name__)
        for attr_name in dir(module):
            cls = getattr(module, attr_name)
            if (
                isinstance(cls, type)
                and issubclass(cls, PrincipleCheck)
                and cls is not PrincipleCheck
            ):
                _registry[cls.meta.check_id] = cls

    _discovered = True
    return _registry
