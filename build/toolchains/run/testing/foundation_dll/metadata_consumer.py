"""Metadata-driven lookup for assembly type/method information.

Loads assembly-metadata.json (produced by ``metadata-extract`` command from
Chaos.IL2CPP.Tools.FoundationPack.Analysis) and provides fast lookups to
replace the hardcoded static/instance/type maps in ``test_code_generator.py``.

Usage::

    loader = MetadataLoader("path/to/assembly-metadata.json")
    info = loader.get_method_info("System.Convert", "ToChar", 1)
    # info.is_static == True
    # info.is_property_accessor == False
    # info.param_types == ["System.Int32"]
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


class MethodInfo:
    """Resolved information for a single method."""

    def __init__(self, data: dict[str, Any]) -> None:
        self.name: str = data["name"]
        self.param_count: int = data["paramCount"]
        self.param_types: list[str] = list(data["paramTypes"])
        self.return_type: str = data.get("returnType", "")
        self.is_static: bool = data.get("isStatic", False)
        self.is_virtual: bool = data.get("isVirtual", False)
        self.is_property_accessor: bool = data.get("isPropertyAccessor", False)
        self.property_name: str | None = data.get("propertyName")
        self.is_constructor: bool = data.get("isConstructor", False)


class TypeInfo:
    """Resolved information for a single type."""

    def __init__(self, type_name: str, data: dict[str, Any]) -> None:
        self.type_name: str = type_name
        self.full_name: str = data.get("fullName", "")
        self.arity: int = data.get("arity", 0)
        self.is_static: bool = data.get("isStatic", False)
        self.is_value_type: bool = data.get("isValueType", False)
        self._methods: list[dict[str, Any]] = data.get("methods", [])
        self._methods_by_key: dict[tuple[str, int], MethodInfo] = {}

        for m in self._methods:
            key = (m["name"], m["paramCount"])
            self._methods_by_key[key] = MethodInfo(m)

    def get_method(self, method_name: str, param_count: int) -> MethodInfo | None:
        """Look up a method by name and parameter count."""
        return self._methods_by_key.get((method_name, param_count))

    def has_method(self, method_name: str, param_count: int) -> bool:
        return (method_name, param_count) in self._methods_by_key

    def get_all_methods(self) -> list[MethodInfo]:
        return [MethodInfo(m) for m in self._methods]

    def is_static_only(self) -> bool:
        """True if ALL public methods on this type are static (pure static class)."""
        return all(m.get("isStatic", False) for m in self._methods)


class MetadataLoader:
    """Loads and provides access to assembly metadata."""

    def __init__(self, metadata_path: str | Path) -> None:
        self._path = Path(metadata_path)
        with open(self._path, encoding="utf-8") as f:
            self._data: dict[str, Any] = json.load(f)

        self.assembly_name: str = self._data.get("assemblyName", "")
        self._types: dict[str, TypeInfo] = {}

        # Also index by short name (e.g. "Convert", "ImmutableArray")
        self._types_by_short: dict[str, list[TypeInfo]] = {}

        raw_types: dict[str, Any] = self._data.get("types", {})
        for full_name, type_data in raw_types.items():
            ti = TypeInfo(full_name, type_data)
            self._types[full_name] = ti

            short = type_data.get("typeName", "")
            if short not in self._types_by_short:
                self._types_by_short[short] = []
            self._types_by_short[short].append(ti)

    @property
    def type_count(self) -> int:
        return len(self._types)

    @property
    def method_count(self) -> int:
        return sum(len(ti._methods) for ti in self._types.values())

    def get_type(self, full_name: str) -> TypeInfo | None:
        """Look up a type by its full CLR name (e.g. ``System.Convert``)."""
        return self._types.get(full_name)

    def resolve_type(self, short_name: str, arity: int = 0) -> TypeInfo | None:
        """Resolve a short type name to a TypeInfo, preferring exact arity match.

        Args:
            short_name: Short type name (e.g. ``ImmutableArray``).
            arity: Expected generic arity (0 for non-generic, 1 for ``T``, etc.).

        Returns:
            TypeInfo if found, else None.
        """
        candidates = self._types_by_short.get(short_name, [])
        if not candidates:
            return None
        if len(candidates) == 1:
            return candidates[0]
        # Multiple candidates with different arities: find the matching one
        for c in candidates:
            if c.arity == arity:
                return c
        return candidates[0]  # fallback to first

    def type_is_static(self, short_name: str, arity: int = 0) -> bool | None:
        """Check if a type is a static class.

        Returns True/False if type found, None if type not found.
        """
        ti = self.resolve_type(short_name, arity)
        return ti.is_static if ti else None

    def type_is_value_type(self, short_name: str, arity: int = 0) -> bool | None:
        ti = self.resolve_type(short_name, arity)
        return ti.is_value_type if ti else None

    def method_is_static(self, short_name: str, method_name: str,
                         param_count: int, arity: int = 0) -> bool | None:
        """Check if a method is static, given type short name.

        Returns True/False if method found, None if type or method not found.
        """
        ti = self.resolve_type(short_name, arity)
        if ti is None:
            return None
        mi = ti.get_method(method_name, param_count)
        if mi is None:
            return None
        return mi.is_static

    def get_method_info(self, short_name: str, method_name: str,
                        param_count: int, arity: int = 0) -> MethodInfo | None:
        """Get full MethodInfo for a method."""
        ti = self.resolve_type(short_name, arity)
        if ti is None:
            return None
        return ti.get_method(method_name, param_count)

    def is_static_only_type(self, short_name: str, arity: int = 0) -> bool | None:
        """Check if a type is a pure static class (all methods are static).

        Returns True/False if type found, None if type not found.
        """
        ti = self.resolve_type(short_name, arity)
        if ti is None:
            return None
        return ti.is_static_only()

    def find_type_by_full_name_segment(self, segment: str) -> TypeInfo | None:
        """Find a type by matching a segment of its full name.

        E.g. ``find_type_by_full_name_segment("ImmutableInterlocked")``
        matches ``System.Collections.Immutable.ImmutableInterlocked``.
        """
        for full_name, ti in self._types.items():
            if segment in full_name or full_name.endswith(f".{segment}"):
                return ti
        return None
