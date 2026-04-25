from __future__ import annotations

import csv
import io
import json
import sys
from pathlib import Path
from typing import Any

try:
    from ..core.common import write_json
    from . import inventory_source as inventory_source_module
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from testing import inventory_source as inventory_source_module
    from testing import verification_layout as verification_layout_module


INVENTORY_SCHEMA_VERSION = 1
MODE_ORDER = ("managed", "native", "interpreter")
UNIT_STAGES = (
    ("collector", 10),
    ("registry", 20),
    ("workspace", 30),
    ("managed-proof", 40),
    ("native-proof", 50),
    ("hotupdate-proof", 60),
)

CAPABILITY_COLUMNS = (
    ("ownerSubjectId", "OwnerSubject"),
    ("capabilityFamily", "CapabilityFamily"),
    ("capabilityFamilyLabel", "CapabilityFamilyLabel"),
    ("capabilityItem", "CapabilityItem"),
    ("capabilityItemLabel", "CapabilityLabel"),
    ("supportStates", "SupportStates"),
    ("supportStateLabels", "SupportStateLabels"),
    ("proofRequired", "ProofReq"),
    ("benchmarkRequired", "BenchmarkReq"),
    ("declaredUnitCount", "DeclaredUnitCount"),
    ("declaredBenchmarkCount", "DeclaredBenchmarkCount"),
    ("contractStatus", "ContractStatus"),
    ("statusReason", "StatusReason"),
)
UNIT_COLUMNS = (
    ("subjectId", "Subject"),
    ("stableId", "StableId"),
    ("alias", "Alias"),
    ("method", "Method"),
    ("stage", "Stage"),
    ("stageOrder", "StageOrder"),
    ("capabilityFamily", "CapabilityFamily"),
    ("capabilityItem", "CapabilityItem"),
    ("capabilityItemLabel", "Capability"),
    ("ownerSubjectId", "Owner"),
    ("supportStateLabels", "SupportStates"),
    ("stageRequirement", "StageRequirement"),
    ("stageCoverage", "StageCoverage"),
    ("stageStatus", "StageStatus"),
    ("statusReason", "StatusReason"),
    ("defaultGoalId", "Goal"),
    ("defaultMatrixId", "Matrix"),
    ("evidenceLabels", "Evidence"),
    ("priority", "Priority"),
)
UNIT_CSV_COLUMNS = tuple(
    (key, label)
    for key, label in UNIT_COLUMNS
    if key not in {"stageRequirement", "stageCoverage"}
)
BENCHMARK_COLUMNS = (
    ("platformId", "Platform"),
    ("deviceId", "DeviceId"),
    ("deviceName", "Device"),
    ("subjectId", "Subject"),
    ("stableId", "StableId"),
    ("alias", "Alias"),
    ("method", "Method"),
    ("capabilityFamily", "CapabilityFamily"),
    ("capabilityItem", "CapabilityItem"),
    ("capabilityItemLabel", "Capability"),
    ("ownerSubjectId", "Owner"),
    ("supportStateLabels", "SupportStates"),
    ("supportedModes", "SupportedModes"),
    ("metricLabels", "Metrics"),
    ("warmupCount", "Warmup"),
    ("iterationCount", "Iter"),
    ("invocationCount", "Invoke"),
    ("managedStatus", "M.Status"),
    ("managedMeanDurationMs", "M.ms"),
    ("managedOpsPerSecond", "M.ops"),
    ("nativeStatus", "N.Status"),
    ("nativeMeanDurationMs", "N.ms"),
    ("nativeOpsPerSecond", "N.ops"),
    ("nativeRelativeToManaged", "N.Rel"),
    ("interpreterStatus", "I.Status"),
    ("interpreterMeanDurationMs", "I.ms"),
    ("interpreterOpsPerSecond", "I.ops"),
    ("interpreterRelativeToManaged", "I.Rel"),
    ("isStale", "Stale"),
    ("lastRecordedAt", "LastRecordedAt"),
    ("gitCommit", "GitCommit"),
)

BENCHMARK_TABLE_TITLE = "性能结果总表"
BENCHMARK_TABLE_DESCRIPTION = (
    "一行代表一个性能方法在一台设备上的结果。"
    "用它看这台设备有没有跑过、各模式谁更快、结果是不是过期。"
)


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _list_value(value: Any) -> list[Any]:
    if isinstance(value, list):
        return value
    if isinstance(value, tuple):
        return list(value)
    if value in (None, ""):
        return []
    return [value]


def _relative(repo_root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _capability_key(item: dict[str, Any]) -> tuple[int, int]:
    return (_int_value(item.get("capabilityFamily")), _int_value(item.get("capabilityItem")))


def _has_unsupported_contract(item: dict[str, Any]) -> bool:
    state_ids = {_int_value(value) for value in _list_value(item.get("supportStates"))}
    state_labels = {str(value) for value in _list_value(item.get("supportStateLabels"))}
    return 6 in state_ids or "Unsupported" in state_labels


def _has_native_support(item: dict[str, Any]) -> bool:
    state_ids = {_int_value(value) for value in _list_value(item.get("supportStates"))}
    state_labels = {str(value) for value in _list_value(item.get("supportStateLabels"))}
    return 1 in state_ids or "NativeGenerated" in state_labels


def _method_label(item: dict[str, Any], *, prefer_workload: bool = False) -> str:
    if prefer_workload:
        workload_entry = str(item.get("workloadEntry") or "").strip()
        if workload_entry:
            return workload_entry
    source_entry = str(item.get("sourceEntry") or "").strip()
    if source_entry:
        return source_entry
    assembly_name = str(item.get("assemblyName") or "").strip()
    declaring_type = str(item.get("declaringType") or "").strip()
    method_signature = str(item.get("methodSignature") or item.get("methodName") or "").strip()
    if assembly_name and declaring_type and method_signature:
        return f"{assembly_name}/{declaring_type}::{method_signature}"
    return str(item.get("stableId") or "").strip()


def _benchmark_mode_display(key: str) -> str:
    if key.startswith("managed"):
        return "托管模式"
    if key.startswith("native"):
        return "原生模式"
    if key.startswith("interpreter"):
        return "解释执行模式"
    return "当前模式"


def _benchmark_hint_summary(key: str, label: str) -> str:
    if key == "platformId":
        return "这条结果是在哪个平台上跑出来的。"
    if key == "deviceId":
        return "这条结果来自哪一台具体设备。"
    if key == "deviceName":
        return "给人直接看的设备名称。"
    if key == "subjectId":
        return "这条结果属于哪个 subject。"
    if key == "stableId":
        return "这个性能方法的稳定编号。"
    if key == "alias":
        return "这个性能方法的短名字。"
    if key == "method":
        return "真正被执行的性能方法。"
    if key == "capabilityFamily":
        return "这个方法对应的能力大类编号。"
    if key == "capabilityItem":
        return "这个方法对应的具体能力编号。"
    if key == "capabilityItemLabel":
        return "这个方法主要在测哪项能力。"
    if key == "ownerSubjectId":
        return "这项能力目前归哪个 subject 负责。"
    if key == "supportStateLabels":
        return "这项能力当前宣称支持到什么程度。"
    if key == "supportedModes":
        return "这个方法理论上应该有哪些运行模式结果。"
    if key == "metricLabels":
        return "这条记录会展示哪些性能指标。"
    if key == "warmupCount":
        return "正式计数前先预热了多少次。"
    if key == "iterationCount":
        return "正式统计一共跑了多少轮。"
    if key == "invocationCount":
        return "每一轮里实际调用了多少次目标方法。"
    if key.endswith("Status"):
        return f"{_benchmark_mode_display(key)}现在有没有可用结果。"
    if key.endswith("MeanDurationMs"):
        return f"{_benchmark_mode_display(key)}的平均耗时。"
    if key.endswith("OpsPerSecond"):
        return f"{_benchmark_mode_display(key)}每秒大约能跑多少次。"
    if key.endswith("RelativeToManaged"):
        return f"{_benchmark_mode_display(key)}和托管模式相比快还是慢。"
    if key == "isStale":
        return "这条结果现在是不是过期了。"
    if key == "lastRecordedAt":
        return "这条结果最近一次写入的时间。"
    if key == "gitCommit":
        return "跑出这条结果时对应的代码提交。"
    return f"{label} 列用来帮助你快速读懂这条性能结果。"


def _benchmark_hint_definition(key: str, label: str) -> str:
    if key == "platformId":
        return "用来区分 Windows、Android、iOS 等不同平台。"
    if key == "deviceId":
        return "同一平台下每台设备的唯一标识。"
    if key == "deviceName":
        return "便于人工阅读的设备名称，比如某台 PC 或手机型号。"
    if key == "subjectId":
        return "说明这条结果属于哪个测试主题。"
    if key == "stableId":
        return "说明这到底是哪个 benchmark 方法。"
    if key == "alias":
        return "给 benchmark 方法准备的短名，方便扫表。"
    if key == "method":
        return "真正被跑起来的 benchmark 入口方法。"
    if key == "capabilityFamily":
        return "能力大类的编号，用来对齐能力清单。"
    if key == "capabilityItem":
        return "具体能力项的编号，用来对齐能力清单。"
    if key == "capabilityItemLabel":
        return "告诉你这个 benchmark 主要在测什么能力。"
    if key == "ownerSubjectId":
        return "告诉你这项能力由哪个 subject 负责维护。"
    if key == "supportStateLabels":
        return "说明这项能力当前是原生支持、桥接支持还是其他状态。"
    if key == "supportedModes":
        return "声明里写明这个 benchmark 应该有哪些模式结果。"
    if key == "metricLabels":
        return "列出这条 benchmark 关心的指标，比如耗时或分配。"
    if key == "warmupCount":
        return "正式计数前的预热次数。"
    if key == "iterationCount":
        return "正式统计时循环的轮数。"
    if key == "invocationCount":
        return "每轮里调用目标方法的次数。"
    if key.endswith("Status"):
        return f"表示 {_benchmark_mode_display(key)}是已有结果、缺结果，还是本来就不支持。"
    if key.endswith("MeanDurationMs"):
        return f"表示 {_benchmark_mode_display(key)}每次运行的平均耗时，单位是毫秒。"
    if key.endswith("OpsPerSecond"):
        return f"表示 {_benchmark_mode_display(key)}每秒大约能完成多少次调用。"
    if key.endswith("RelativeToManaged"):
        return "表示当前模式耗时相对托管模式的倍数。"
    if key == "isStale":
        return "表示这条结果是否已经被上游标记为过期。"
    if key == "lastRecordedAt":
        return "表示最近一次拿到这条结果的时间。"
    if key == "gitCommit":
        return "表示跑出这条结果时的代码版本。"
    return f"{label} 是性能结果表里的正式字段。"


def _benchmark_hint_why(key: str, label: str) -> str:
    if key in {"platformId", "deviceId", "deviceName"}:
        return "先确认平台和设备，才能避免把不同环境的数字混在一起看。"
    if key in {"subjectId", "stableId", "alias", "method", "capabilityFamily", "capabilityItem", "capabilityItemLabel", "ownerSubjectId", "supportStateLabels"}:
        return "先看清楚是谁、测的是什么，后面的性能数字才有意义。"
    if key in {"supportedModes", "metricLabels", "warmupCount", "iterationCount", "invocationCount"}:
        return "这些列帮助你理解这组 benchmark 结果的统计口径和预期范围。"
    if key.endswith("Status") or key.endswith("MeanDurationMs") or key.endswith("OpsPerSecond") or key.endswith("RelativeToManaged"):
        return "这些列就是读性能快慢、缺项和模式差异时最常看的信息。"
    if key in {"isStale", "lastRecordedAt", "gitCommit"}:
        return "这些列帮助你判断结果还能不能信、是否需要重新跑。"
    return f"保留 {label} 是为了让这张性能表尽量一次看全。"


def _benchmark_hint_values(key: str) -> list[str]:
    if key == "supportedModes":
        return ["managed: 托管运行", "native: 原生代码运行", "interpreter: 解释执行"]
    if key == "metricLabels":
        return ["常见值如 Wall Clock（耗时）、Managed Alloc（托管分配）。"]
    if key in {"warmupCount", "iterationCount", "invocationCount"}:
        return ["直接显示 benchmark 配置里的次数。", "数字更大时统计通常更稳，但跑得也更久。"]
    if key in {"managedStatus", "nativeStatus", "interpreterStatus"}:
        return ["recorded: 已经有结果", "missing: 这模式应该有结果，但当前还没跑到", "unsupported: 这个模式本来就不支持"]
    if key.endswith("MeanDurationMs"):
        return ["单位是毫秒（ms）。", "数值越小越快。"]
    if key.endswith("OpsPerSecond"):
        return ["单位是次/秒。", "数值越大越快。"]
    if key.endswith("RelativeToManaged"):
        return ["1: 和托管模式差不多", "< 1: 比托管模式快", "> 1: 比托管模式慢"]
    if key == "isStale":
        return ["true: 结果可能已经过期", "false: 当前没有过期标记"]
    if key == "gitCommit":
        return ["通常会是 commit sha 或其缩写。"]
    if key == "lastRecordedAt":
        return ["显示最近一次写入结果的时间。"]
    return ["直接显示 benchmark authority 里的原始值。"]


def _benchmark_hint_rule(key: str) -> str:
    if key == "supportedModes":
        return "这里只说明按声明应该有哪些模式，不代表这些模式现在都已经有结果。"
    if key in {"managedStatus", "nativeStatus", "interpreterStatus"}:
        return "先看这个模式是否在 supportedModes 里；在但没结果就是 missing，不在就是 unsupported。"
    if key.endswith("MeanDurationMs") or key.endswith("OpsPerSecond"):
        return "只有该模式真的有 recorded 结果时，这里才会出现具体数字。"
    if key.endswith("RelativeToManaged"):
        return "用当前模式的平均耗时除以托管模式的平均耗时算出来。"
    if key == "isStale":
        return "直接看上游 benchmark 结果是否带了 stale 标记。"
    return "这列直接展示 benchmark authority 中已经整理好的值，不再额外推断。"


def _benchmark_hint_empty(key: str) -> str:
    if key in {"alias", "deviceName"}:
        return "空值表示上游没有给更好读的名字，只能回退看 stableId 或 deviceId。"
    if key in {"managedStatus", "nativeStatus", "interpreterStatus"}:
        return "空值通常表示上游没有给出模式状态，这时页面会按 supportedModes 补成 missing 或 unsupported。"
    if key.endswith("MeanDurationMs") or key.endswith("OpsPerSecond"):
        return "空值表示这个模式目前没有可用结果，或者结果里没有这个指标。"
    if key.endswith("RelativeToManaged"):
        return "空值表示没有足够数据和托管模式做对比。"
    if key in {"lastRecordedAt", "gitCommit"}:
        return "空值表示这条结果没有附带时间或代码版本信息。"
    if key in {"supportedModes", "metricLabels"}:
        return "空值通常表示上游没有明确写出这项信息。"
    return "空值通常表示上游 benchmark 文档没有提供这项内容。"


def _hint_source(key: str) -> list[str]:
    if key.startswith("stage") or key == "statusReason":
        return ["inventory_generator.py", "verification-v1/spec.md", "subject-test-framework-v1/INDEX.md", "AOT新Feature接入自测规范.md"]
    if key in {"platformId", "deviceId", "deviceName", "managedStatus", "nativeStatus", "interpreterStatus", "lastRecordedAt", "gitCommit", "isStale"} or key.endswith("MeanDurationMs") or key.endswith("OpsPerSecond") or key.endswith("RelativeToManaged"):
        return [
            "verification/archive/master/evidence-claims-master.json",
            "verification/archive/master/result-master.json",
            "verification_projection.py",
        ]
    if key in {"defaultGoalId", "defaultMatrixId"}:
        return ["registry.py", "workspace.manifest.json"]
    if key in {"proofRequired", "benchmarkRequired", "supportStates", "supportStateLabels", "ownerSubjectId", "capabilityFamily", "capabilityFamilyLabel", "capabilityItem", "capabilityItemLabel"}:
        return ["verification/catalog/owners/*/owner.features.json", "capability_coverage.py"]
    return ["compiled_catalog.py", "registry.py", "inventory_source.py"]


def _hint_values(key: str) -> list[str]:
    if key == "contractStatus":
        return ["ok", "missingProof", "missingBenchmark", "unsupported", "proofOptional"]
    if key == "stageRequirement":
        return ["required", "optional", "not-applicable", "unsupported"]
    if key == "stageCoverage":
        return ["covered", "failed", "missing-evidence", "pending-proof", "n/a"]
    if key == "stageStatus":
        return ["covered", "failed", "missing-evidence", "required", "optional", "not-applicable", "unsupported"]
    if key in {"managedStatus", "nativeStatus", "interpreterStatus"}:
        return ["recorded", "missing", "unsupported"]
    if key == "stage":
        return [stage for stage, _ in UNIT_STAGES]
    if key == "supportStates":
        return ["1 NativeGenerated", "2 InterpreterReady", "3 BridgeDispatch", "6 Unsupported"]
    return ["该列的实际取值来自 authority 数据源", "CSV 与 HTML 只消费这里生成的结果"]


def _hint_rule(key: str) -> str:
    if key == "contractStatus":
        return "按 unsupported -> missingBenchmark -> missingProof -> proofOptional -> ok 的优先级压缩能力状态。"
    if key == "stageRequirement":
        return "先判断阶段义务，再与证据覆盖组合；authority JSON 必须保留原值。"
    if key == "stageCoverage":
        return "collector/registry/workspace 直接看机器证据；managed-proof 读取最新托管 proof 运行结果，其余 proof 阶段在未执行前记为 pending-proof。"
    if key == "stageStatus":
        return "CSV 和 HTML 使用压缩后的阶段状态，但不能反向替代 StageRequirement 和 StageCoverage。"
    if key.endswith("RelativeToManaged"):
        return "只有 managed 和当前模式都 recorded 且存在耗时指标时才计算相对比值。"
    if key in {"managedStatus", "nativeStatus", "interpreterStatus"}:
        return "如果模式在 SupportedModes 内但没有记录则为 missing；不在声明范围内则为 unsupported。"
    return "该列只读展示 authority 或派生结果，不允许在 HTML 或 CSV 中反向改写。"


def _hint_empty(key: str) -> str:
    if key in {"managedMeanDurationMs", "managedOpsPerSecond", "nativeMeanDurationMs", "nativeOpsPerSecond", "interpreterMeanDurationMs", "interpreterOpsPerSecond", "nativeRelativeToManaged", "interpreterRelativeToManaged"}:
        return "空值表示该模式没有 recorded 证据，或者当前记录没有该指标。"
    if key in {"defaultGoalId", "defaultMatrixId"}:
        return "空值表示 registry 没有给出默认执行上下文，需要由调用方显式指定。"
    if key in {"alias", "deviceName"}:
        return "空值表示没有额外可读短名，只能回退使用 stableId 或 deviceId。"
    return "空值通常表示上游 authority 没有提供该字段，或当前行不适用该列。"


def _headers(
    columns: tuple[tuple[str, str], ...],
    *,
    scope: str = "default",
) -> list[dict[str, Any]]:
    headers: list[dict[str, Any]] = []
    for key, label in columns:
        if scope == "benchmark":
            hint = {
                "label": label,
                "summary": _benchmark_hint_summary(key, label),
                "definition": _benchmark_hint_definition(key, label),
                "why": _benchmark_hint_why(key, label),
                "source": _hint_source(key),
                "values": _benchmark_hint_values(key),
                "rule": _benchmark_hint_rule(key),
                "emptyMeans": _benchmark_hint_empty(key),
            }
        else:
            hint = {
                "label": label,
                "summary": f"{label} 列用于快速说明当前行在该字段上的核心信息，避免读者反查额外文档。",
                "definition": f"{label} 是 inventory authority 或其派生结果中的正式列，用来描述能力、阶段、设备或状态边界。",
                "why": f"保留 {label} 可以让表格一次性展示最完整的内容，而不是依赖额外说明文件。",
                "source": _hint_source(key),
                "values": _hint_values(key),
                "rule": _hint_rule(key),
                "emptyMeans": _hint_empty(key),
            }
        headers.append(
            {
                "key": key,
                "label": label,
                "hint": hint,
            }
        )
    return headers


def _contract_reason(status: str) -> str:
    return {
        "ok": "能力已经满足当前声明的 proof 与 benchmark 覆盖要求。",
        "missingProof": "能力要求 formal proof，但当前没有 declared UnitTest 覆盖。",
        "missingBenchmark": "能力要求 benchmark，但当前没有 declared benchmark 覆盖。",
        "unsupported": "能力 contract 明确标记为 unsupported。",
        "proofOptional": "能力没有强制 proof 要求，当前按可选覆盖处理。",
    }.get(status, "能力状态由 inventory generator 派生。")


def _build_capability_table(source_payload: dict[str, Any]) -> dict[str, Any]:
    contracts_by_key: dict[tuple[int, int], dict[str, Any]] = {}
    for item in _list_value(source_payload.get("capabilityContracts")):
        if isinstance(item, dict):
            contracts_by_key[_capability_key(item)] = dict(item)

    unit_counts: dict[tuple[int, int], int] = {}
    benchmark_counts: dict[tuple[int, int], int] = {}
    for item in _list_value(source_payload.get("declaredUnitTests")):
        if not isinstance(item, dict):
            continue
        key = _capability_key(item)
        unit_counts[key] = unit_counts.get(key, 0) + 1
        contracts_by_key.setdefault(key, dict(item))
    for item in _list_value(source_payload.get("declaredBenchmarks")):
        if not isinstance(item, dict):
            continue
        key = _capability_key(item)
        benchmark_counts[key] = benchmark_counts.get(key, 0) + 1
        contracts_by_key.setdefault(key, dict(item))

    rows: list[dict[str, Any]] = []
    for key, contract in sorted(contracts_by_key.items()):
        unit_count = unit_counts.get(key, 0)
        benchmark_count = benchmark_counts.get(key, 0)
        if _has_unsupported_contract(contract):
            status = "unsupported"
        elif bool(contract.get("benchmarkRequired")) and benchmark_count == 0:
            status = "missingBenchmark"
        elif bool(contract.get("proofRequired")) and unit_count == 0:
            status = "missingProof"
        elif not bool(contract.get("proofRequired")):
            status = "proofOptional"
        else:
            status = "ok"
        rows.append(
            {
                "ownerSubjectId": str(contract.get("ownerSubjectId") or contract.get("subjectId") or ""),
                "capabilityFamily": key[0],
                "capabilityFamilyLabel": str(contract.get("capabilityFamilyLabel") or ""),
                "capabilityItem": key[1],
                "capabilityItemLabel": str(contract.get("capabilityItemLabel") or ""),
                "supportStates": [_int_value(value) for value in _list_value(contract.get("supportStates"))],
                "supportStateLabels": [str(value) for value in _list_value(contract.get("supportStateLabels"))],
                "proofRequired": bool(contract.get("proofRequired", False)),
                "benchmarkRequired": bool(contract.get("benchmarkRequired", False)),
                "declaredUnitCount": unit_count,
                "declaredBenchmarkCount": benchmark_count,
                "contractStatus": status,
                "statusReason": _contract_reason(status),
            }
        )

    return {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": "Capability Inventory",
        "rowGrain": "capability item",
        "headers": _headers(CAPABILITY_COLUMNS),
        "rows": rows,
    }


def _workspace_unit_ids(source_payload: dict[str, Any]) -> set[str]:
    stable_ids: set[str] = set()
    for collection in _list_value(source_payload.get("workspaceCollections")):
        if not isinstance(collection, dict):
            continue
        for item in _list_value(collection.get("declaredUnitTests")):
            if not isinstance(item, dict):
                continue
            stable_id = str(item.get("stableId") or "").strip()
            if stable_id:
                stable_ids.add(stable_id)
    return stable_ids


def _proof_evidence_by_stage(source_payload: dict[str, Any]) -> dict[str, dict[str, dict[str, Any]]]:
    evidence_by_stage: dict[str, dict[str, dict[str, Any]]] = {}
    proof_items = _list_value(source_payload.get("proofEvidence"))
    for item in proof_items:
        if not isinstance(item, dict):
            continue
        stable_id = str(item.get("stableId") or "").strip()
        stage_kind = str(item.get("stageKind") or "managed-proof").strip()
        if stable_id and stage_kind:
            evidence_by_stage.setdefault(stage_kind, {})[stable_id] = dict(item)
    return evidence_by_stage


def _derive_stage_status(requirement: str, coverage: str) -> str:
    if requirement in {"unsupported", "not-applicable"}:
        return requirement
    if coverage == "failed":
        return "failed"
    if coverage == "missing-evidence":
        return coverage
    if coverage == "covered":
        return coverage
    if requirement in {"required", "optional"}:
        return requirement
    return coverage or requirement or "missing-evidence"


def _stage_reason(
    stage: str,
    requirement: str,
    coverage: str,
    status: str,
    *,
    evidence: dict[str, Any] | None = None,
) -> str:
    if status == "covered":
        if evidence and stage == "managed-proof":
            run_id = str(evidence.get("runId") or "").strip()
            if run_id:
                return f"{stage} 阶段已经有对应的机器可见证据；最新通过记录来自 run {run_id}。"
        return f"{stage} 阶段已经有对应的机器可见证据。"
    if status == "failed":
        if evidence and stage == "managed-proof":
            run_id = str(evidence.get("runId") or "").strip()
            if run_id:
                return f"{stage} 阶段已有正式执行记录，但最新 proof 在 run {run_id} 中失败。"
        return f"{stage} 阶段已有正式执行记录，但最新 proof 失败。"
    if status == "missing-evidence":
        if stage == "managed-proof":
            return f"{stage} 阶段是必经阶段，但当前没有找到对应的托管 proof 执行记录。"
        return f"{stage} 阶段是必经阶段，但当前没有找到对应证据。"
    if status == "required":
        return f"{stage} 阶段是必需义务，当前需要正式执行 proof 才能变为 covered。"
    if status == "optional":
        return f"{stage} 阶段是可选义务，当前没有强制 proof 要求。"
    if status == "not-applicable":
        return f"{stage} 阶段不适用于该 declared UnitTest。"
    if status == "unsupported":
        return f"{stage} 阶段被 capability contract 标记为不支持。"
    return f"{stage} 阶段状态由 requirement={requirement} 与 coverage={coverage} 派生。"


def _unit_stage_values(
    item: dict[str, Any],
    stage: str,
    workspace_ids: set[str],
    proof_evidence_by_stage: dict[str, dict[str, dict[str, Any]]],
) -> tuple[str, str, str, str]:
    stable_id = str(item.get("stableId") or "").strip()
    evidence: dict[str, Any] | None = None
    if stage in {"collector", "registry"}:
        requirement = "required"
        coverage = "covered" if stable_id else "missing-evidence"
    elif stage == "workspace":
        requirement = "required"
        coverage = "covered" if stable_id in workspace_ids else "missing-evidence"
    elif stage == "managed-proof":
        requirement = "required" if bool(item.get("proofRequired", True)) else "optional"
        evidence = dict(proof_evidence_by_stage.get("managed-proof", {}).get(stable_id) or {}) or None
        if evidence:
            proof_status = str(evidence.get("status") or "").strip()
            if proof_status == "ok":
                coverage = "covered"
            elif proof_status in {"fail", "aborted"}:
                coverage = "failed"
            else:
                coverage = "missing-evidence" if requirement == "required" else "pending-proof"
        else:
            coverage = "missing-evidence" if requirement == "required" else "pending-proof"
    elif stage == "native-proof":
        if _has_unsupported_contract(item):
            requirement = "unsupported"
            coverage = "n/a"
        elif _has_native_support(item):
            requirement = "required"
            evidence = dict(proof_evidence_by_stage.get("native-proof", {}).get(stable_id) or {}) or None
            if evidence:
                proof_status = str(evidence.get("status") or "").strip()
                if proof_status == "ok":
                    coverage = "covered"
                elif proof_status in {"fail", "aborted"}:
                    coverage = "failed"
                else:
                    coverage = "pending-proof"
            else:
                coverage = "pending-proof"
        else:
            requirement = "not-applicable"
            coverage = "n/a"
    elif stage == "hotupdate-proof":
        has_hotupdate = _int_value(item.get("hotUpdateCapability")) > 0 or bool(
            _list_value(item.get("hotUpdateCapabilityLabels"))
        )
        requirement = "required" if has_hotupdate else "not-applicable"
        if has_hotupdate:
            evidence = dict(proof_evidence_by_stage.get("hotupdate-proof", {}).get(stable_id) or {}) or None
            if evidence:
                proof_status = str(evidence.get("status") or "").strip()
                if proof_status == "ok":
                    coverage = "covered"
                elif proof_status in {"fail", "aborted"}:
                    coverage = "failed"
                else:
                    coverage = "pending-proof"
            else:
                coverage = "pending-proof"
        else:
            coverage = "n/a"
    else:
        requirement = "unsupported"
        coverage = "n/a"
    status = _derive_stage_status(requirement, coverage)
    return requirement, coverage, status, _stage_reason(stage, requirement, coverage, status, evidence=evidence)


def _build_unit_table(source_payload: dict[str, Any]) -> dict[str, Any]:
    workspace_ids = _workspace_unit_ids(source_payload)
    proof_evidence_by_stage = _proof_evidence_by_stage(source_payload)
    rows: list[dict[str, Any]] = []
    unit_items = [
        dict(item)
        for item in _list_value(source_payload.get("declaredUnitTests"))
        if isinstance(item, dict)
    ]
    for item in sorted(unit_items, key=lambda value: str(value.get("stableId") or "")):
        for stage, stage_order in UNIT_STAGES:
            requirement, coverage, status, reason = _unit_stage_values(
                item,
                stage,
                workspace_ids,
                proof_evidence_by_stage,
            )
            row = {
                "subjectId": str(item.get("subjectId") or ""),
                "stableId": str(item.get("stableId") or ""),
                "alias": str(item.get("alias") or ""),
                "method": _method_label(item),
                "stage": stage,
                "stageOrder": stage_order,
                "capabilityFamily": _int_value(item.get("capabilityFamily")),
                "capabilityItem": _int_value(item.get("capabilityItem")),
                "capabilityItemLabel": str(item.get("capabilityItemLabel") or ""),
                "ownerSubjectId": str(item.get("ownerSubjectId") or ""),
                "supportStateLabels": [str(value) for value in _list_value(item.get("supportStateLabels"))],
                "stageRequirement": requirement,
                "stageCoverage": coverage,
                "stageStatus": status,
                "statusReason": reason,
                "defaultGoalId": str(item.get("defaultGoalId") or ""),
                "defaultMatrixId": str(item.get("defaultMatrixId") or ""),
                "evidenceLabels": [str(value) for value in _list_value(item.get("evidenceLabels"))],
                "priority": _int_value(item.get("priority")),
            }
            rows.append(row)

    return {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": "UnitTest Inventory",
        "rowGrain": "declared-unit-test x stage",
        "headers": _headers(UNIT_COLUMNS),
        "rows": rows,
    }


def _metric(metrics: dict[str, Any], key: str) -> float | str:
    value = metrics.get(key)
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        return float(value)
    return ""


def _mode_status(
    evidence: dict[str, Any],
    mode: str,
    supported_modes: set[str],
) -> tuple[str, float | str, float | str]:
    mode_status = dict(evidence.get("modeStatus") or {})
    raw = dict(mode_status.get(mode) or {})
    status = str(raw.get("status") or "").strip()
    if not status:
        status = "missing" if mode in supported_modes else "unsupported"
    metrics = dict(raw.get("metrics") or {})
    return status, _metric(metrics, "meanDurationMs"), _metric(metrics, "meanOpsPerSecond")


def _relative_to_managed(value: float | str, managed_value: float | str) -> float | str:
    if not isinstance(value, float) or not isinstance(managed_value, float) or managed_value == 0:
        return ""
    return round(value / managed_value, 4)


def _declared_benchmarks_by_id(source_payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    benchmarks: dict[str, dict[str, Any]] = {}
    for item in _list_value(source_payload.get("declaredBenchmarks")):
        if not isinstance(item, dict):
            continue
        stable_id = str(item.get("stableId") or "").strip()
        if stable_id:
            benchmarks[stable_id] = dict(item)
    return benchmarks


def _benchmark_evidence_by_id(source_payload: dict[str, Any]) -> dict[str, list[dict[str, Any]]]:
    evidence_by_id: dict[str, list[dict[str, Any]]] = {}
    for item in _list_value(source_payload.get("benchmarkEvidence")):
        if not isinstance(item, dict):
            continue
        stable_id = str(item.get("stableId") or "").strip()
        if not stable_id:
            continue
        evidence_by_id.setdefault(stable_id, []).append(dict(item))
    return evidence_by_id


def _first_text(primary: dict[str, Any], fallback: dict[str, Any], key: str) -> str:
    return str(primary.get(key) or fallback.get(key) or "")


def _first_int(primary: dict[str, Any], fallback: dict[str, Any], key: str) -> int:
    value = primary.get(key) if primary.get(key) not in (None, "") else fallback.get(key)
    return _int_value(value)


def _first_list(primary: dict[str, Any], fallback: dict[str, Any], key: str) -> list[str]:
    values = _list_value(primary.get(key))
    if not values:
        values = _list_value(fallback.get(key))
    return [str(value) for value in values]


def _build_benchmark_table(source_payload: dict[str, Any]) -> dict[str, Any]:
    declared_by_id = _declared_benchmarks_by_id(source_payload)
    evidence_by_id = _benchmark_evidence_by_id(source_payload)
    benchmark_subjects = {
        str(subject_id): dict(payload or {})
        for subject_id, payload in dict(source_payload.get("benchmarkSubjects") or {}).items()
    }
    rows: list[dict[str, Any]] = []

    for stable_id in sorted(set(declared_by_id) | set(evidence_by_id)):
        declared = declared_by_id.get(stable_id, {"stableId": stable_id})
        evidence_rows = evidence_by_id.get(stable_id) or [{}]
        for evidence in sorted(
            evidence_rows,
            key=lambda value: (str(value.get("deviceId") or ""), str(value.get("platformId") or "")),
        ):
            merged = {**declared, **evidence}
            subject_id = _first_text(evidence, declared, "subjectId")
            subject_payload = benchmark_subjects.get(subject_id, {})
            supported_modes = set(_first_list(evidence, declared, "supportedModes"))
            managed_status, managed_ms, managed_ops = _mode_status(evidence, "managed", supported_modes)
            native_status, native_ms, native_ops = _mode_status(evidence, "native", supported_modes)
            interpreter_status, interpreter_ms, interpreter_ops = _mode_status(evidence, "interpreter", supported_modes)
            row = {
                    "platformId": _first_text(evidence, declared, "platformId"),
                    "deviceId": _first_text(evidence, declared, "deviceId"),
                    "deviceName": _first_text(evidence, declared, "deviceName"),
                    "subjectId": _first_text(evidence, declared, "subjectId"),
                    "stableId": stable_id,
                    "alias": _first_text(evidence, declared, "alias"),
                    "method": _method_label(merged, prefer_workload=True),
                    "capabilityFamily": _first_int(evidence, declared, "capabilityFamily"),
                    "capabilityItem": _first_int(evidence, declared, "capabilityItem"),
                    "capabilityItemLabel": _first_text(evidence, declared, "capabilityItemLabel"),
                    "ownerSubjectId": _first_text(evidence, declared, "ownerSubjectId"),
                    "displayName": _first_text(evidence, declared, "displayName"),
                    "workloadEntry": _first_text(evidence, declared, "workloadEntry"),
                    "entryIndex": _first_int(evidence, declared, "entryIndex"),
                    "assemblyName": _first_text(evidence, declared, "assemblyName"),
                    "declaringType": _first_text(evidence, declared, "declaringType"),
                    "methodName": _first_text(evidence, declared, "methodName"),
                    "methodSignature": _first_text(evidence, declared, "methodSignature"),
                    "category": _first_int(evidence, declared, "category"),
                    "categoryLabel": _first_text(evidence, declared, "categoryLabel"),
                    "metrics": _first_int(evidence, declared, "metrics"),
                    "supportStateLabels": _first_list(evidence, declared, "supportStateLabels"),
                    "supportStates": [_int_value(value) for value in _list_value(evidence.get("supportStates") if evidence.get("supportStates") not in (None, "") else declared.get("supportStates"))],
                    "proofRequired": bool(evidence.get("proofRequired", declared.get("proofRequired", False))),
                    "benchmarkRequired": bool(evidence.get("benchmarkRequired", declared.get("benchmarkRequired", False))),
                    "supportedModes": sorted(
                        supported_modes,
                        key=lambda mode: MODE_ORDER.index(mode) if mode in MODE_ORDER else 999,
                    ),
                    "metricLabels": _first_list(evidence, declared, "metricLabels"),
                    "requires": _first_int(evidence, declared, "requires"),
                    "requirementLabels": _first_list(evidence, declared, "requirementLabels"),
                    "archetype": _first_int(evidence, declared, "archetype"),
                    "archetypeLabel": _first_text(evidence, declared, "archetypeLabel"),
                    "hotUpdateCapability": _first_int(evidence, declared, "hotUpdateCapability"),
                    "hotUpdateCapabilityLabels": _first_list(evidence, declared, "hotUpdateCapabilityLabels"),
                    "warmupCount": _first_int(evidence, declared, "warmupCount"),
                    "iterationCount": _first_int(evidence, declared, "iterationCount"),
                    "invocationCount": _first_int(evidence, declared, "invocationCount"),
                    "managedStatus": managed_status,
                    "managedMeanDurationMs": managed_ms,
                    "managedOpsPerSecond": managed_ops,
                    "nativeStatus": native_status,
                    "nativeMeanDurationMs": native_ms,
                    "nativeOpsPerSecond": native_ops,
                    "nativeRelativeToManaged": _relative_to_managed(native_ms, managed_ms),
                    "interpreterStatus": interpreter_status,
                    "interpreterMeanDurationMs": interpreter_ms,
                    "interpreterOpsPerSecond": interpreter_ops,
                    "interpreterRelativeToManaged": _relative_to_managed(interpreter_ms, managed_ms),
                    "isStale": bool(evidence.get("isStale", False)),
                    "lastRecordedAt": str(evidence.get("lastRecordedAt") or ""),
                    "gitCommit": str(evidence.get("gitCommit") or ""),
                    "modeStatus": dict(evidence.get("modeStatus") or {}),
                    "recordedModes": _first_list(evidence, declared, "recordedModes"),
                    "missingModes": _first_list(evidence, declared, "missingModes"),
                    "staleModes": _first_list(evidence, declared, "staleModes"),
                    "unsupportedModes": _first_list(evidence, declared, "unsupportedModes"),
                    "sourceSubjectPath": _first_text(evidence, declared, "sourceSubjectPath"),
                    "subjectDisplayName": str(subject_payload.get("displayName") or subject_id),
                    "subjectSummaryWorkloadEntry": str(subject_payload.get("summaryWorkloadEntry") or ""),
                    "subjectSummaryBenchmarkCase": dict(subject_payload.get("summaryBenchmarkCase") or {}),
                    "subjectSupportedModesByPlatform": dict(subject_payload.get("supportedModesByPlatform") or {}),
                    "subjectPlatforms": dict(subject_payload.get("platforms") or {}),
                "subjectLatestByDevice": dict(subject_payload.get("latestByDevice") or {}),
            }
            if bool(evidence.get("legacyCompatibilityClaim")):
                row["legacyCompatibilityClaim"] = True
            rows.append(row)

    return {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": BENCHMARK_TABLE_TITLE,
        "rowGrain": "declared-benchmark x device",
        "headers": _headers(BENCHMARK_COLUMNS, scope="benchmark"),
        "rows": rows,
    }


def _csv_value(value: Any) -> str:
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, (list, tuple)):
        return "; ".join(str(item) for item in value)
    if isinstance(value, dict):
        return json.dumps(value, ensure_ascii=False, sort_keys=True)
    if value is None:
        return ""
    return str(value)


def _csv_text(rows: list[dict[str, Any]], columns: tuple[tuple[str, str], ...]) -> str:
    keys = [key for key, _ in columns]
    buffer = io.StringIO()
    writer = csv.DictWriter(buffer, fieldnames=keys, lineterminator="\n")
    writer.writeheader()
    for row in rows:
        writer.writerow({key: _csv_value(row.get(key)) for key in keys})
    return buffer.getvalue()


def _html_payload(
    source_payload: dict[str, Any],
    fixed_views: list[dict[str, Any]],
    capability: dict[str, Any],
    unit_test: dict[str, Any],
    benchmark: dict[str, Any],
) -> dict[str, Any]:
    return {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "generatedAt": str(source_payload.get("generatedAt") or ""),
        "hostPlatform": str(source_payload.get("hostPlatform") or ""),
        "fixedViews": fixed_views,
        "tables": [
            {
                "title": "Capability Inventory",
                "description": "一行一个 capability item，用于扫描能力 owner、proof 与 benchmark 缺口。",
                "headers": capability["headers"],
                "rows": capability["rows"],
            },
            {
                "title": "UnitTest Inventory",
                "description": "一行一个 declared-unit-test x stage，用于查看 collector、registry、workspace 与 proof 阶段状态。",
                "headers": unit_test["headers"],
                "rows": unit_test["rows"],
            },
            {
                "title": benchmark["title"],
                "description": BENCHMARK_TABLE_DESCRIPTION,
                "headers": benchmark["headers"],
                "rows": benchmark["rows"],
            },
        ],
    }


def _template_path() -> Path:
    return Path(__file__).resolve().parent / "templates" / "testing-inventory.html"


def _render_html(payload: dict[str, Any]) -> str:
    template_path = _template_path()
    template = template_path.read_text(encoding="utf-8") if template_path.is_file() else _DEFAULT_HTML
    return template.replace(
        "/* TESTING_INVENTORY_DATA_PLACEHOLDER */",
        f"window.TESTING_INVENTORY_DATA = {json.dumps(payload, ensure_ascii=False)};",
    )


def build_source_inventory_tables(source_payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        "capability": _build_capability_table(source_payload),
        "unitTest": _build_unit_table(source_payload),
        "benchmark": _build_benchmark_table(source_payload),
    }


def _import_verification_modules():
    try:
        from . import verification_bundle as verification_bundle_module
        from . import verification_projection as verification_projection_module
    except ImportError:
        testing_root = Path(__file__).resolve().parent
        if str(testing_root) not in sys.path:
            sys.path.insert(0, str(testing_root))
        import verification_bundle as verification_bundle_module
        import verification_projection as verification_projection_module
    return verification_bundle_module, verification_projection_module


def _inventory_outputs_from_rows(
    source_payload: dict[str, Any],
    *,
    fixed_views: list[dict[str, Any]],
    capability_rows: list[dict[str, Any]],
    unit_rows: list[dict[str, Any]],
    benchmark_rows: list[dict[str, Any]],
) -> dict[str, Any]:
    capability = {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": "Capability Inventory",
        "rowGrain": "capability item",
        "headers": _headers(CAPABILITY_COLUMNS),
        "rows": capability_rows,
    }
    unit_test = {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": "UnitTest Inventory",
        "rowGrain": "declared-unit-test x stage",
        "headers": _headers(UNIT_COLUMNS),
        "rows": unit_rows,
    }
    benchmark = {
        "schemaVersion": INVENTORY_SCHEMA_VERSION,
        "title": BENCHMARK_TABLE_TITLE,
        "rowGrain": "declared-benchmark x device",
        "headers": _headers(BENCHMARK_COLUMNS, scope="benchmark"),
        "rows": benchmark_rows,
    }
    html = _html_payload(source_payload, fixed_views, capability, unit_test, benchmark)
    return {
        "capability": capability,
        "unitTest": unit_test,
        "benchmark": benchmark,
        "csv": {
            "capability": _csv_text(capability["rows"], CAPABILITY_COLUMNS),
            "unitTest": _csv_text(unit_test["rows"], UNIT_CSV_COLUMNS),
            "benchmark": _csv_text(benchmark["rows"], BENCHMARK_COLUMNS),
        },
        "html": html,
        "htmlDocument": _render_html(html),
    }


def build_inventory_outputs(
    source_payload: dict[str, Any],
    *,
    formal_source: dict[str, Any] | None = None,
) -> dict[str, Any]:
    verification_bundle_module, verification_projection_module = _import_verification_modules()
    current_formal_source = formal_source
    if current_formal_source is None:
        bundle = verification_bundle_module.build_verification_bundle(
            source_payload,
            closure_kind="completed",
            scope_code="testing-inventory",
        )
        current_formal_source = bundle["master"]
    projected_tables = verification_projection_module.project_inventory_tables(current_formal_source)
    fixed_views = verification_projection_module.project_homepage_views(current_formal_source)
    return _inventory_outputs_from_rows(
        source_payload,
        fixed_views=fixed_views,
        capability_rows=list(projected_tables.get("capability") or []),
        unit_rows=list(projected_tables.get("unitTest") or []),
        benchmark_rows=list(projected_tables.get("benchmark") or []),
    )


def resolve_inventory_output_root(repo_root: Path, output_root: Path | str | None = None) -> Path:
    if output_root in (None, ""):
        resolved = verification_layout_module.testing_inventory_projection_root(repo_root)
    else:
        resolved = Path(output_root)
        if not resolved.is_absolute():
            resolved = repo_root / resolved
    return resolved


def _artifact_repo_path(repo_root: Path, artifact_path: str) -> Path:
    resolved = Path(artifact_path)
    if not resolved.is_absolute():
        resolved = repo_root / resolved
    return resolved


def _normalized_artifact_path(path: str) -> str:
    return str(path).replace("\\", "/")


def _find_artifact_by_suffix(artifacts: list[str], suffix: str) -> str:
    normalized_suffix = _normalized_artifact_path(suffix)
    for artifact in artifacts:
        if _normalized_artifact_path(artifact).endswith(normalized_suffix):
            return artifact
    raise RuntimeError(f"inventory refresh did not produce required artifact: {suffix}")


def validate_inventory_outputs(
    repo_root: Path,
    payload: dict[str, Any],
    *,
    required_codegen_stub_count: int = 0,
) -> dict[str, Any]:
    artifacts = [str(item).strip() for item in list(payload.get("artifacts") or []) if str(item).strip()]
    if not artifacts:
        raise RuntimeError("inventory refresh produced no artifacts")

    missing_artifacts = [
        artifact
        for artifact in artifacts
        if not _artifact_repo_path(repo_root, artifact).is_file()
    ]
    if missing_artifacts:
        missing_text = ", ".join(sorted(missing_artifacts))
        raise RuntimeError(f"inventory refresh reported missing artifacts: {missing_text}")

    verification_payload = dict(payload.get("verification") or {})
    codegen_stub_paths = [
        artifact
        for artifact in artifacts
        if "verification/evidence/owners/" in _normalized_artifact_path(artifact)
    ]
    if required_codegen_stub_count > 0 and not codegen_stub_paths:
        raise RuntimeError(
            "inventory refresh did not materialize required codegen stub artifacts"
        )

    verification_data = {
        "verificationOutputRoot": str(verification_payload.get("outputRoot") or ""),
        "latestResultPath": _find_artifact_by_suffix(artifacts, "verification/archive/latest/result-snapshot.json"),
        "masterResultPath": _find_artifact_by_suffix(artifacts, "verification/archive/master/result-master.json"),
        "reportSummaryPath": _find_artifact_by_suffix(
            artifacts,
            "verification/archive/reports/completed/testing-inventory/summary.md",
        ),
        "codegenStubPaths": codegen_stub_paths,
    }

    return {
        **payload,
        "artifacts": artifacts,
        "validated": True,
        "verificationData": verification_data,
    }


def _write_verification_navigation(
    repo_root: Path,
    *,
    projection_root: Path,
) -> list[str]:
    verification_root = verification_layout_module.verification_root(repo_root)
    verification_root.mkdir(parents=True, exist_ok=True)

    manifest_path = verification_layout_module.verification_manifest_path(repo_root)
    index_path = verification_layout_module.verification_index_path(repo_root)
    testing_inventory_root = projection_root
    benchmark_root = verification_layout_module.benchmark_projection_root(repo_root)
    archive_root = verification_layout_module.archive_root(repo_root)
    workspaces_root = verification_layout_module.workspaces_root(repo_root)
    verification_all_solution = verification_layout_module.verification_all_solution_path(repo_root)
    verification_all_manifest = verification_layout_module.verification_all_manifest_path(repo_root)

    write_json(
        manifest_path,
        {
            "schemaVersion": 1,
            "rootPath": _relative(repo_root, verification_root),
            "archive": {
                "root": _relative(repo_root, archive_root),
                "latest": _relative(repo_root, verification_layout_module.archive_latest_root(repo_root)),
                "master": _relative(repo_root, verification_layout_module.archive_master_root(repo_root)),
                "reports": _relative(repo_root, verification_layout_module.archive_reports_root(repo_root)),
            },
            "catalog": {
                "root": _relative(repo_root, verification_layout_module.catalog_root(repo_root)),
                "owners": _relative(repo_root, verification_layout_module.owners_root(repo_root)),
                "scenarios": _relative(repo_root, verification_layout_module.scenarios_root(repo_root)),
            },
            "evidence": {
                "root": _relative(repo_root, verification_layout_module.evidence_root(repo_root)),
                "owners": _relative(repo_root, verification_layout_module.evidence_owners_root(repo_root)),
            },
            "workspaces": {
                "root": _relative(repo_root, workspaces_root),
                "verificationAllSolution": _relative(repo_root, verification_all_solution),
                "verificationAllManifest": _relative(repo_root, verification_all_manifest),
            },
            "projections": {
                "root": _relative(repo_root, verification_layout_module.projections_root(repo_root)),
                "testingInventory": _relative(repo_root, testing_inventory_root),
                "benchmark": _relative(repo_root, benchmark_root),
            },
            "commands": {
                "verify": "python build/toolchains/run/run.py verify verification-v1 --json",
                "workspace": "python build/toolchains/run/run.py generate project all --json",
            },
        },
    )

    index_path.write_text(
        "\n".join(
            [
                "# Verification",
                "",
                "## Authority",
                f"- Archive: `{_relative(repo_root, archive_root)}`",
                f"- Catalog: `{_relative(repo_root, verification_layout_module.catalog_root(repo_root))}`",
                f"- Evidence: `{_relative(repo_root, verification_layout_module.evidence_root(repo_root))}`",
                f"- Workspaces: `{_relative(repo_root, workspaces_root)}`",
                f"- Projections: `{_relative(repo_root, verification_layout_module.projections_root(repo_root))}`",
                "",
                "## Main Entries",
                f"- Testing inventory: `{_relative(repo_root, testing_inventory_root / 'inventory.html')}`",
                f"- Benchmark dashboard: `{_relative(repo_root, benchmark_root / 'dashboard.html')}`",
                f"- Total workspace: `{_relative(repo_root, verification_all_solution)}`",
                "",
                "## Commands",
                "- `python build/toolchains/run/run.py verify verification-v1 --json`",
                "- `python build/toolchains/run/run.py generate project all --json`",
                "",
            ]
        ),
        encoding="utf-8",
    )
    return [_relative(repo_root, manifest_path), _relative(repo_root, index_path)]


def write_inventory_outputs(repo_root: Path, *, host_platform: str, output_root: Path) -> dict[str, Any]:
    source_payload = inventory_source_module.collect_inventory_source(repo_root, host_platform=host_platform)
    verification_bundle_module, _ = _import_verification_modules()
    bundle = verification_bundle_module.build_verification_bundle(
        source_payload,
        closure_kind="completed",
        scope_code="testing-inventory",
    )
    verification_payload = verification_bundle_module.write_verification_bundle(
        repo_root,
        source_payload,
        closure_kind="completed",
        scope_code="testing-inventory",
        output_root=verification_layout_module.archive_root(repo_root),
        bundle=bundle,
    )
    outputs = build_inventory_outputs(
        source_payload,
        formal_source=dict(verification_payload.get("masterPayloads") or bundle["master"]),
    )
    output_root.mkdir(parents=True, exist_ok=True)
    files = {
        "source": output_root / "inventory-source.json",
        "capability_json": output_root / "capability-inventory.json",
        "unit_json": output_root / "unit-test-inventory.json",
        "benchmark_json": output_root / "benchmark-inventory.json",
        "capability_csv": output_root / "capability-inventory.csv",
        "unit_csv": output_root / "unit-test-inventory.csv",
        "benchmark_csv": output_root / "benchmark-inventory.csv",
        "html": output_root / "inventory.html",
    }
    write_json(files["source"], source_payload)
    write_json(files["capability_json"], outputs["capability"])
    write_json(files["unit_json"], outputs["unitTest"])
    write_json(files["benchmark_json"], outputs["benchmark"])
    files["capability_csv"].write_text(outputs["csv"]["capability"], encoding="utf-8")
    files["unit_csv"].write_text(outputs["csv"]["unitTest"], encoding="utf-8")
    files["benchmark_csv"].write_text(outputs["csv"]["benchmark"], encoding="utf-8")
    files["html"].write_text(outputs["htmlDocument"], encoding="utf-8")
    artifacts = [_relative(repo_root, path) for path in files.values()]
    artifacts.extend(_write_verification_navigation(repo_root, projection_root=output_root))
    artifacts.extend(str(item) for item in list(verification_payload.get("artifacts") or []))
    payload = {
        "outputRoot": _relative(repo_root, output_root),
        "artifacts": artifacts,
        "tables": {
            "capability": {"rowCount": len(outputs["capability"]["rows"])},
            "unitTest": {"rowCount": len(outputs["unitTest"]["rows"])},
            "benchmark": {"rowCount": len(outputs["benchmark"]["rows"])},
        },
        "verification": verification_payload,
        "sourceSummary": {
            "codegenStubCount": len(_list_value(source_payload.get("codegenStubs"))),
        },
    }
    return validate_inventory_outputs(
        repo_root,
        payload,
        required_codegen_stub_count=len(_list_value(source_payload.get("codegenStubs"))),
    )


def refresh_inventory_outputs(
    repo_root: Path,
    *,
    host_platform: str,
    output_root: Path | str | None = None,
) -> dict[str, Any]:
    return write_inventory_outputs(
        repo_root,
        host_platform=host_platform,
        output_root=resolve_inventory_output_root(repo_root, output_root),
    )


_DEFAULT_HTML = """<!DOCTYPE html>
<html lang="zh-CN">
<head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Testing Inventory</title></head>
<body><script>/* TESTING_INVENTORY_DATA_PLACEHOLDER */</script></body>
</html>
"""
