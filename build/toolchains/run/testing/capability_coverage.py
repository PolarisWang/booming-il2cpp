from __future__ import annotations

from enum import IntEnum
from pathlib import Path
from typing import Any
import sys

try:
    from . import declared_metadata_labels as declared_metadata_labels_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import declared_metadata_labels as declared_metadata_labels_module


class BodyAvailabilityCode(IntEnum):
    NONE = 0
    NATIVE_GENERATED = 1
    INTERPRETER_READY = 2
    BRIDGE_DISPATCH = 3
    METADATA_ONLY = 4
    EXTERNAL_RUNTIME = 5
    UNSUPPORTED = 6


_OWNER_SUBJECT_BY_FAMILY: dict[int, str] = {
    1: "SolutionCorePack",
    2: "SolutionCorePack",
    3: "SolutionCorePack",
    4: "SolutionCorePack",
    5: "SolutionCorePack",
    6: "SolutionCorePack",
    7: "SolutionCorePack",
    8: "SolutionCorePack",
    9: "SolutionCorePack",
    10: "SolutionCorePack",
    11: "SolutionCorePack",
    12: "SolutionCorePack",
    13: "HotUpdateHostPack",
    14: "MixedExecutionFeaturePack",
}

_CAPABILITY_FAMILY_BY_ITEM: dict[int, int] = {
    1: 1,
    2: 1,
    3: 1,
    4: 1,
    5: 1,
    6: 1,
    7: 1,
    8: 2,
    9: 2,
    10: 2,
    11: 2,
    12: 2,
    13: 2,
    14: 3,
    15: 3,
    16: 3,
    17: 4,
    18: 4,
    19: 4,
    20: 4,
    21: 6,
    22: 6,
    23: 6,
    24: 6,
    25: 6,
    26: 6,
    27: 5,
    28: 5,
    29: 5,
    30: 5,
    31: 7,
    32: 7,
    33: 7,
    34: 7,
    35: 8,
    36: 8,
    37: 8,
    38: 8,
    39: 9,
    40: 9,
    41: 9,
    42: 9,
    43: 10,
    44: 10,
    45: 10,
    46: 10,
    47: 11,
    48: 11,
    49: 11,
    50: 11,
    51: 12,
    52: 12,
    53: 12,
    54: 13,
    55: 13,
    56: 13,
    57: 13,
    58: 13,
    59: 13,
    60: 13,
    61: 14,
    62: 14,
    63: 14,
    64: 14,
    65: 14,
    66: 14,
    67: 2,
    68: 2,
}

_SUPPORT_STATES_BY_ITEM: dict[int, tuple[BodyAvailabilityCode, ...]] = {
    1: (BodyAvailabilityCode.NATIVE_GENERATED,),
    2: (BodyAvailabilityCode.NATIVE_GENERATED,),
    3: (BodyAvailabilityCode.NATIVE_GENERATED,),
    4: (BodyAvailabilityCode.NATIVE_GENERATED,),
    5: (BodyAvailabilityCode.NATIVE_GENERATED,),
    6: (BodyAvailabilityCode.NATIVE_GENERATED,),
    7: (BodyAvailabilityCode.NATIVE_GENERATED,),
    8: (BodyAvailabilityCode.NATIVE_GENERATED,),
    9: (BodyAvailabilityCode.NATIVE_GENERATED,),
    10: (BodyAvailabilityCode.NATIVE_GENERATED,),
    11: (BodyAvailabilityCode.NATIVE_GENERATED,),
    12: (BodyAvailabilityCode.NATIVE_GENERATED,),
    13: (BodyAvailabilityCode.NATIVE_GENERATED,),
    14: (BodyAvailabilityCode.NATIVE_GENERATED,),
    15: (BodyAvailabilityCode.NATIVE_GENERATED,),
    16: (BodyAvailabilityCode.NATIVE_GENERATED,),
    17: (BodyAvailabilityCode.NATIVE_GENERATED,),
    18: (BodyAvailabilityCode.NATIVE_GENERATED,),
    19: (BodyAvailabilityCode.NATIVE_GENERATED,),
    20: (BodyAvailabilityCode.NATIVE_GENERATED,),
    21: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    22: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    23: (BodyAvailabilityCode.METADATA_ONLY,),
    24: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    25: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    26: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    27: (BodyAvailabilityCode.NATIVE_GENERATED,),
    28: (BodyAvailabilityCode.NATIVE_GENERATED,),
    29: (BodyAvailabilityCode.NATIVE_GENERATED,),
    30: (BodyAvailabilityCode.NATIVE_GENERATED,),
    31: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.EXTERNAL_RUNTIME),
    32: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.EXTERNAL_RUNTIME),
    33: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.EXTERNAL_RUNTIME),
    34: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.EXTERNAL_RUNTIME),
    35: (BodyAvailabilityCode.NATIVE_GENERATED,),
    36: (BodyAvailabilityCode.NATIVE_GENERATED,),
    37: (BodyAvailabilityCode.NATIVE_GENERATED,),
    38: (BodyAvailabilityCode.NATIVE_GENERATED,),
    39: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.METADATA_ONLY),
    40: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.METADATA_ONLY),
    41: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.METADATA_ONLY),
    42: (BodyAvailabilityCode.NATIVE_GENERATED, BodyAvailabilityCode.METADATA_ONLY),
    43: (BodyAvailabilityCode.NATIVE_GENERATED,),
    44: (BodyAvailabilityCode.NATIVE_GENERATED,),
    45: (BodyAvailabilityCode.NATIVE_GENERATED,),
    46: (BodyAvailabilityCode.NATIVE_GENERATED,),
    47: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.UNSUPPORTED),
    48: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.UNSUPPORTED),
    49: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.UNSUPPORTED),
    50: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.UNSUPPORTED),
    51: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    52: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    53: (BodyAvailabilityCode.METADATA_ONLY, BodyAvailabilityCode.NATIVE_GENERATED),
    54: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    55: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    56: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    57: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    58: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    59: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    60: (BodyAvailabilityCode.BRIDGE_DISPATCH, BodyAvailabilityCode.INTERPRETER_READY),
    61: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    62: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    63: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    64: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    65: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    66: (
        BodyAvailabilityCode.BRIDGE_DISPATCH,
        BodyAvailabilityCode.INTERPRETER_READY,
        BodyAvailabilityCode.NATIVE_GENERATED,
    ),
    67: (BodyAvailabilityCode.NATIVE_GENERATED,),
    68: (BodyAvailabilityCode.NATIVE_GENERATED,),
}

_BENCHMARK_REQUIRED_ITEMS = {
    1,
    10,
    11,
    15,
    22,
    31,
    32,
    33,
    34,
    36,
    37,
    43,
    45,
    46,
    51,
    52,
    54,
    56,
    60,
    61,
    63,
}

_PROOF_OPTIONAL_ITEMS = {
    47,
    48,
    49,
    50,
}


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def resolve_capability_contract(*, capability_family: Any, capability_item: Any) -> dict[str, Any]:
    capability_item_value = _int_value(capability_item)
    declared_family_value = _int_value(capability_family)
    mapped_family_value = _CAPABILITY_FAMILY_BY_ITEM.get(capability_item_value, 0)
    resolved_family_value = declared_family_value or mapped_family_value
    support_states = [
        int(state)
        for state in _SUPPORT_STATES_BY_ITEM.get(capability_item_value, ())
    ]

    return {
        "capabilityFamily": resolved_family_value,
        "capabilityFamilyLabel": declared_metadata_labels_module.capability_family_label(resolved_family_value),
        "capabilityItem": capability_item_value,
        "capabilityItemLabel": declared_metadata_labels_module.capability_item_label(capability_item_value),
        "ownerSubjectId": _OWNER_SUBJECT_BY_FAMILY.get(resolved_family_value, ""),
        "supportStates": support_states,
        "supportStateLabels": declared_metadata_labels_module.body_availability_labels(support_states),
        "proofRequired": capability_item_value not in _PROOF_OPTIONAL_ITEMS,
        "benchmarkRequired": capability_item_value in _BENCHMARK_REQUIRED_ITEMS,
    }
