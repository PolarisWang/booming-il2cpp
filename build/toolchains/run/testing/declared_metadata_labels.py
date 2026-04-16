from __future__ import annotations

from typing import Any


MODE_ORDER = ("managed", "native", "interpreter")
MODE_FLAGS = {
    "managed": 1 << 0,
    "native": 1 << 1,
    "interpreter": 1 << 2,
}
ALL_MODE_FLAGS = sum(MODE_FLAGS.values())

UNIT_CATEGORY_LABELS = {
    1: "Runtime Contract",
    2: "Metadata Contract",
    3: "Interop Contract",
    4: "Host Contract",
    5: "Hot Update Contract",
}

BENCHMARK_CATEGORY_LABELS = {
    1: "Runtime Dispatch",
    2: "Startup",
    3: "Allocation",
    4: "Hot Update",
}

CAPABILITY_FAMILY_LABELS = {
    0: "Unspecified",
    1: "Primitives And Ops",
    2: "Type System And Generics",
    3: "Async And Iterators",
    4: "Delegates And Closures",
    5: "Exceptions And Control Flow",
    6: "Reflection And Metadata",
    7: "Interop And Marshaling",
    8: "Threading And Synchronization",
    9: "Runtime Services",
    10: "Unsafe And Low Level",
    11: "Diagnostics And Tracing",
    12: "Linker And Aot Closure",
    13: "Hot Update Workflow",
    14: "Mixed Execution",
}

CAPABILITY_ITEM_LABELS = {
    0: "Unspecified",
    1: "Arithmetic Ops",
    2: "Bitwise Ops",
    3: "Comparison And Branch",
    4: "Numeric Conversion",
    5: "Overflow Behavior",
    6: "Array Basics",
    7: "Array Boxing",
    8: "Generic Instantiation",
    9: "Generic Constraint",
    10: "Generic Virtual Dispatch",
    11: "Generic Interface Dispatch",
    12: "Generic Collections",
    13: "Generic Layout",
    14: "Async Await",
    15: "Task And ValueTask Flow",
    16: "Iterator State Machine",
    17: "Delegate Invocation",
    18: "Delegate Chaining",
    19: "Lambda Closure Capture",
    20: "Event Callback Flow",
    21: "Custom Attribute Lookup",
    22: "Member Metadata Lookup",
    23: "Metadata Supplement",
    24: "Metadata Closure",
    25: "Linker Preserve Contract",
    26: "Linker Stripping",
    27: "Exception Basic",
    28: "Nested Exception",
    29: "Finally And Filter",
    30: "Cross Boundary Exception",
    31: "Native Call Interop",
    32: "String And Utf8 Marshaling",
    33: "Struct Marshaling",
    34: "Delegate Callback Interop",
    35: "Threading",
    36: "Task Scheduling",
    37: "Monitor And Locking",
    38: "Thread Local State",
    39: "Runtime Helpers",
    40: "Gc Sensitive Flow",
    41: "Resource Lifecycle",
    42: "Loader Contract",
    43: "Span And Memory",
    44: "Ref Struct And Stackalloc",
    45: "Unsafe Pointer",
    46: "Function Pointer",
    47: "Runtime Trace Output",
    48: "Native Symbol Evidence",
    49: "Crash Evidence",
    50: "Failure Diagnostics",
    51: "Required Instantiation Closure",
    52: "Generic Sharing Boundary",
    53: "Aot Closure Validation",
    54: "Package Load",
    55: "Shared Contract Binding",
    56: "Method Replacement",
    57: "Hot Update Metadata Supplement",
    58: "Patch Integrity",
    59: "Version Rollback",
    60: "Patch Callback Flow",
    61: "Mixed Bridge Dispatch",
    62: "Interpreter Lowering",
    63: "Interpreter Arithmetic",
    64: "Mixed Generic Flow",
    65: "Mixed Exception Flow",
    66: "Mixed Delegate Flow",
    67: "Object Cast And Type Check",
    68: "Array Covariance",
}

BODY_AVAILABILITY_LABELS = {
    0: "None",
    1: "NativeGenerated",
    2: "InterpreterReady",
    3: "BridgeDispatch",
    4: "MetadataOnly",
    5: "ExternalRuntime",
    6: "Unsupported",
}

METRIC_LABELS = {
    1 << 0: "Wall Clock",
    1 << 1: "Managed Alloc",
    1 << 2: "Native Alloc",
    1 << 3: "Working Set",
}

RUNTIME_FEATURE_LABELS = {
    1 << 0: "Generic Sharing",
    1 << 1: "Reflection",
    1 << 2: "Delegate",
    1 << 3: "Exception Flow",
    1 << 4: "Native Interop",
    1 << 5: "Hot Update",
    1 << 6: "Async State Machine",
    1 << 7: "Threading",
    1 << 8: "Synchronization",
    1 << 9: "Span/Memory",
    1 << 10: "Unsafe Pointer",
    1 << 11: "Function Pointer",
    1 << 12: "Metadata Supplement",
    1 << 13: "Generic Virtual Dispatch",
    1 << 14: "Ref Struct/Stackalloc",
}

EVIDENCE_LABELS = {
    1 << 0: "Stdout",
    1 << 1: "File Artifact",
    1 << 2: "Metadata",
    1 << 3: "Native Symbol",
}

ARCHETYPE_LABELS = {
    0: "Unspecified",
    1: "Simple Library Solution",
    2: "Multi-Project Solution",
    3: "Package Reference Solution",
    4: "Reference Assembly Solution",
    5: "CoreLib Reference Solution",
    6: "Mixed Reference Closure Solution",
    7: "Skeleton Patch Solution",
    8: "Full Project Hot-Update Solution",
    9: "Version Compatibility Solution",
    10: "Reference Assembly Patch Solution",
    11: "Mixed Bridge Solution",
}

HOT_UPDATE_CAPABILITY_LABELS = {
    1 << 0: "Package Load",
    1 << 1: "Shared Contract Binding",
    1 << 2: "Method Replacement",
    1 << 3: "Metadata Supplement",
    1 << 4: "Patch Integrity",
    1 << 5: "Version Rollback",
    1 << 6: "Patch Callback Flow",
}


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def labels_from_mask(value: Any, labels: dict[int, str]) -> list[str]:
    mask = _int_value(value)
    return [label for bit, label in labels.items() if mask & bit]


def supported_modes_from_mask(value: Any) -> list[str]:
    mask = _int_value(value)
    if mask <= 0:
        mask = ALL_MODE_FLAGS
    return [mode for mode in MODE_ORDER if mask & MODE_FLAGS[mode]]


def unit_category_label(value: Any) -> str:
    return UNIT_CATEGORY_LABELS.get(_int_value(value), "Unspecified")


def benchmark_category_label(value: Any) -> str:
    return BENCHMARK_CATEGORY_LABELS.get(_int_value(value), "Uncategorized")


def capability_family_label(value: Any) -> str:
    return CAPABILITY_FAMILY_LABELS.get(_int_value(value), "Unknown")


def capability_item_label(value: Any) -> str:
    return CAPABILITY_ITEM_LABELS.get(_int_value(value), "Unknown")


def body_availability_label(value: Any) -> str:
    return BODY_AVAILABILITY_LABELS.get(_int_value(value), "Unknown")


def body_availability_labels(values: list[Any] | tuple[Any, ...]) -> list[str]:
    return [body_availability_label(value) for value in values]


def archetype_label(value: Any) -> str:
    return ARCHETYPE_LABELS.get(_int_value(value), "Unknown")
