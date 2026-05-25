"""Deep Test Detail Dashboard — Chinese semantic data, parsing, benchmark logic, and CLI.

Generates a self-contained interactive HTML page showing:
  - Family-level overview with pass/fail/skip/error counts
  - Per-family expandable cards with 13-stage breakdown
  - Chinese semantic descriptions for every stage and metric
  - Search/filter interactivity (vanilla JS, no dependencies)

Usage:
    python -m testing.foundation-dll.verification.reporting.dashboard [--report path/to/batch-report.json]
"""

from __future__ import annotations

import json
import math
import re
import sys
import time
from functools import lru_cache
from pathlib import Path
from typing import Any

# ──────────────────────────────────────────────────────────────────────
# Module A: Chinese semantic description database
# ──────────────────────────────────────────────────────────────────────

STAGE_CN: dict[str, dict[str, str | dict[str, str]]] = {
    "preflight": {
        "name": "预检",
        "description": "验证测试环境完整性，包括合约文件存在性、入口二进制可用性、必要依赖就绪状态",
        "failure_meaning": "预检失败：测试环境不完整或合约配置有误，整个验证流程无法继续",
        "common_errors": {
            "contract.json not found": "合约文件缺失，请确认 family 目录下存在 contract.json",
            "entry.exe not found": "入口二进制文件缺失，需要先完成 native 编译",
            "entry-jit.exe not found": "JIT 入口二进制文件缺失，需要先完成 JIT 编译",
        },
    },
    "codegen": {
        "name": "代码生成 (AOT)",
        "description": "基于合约描述执行 AOT 代码生成：生成入口点、运行 IL2CPP 编译、执行 dispatch 代码生成、构建 entry.exe",
        "failure_meaning": "AOT 代码生成失败：可能存在合约定义错误、代码生成器缺陷或不支持的 IL 模式",
        "common_errors": {
            "CodegenError": "代码生成器内部错误，请检查合约定义和 IL 指令兼容性",
            "MissingMethod": "合约中引用的方法在程序集中不存在",
            "BuildFailed": "native 编译失败，请检查 CMake 配置和编译器输出",
            "Access to the path is denied": "文件访问被拒绝，通常由前次运行的残留文件锁引起，清理 codegen 目录后重试",
        },
    },
    "jit_codegen": {
        "name": "JIT 代码生成",
        "description": "使用 JIT 编译器生成即时编译代码，构建 entry-jit.exe，验证运行时动态代码生成路径",
        "failure_meaning": "JIT 代码生成失败：可能由于运行时类型加载错误或不支持的 IL 序列",
        "common_errors": {
            "JitCompilationError": "JIT 编译错误，请检查 IL 序列是否包含不支持的指令",
            "TypeLoadException": "类型加载异常，JIT 无法解析合约中引用的类型",
        },
    },
    "managed_fact": {
        "name": "托管事实验证 (.NET8)",
        "description": "运行 .NET8 托管事实验证，通过自动生成的 C# harness 逐方法调用被测方法，确保 IL 语义与 .NET 运行时一致",
        "failure_meaning": "托管事实验证不通过：被测方法在 .NET8 上抛出预期外异常或返回非预期结果",
        "common_errors": {
            "FactAssertionFailed": "托管事实断言失败，方法执行结果与预期不一致",
            "UnexpectedException": "被测方法抛出预期外异常，可能由 harness 参数输入不当引起",
        },
    },
    "fact": {
        "name": "AOT 正确性验证",
        "description": "运行 AOT 编译输出的事实验证（Fact），确认 IL 语义在 AOT 路径下执行结果与预期一致",
        "failure_meaning": "AOT 事实验证不通过：AOT 代码生成产生了语义偏差或执行结果不符合预期",
        "common_errors": {
            "FactAssertionFailed": "事实验证断言失败，AOT 执行结果与预期不一致",
            "ExecutionTimeout": "事实验证执行超时，可能存在死循环或性能退化",
        },
    },
    "fact_jit": {
        "name": "JIT 正确性验证",
        "description": "运行 JIT 编译输出的事实验证（Fact），通过解释器调度执行，确认 IL 语义在 JIT 路径下正确",
        "failure_meaning": "JIT 事实验证不通过：JIT 代码生成产生了语义偏差或执行结果不符合预期",
        "common_errors": {
            "FactAssertionFailed": "JIT 事实验证断言失败，JIT 执行结果与预期不一致",
        },
    },
    "audit": {
        "name": "机制与原则审计",
        "description": "对生成的代码进行静态分析审计：检测存根实现、假通过检查、原则对齐验证（VIOLATION/CONCERN/ALIGNED）",
        "failure_meaning": "审计发现违规项：生成的代码违反了 IL2CPP 的设计原则或机制约束，需要修复",
        "common_errors": {
            "PrincipleViolation": "原则违规：生成的代码模式不符合 IL2CPP 架构设计原则",
            "MechanismMismatch": "机制不匹配：使用了错误的运行时机制路径",
        },
    },
    "asm_compare": {
        "name": "汇编对比 (JIT vs AOT)",
        "description": "对比 JIT 和 AOT 两条路径生成的汇编代码，分析指令数差异、IR 膨胀比和代码质量",
        "failure_meaning": "汇编对比发现显著差异：可能存在 AOT 代码退化、冗余指令或语义不一致",
        "common_errors": {
            "AsmMismatch": "汇编指令序列不匹配，AOT 与 JIT 存在语义级差异",
            "IrExpansionExceeded": "IR 膨胀比超出阈值，AOT 生成的代码量异常增大",
        },
    },
    "microbench": {
        "name": "微基准测试 (解释器)",
        "description": "在解释器模式下执行微基准测试，测量单个操作的原语级性能基线（如 dispatch 开销、内存分配耗时等）",
        "failure_meaning": "微基准测试失败：可能由于运行时错误或测试夹具初始化失败",
        "common_errors": {
            "BenchmarkCrash": "基准测试进程崩溃",
            "SetupFailure": "测试夹具初始化失败",
        },
    },
    "benchmark": {
        "name": "基准测试 (三路对比)",
        "description": "在 AOT 和 JIT 两条路径上执行全量基准测试，对比每秒操作数（ops/s）、Native 优胜比等性能指标",
        "failure_meaning": "基准测试失败：部分或全部技术路径的性能数据不可用，可能由于进程崩溃或超时",
        "common_errors": {
            "NoBaseline": "无基线数据，首次运行需要建立性能基线",
            "ProcessExitFailure": "基准测试进程退出码非零",
        },
    },
    "hotupdate": {
        "name": "热更新 AOT 正确性",
        "description": "在热更新（HotUpdate）场景下验证 AOT 事实验证路径的语义正确性，运行 entry.exe --hotupdate",
        "failure_meaning": "热更新 AOT 事实验证失败：热更新后的 AOT 代码存在语义问题",
        "common_errors": {
            "HotupdateFactFailed": "热更新事实验证断言失败",
        },
    },
    "hotupdate_aot_benchmark": {
        "name": "热更新 AOT 基准测试",
        "description": "在热更新场景下执行 AOT 基准测试，测量修补后 AOT 代码的性能表现",
        "failure_meaning": "热更新 AOT 基准测试失败：可能由于热更新流程中的性能退化",
        "common_errors": {
            "BenchmarkFailed": "热更新基准测试执行失败",
        },
    },
    "hotupdate_jit_fact": {
        "name": "热更新 JIT 正确性",
        "description": "在热更新场景下验证 JIT 事实验证路径的语义正确性，运行 entry-jit.exe --hotupdate",
        "failure_meaning": "热更新 JIT 事实验证失败：热更新后的 JIT 代码存在语义问题",
        "common_errors": {},
    },
    "hotupdate_jit_benchmark": {
        "name": "热更新 JIT 基准测试",
        "description": "在热更新场景下执行 JIT 基准测试，测量修补后 JIT 代码的性能表现",
        "failure_meaning": "热更新 JIT 基准测试失败：可能由于热更新流程中的 JIT 性能退化",
        "common_errors": {},
    },
    "dashboard": {
        "name": "仪表盘生成",
        "description": "聚合所有阶段数据生成统一 JSON 仪表盘和 HTML 报告，包含覆盖率、性能对比、IR 翻译指标等的综合摘要",
        "failure_meaning": "仪表盘生成失败：部分阶段数据可能缺失或格式异常",
        "common_errors": {
            "JsonSerializationError": "JSON 序列化失败，报告数据可能包含不可序列化的类型",
            "MissingStageData": "缺少必要阶段数据，仪表盘部分章节可能为空",
        },
    },
}

# All 15 stage keys in pipeline order
STAGE_KEYS = [
    "preflight", "codegen", "jit_codegen", "managed_fact", "fact", "fact_jit",
    "audit", "asm_compare", "microbench", "benchmark",
    "hotupdate", "hotupdate_aot_benchmark", "hotupdate_jit_fact",
    "hotupdate_jit_benchmark", "dashboard",
]

METRIC_CN: dict[str, dict[str, str]] = {
    "nativeFasterRatio": {"name": "Native 性能优胜比", "description": "Native AOT 性能优于托管 JIT 的方法比例", "unit": "%"},
    "managedFasterRatio": {"name": "托管性能优胜比", "description": "托管 JIT 性能优于 Native AOT 的方法比例", "unit": "%"},
    "irExpansionRatio": {"name": "IR 膨胀比", "description": "AOT 生成指令数与原始 IL 指令数的比值，反映代码膨胀程度", "unit": "x"},
    "asmPassRate": {"name": "汇编对比通过率", "description": "汇编一致性检查的通过比例", "unit": "%"},
    "averageSpeedupPercent": {"name": "平均加速比", "description": "Native AOT 相对于托管 JIT 的平均加速百分比", "unit": "%"},
    "opsPerSecond": {"name": "每秒操作数", "description": "单位时间内完成的操作次数，衡量性能的绝对指标", "unit": "ops/s"},
    "stagesPassed": {"name": "通过阶段数", "description": "", "unit": ""},
    "stagesFailed": {"name": "失败阶段数", "description": "", "unit": ""},
    "stagesSkipped": {"name": "跳过阶段数", "description": "", "unit": ""},
    "stagesTotal": {"name": "总阶段数", "description": "", "unit": ""},
    "stagePassRate": {"name": "阶段通过率", "description": "", "unit": "%"},
    "methodCount": {"name": "方法数", "description": "合约中定义的方法数量", "unit": ""},
    "okCount": {"name": "成功数", "description": "成功执行并获得有效数据的方法数", "unit": ""},
    "failCount": {"name": "失败数", "description": "执行失败或超时的方法数", "unit": ""},
    "totalMethods": {"name": "总方法数", "description": "参与测试的方法总数", "unit": ""},
    "averageOpsPerSecond": {"name": "平均每秒操作数", "description": "所有成功方法的平均性能", "unit": "ops/s"},
}

BENCHMARK_COMPARISON_CN: dict[str, dict[str, str]] = {
    "aot_vs_net8": {
        "name": "Native AOT vs .NET 8 JIT",
        "short_name": "AOT vs .NET 8",
        "description": "IL2CPP Native AOT 相对于 .NET 8 JIT 的性能差异",
    },
    "jit_vs_net8": {
        "name": "Native JIT vs .NET 8 JIT",
        "short_name": "JIT vs .NET 8",
        "description": "IL2CPP Native JIT 相对于 .NET 8 JIT 的性能差异",
    },
    "hu_aot_vs_aot": {
        "name": "HotUpdate AOT vs Baseline AOT",
        "short_name": "HU-AOT vs AOT",
        "description": "热更新后 AOT 代码相对于原始 AOT 的性能变化",
    },
    "hu_jit_vs_jit": {
        "name": "HotUpdate JIT vs Baseline JIT",
        "short_name": "HU-JIT vs JIT",
        "description": "热更新后 JIT 代码相对于原始 JIT 的性能变化",
    },
}

STATUS_CN = {
    "passed": "通过",
    "failed": "失败",
    "skipped": "跳过",
    "error": "错误",
    "crashed": "崩溃",
    "unknown": "未知",
}


def stage_cn(key: str) -> dict[str, Any]:
    """Get Chinese description for a stage key, with fallback for unknown stages."""
    default = {
        "name": key,
        "description": "暂无中文描述",
        "failure_meaning": "未知阶段错误",
        "common_errors": {},
    }
    return STAGE_CN.get(key, default)  # type: ignore[return-value]


# ──────────────────────────────────────────────────────────────────────
# Module B: Data parsing
# ──────────────────────────────────────────────────────────────────────


def extract_json_tail(text: str) -> dict[str, Any]:
    """Extract structured data from truncated JSON tail in stdout.

    The stdout_tail captures the last ~2000 chars of the UnifiedReport JSON.
    The tail typically contains: coverage, dashboard, regression,
    and sometimes the last 1-3 stage entries.
    """
    result: dict[str, Any] = {}

    # Extract coverage section (greedy match to capture all props until last })
    m = re.search(r'"coverage"\s*:\s*\{[^}]+\}', text)
    if m:
        try:
            result["coverage"] = json.loads("{" + m.group(0) + "}")["coverage"]
        except (json.JSONDecodeError, KeyError):
            pass

    # Fallback: coverage key may be truncated (e.g., "rage": or "erage":)
    # Extract individual coverage fields directly from text
    if "coverage" not in result:
        coverage = {}
        m = re.search(r'"stagePassRate"\s*:\s*([0-9.]+)', text)
        if m:
            coverage["stagePassRate"] = float(m.group(1))
        for key in ("stagesPassed", "stagesFailed", "stagesSkipped", "stagesTotal"):
            m = re.search(rf'"{key}"\s*:\s*(\d+)', text)
            if m:
                coverage[key] = int(m.group(1))
        if coverage.get("stagesTotal"):  # require at least stagesTotal to consider valid
            result["coverage"] = coverage

    # Extract dashboard section (greedy match)
    m = re.search(r'"dashboard"\s*:\s*\{[^}]*\}', text)
    if m:
        try:
            result["dashboard"] = json.loads("{" + m.group(0) + "}")["dashboard"]
        except (json.JSONDecodeError, KeyError):
            pass

    # Extract regression section (greedy match, handles nested objects)
    m = re.search(r'"regression"\s*:\s*\{.+?"hasRegression"\s*:\s*(?:true|false).+?\}', text)
    if m:
        try:
            result["regression"] = json.loads("{" + m.group(0) + "}")["regression"]
        except (json.JSONDecodeError, KeyError):
            pass

    # Extract partial stage data (last stages may survive in tail, greedy match)
    for stage_key in reversed(STAGE_KEYS):
        # Look for "stage_key": {...status...} pattern
        # Greedy match: [^}]+ captures all properties until the closing }
        pattern = r'"' + re.escape(stage_key) + r'"\s*:\s*\{[^}]+"status"\s*:\s*"[^"]+"[^}]*\}'
        m = re.search(pattern, text)
        if m:
            try:
                stage_data = json.loads("{" + m.group(0) + "}")[stage_key]
                result.setdefault("stages", {})[stage_key] = stage_data
            except (json.JSONDecodeError, KeyError):
                pass

    # Extract overall_status
    m = re.search(r'"overall_status"\s*:\s*"([^"]+)"', text)
    if m:
        result["overall_status"] = m.group(1)

    return result


def parse_family(entry: dict[str, Any]) -> dict[str, Any]:
    """Parse a single family entry from batch-report.json into a uniform format."""
    slug = entry.get("slug") or entry.get("family", "?")
    status = entry.get("status") or entry.get("overall_status", "unknown")
    duration = entry.get("duration_seconds", 0) or entry.get("duration_ms", 0)

    base = {
        "slug": slug,
        "status": status,
        "duration_seconds": duration,
        "coverage": None,
        "dashboard": None,
        "regression": None,
        "stages": {},
        "has_stage_data": False,
    }

    # Priority 1: structured stages/coverage/dashboard from batch_run_all.py direct output
    stages_raw = entry.get("stages")
    if isinstance(stages_raw, dict) and len(stages_raw) > 0:
        base["stages"] = stages_raw
        base["has_stage_data"] = True
    coverage_raw = entry.get("coverage")
    if isinstance(coverage_raw, dict) and coverage_raw.get("stagesTotal"):
        base["coverage"] = coverage_raw
        base["has_stage_data"] = True
    dashboard_raw = entry.get("dashboard")
    if isinstance(dashboard_raw, dict):
        base["dashboard"] = dashboard_raw
    regression_raw = entry.get("regression")
    if isinstance(regression_raw, dict):
        base["regression"] = regression_raw

    # Priority 2: extract from stdout_tail (robust batch runner fallback)
    tail = entry.get("stdout_tail", "")
    if tail and not base["has_stage_data"]:
        extracted = extract_json_tail(tail)
        base["coverage"] = extracted.get("coverage") or base["coverage"]
        base["dashboard"] = extracted.get("dashboard")
        base["regression"] = extracted.get("regression")
        base["stages"] = extracted.get("stages", {}) or base["stages"]
        if extracted.get("coverage") or extracted.get("stages"):
            base["has_stage_data"] = True
        if extracted.get("overall_status"):
            base["status"] = extracted["overall_status"]

    return base


def load_batch_report(path: Path) -> dict[str, Any]:
    """Load and parse batch-report.json into structured format."""
    raw = json.loads(path.read_text(encoding="utf-8"))
    raw["parsed"] = [parse_family(r) for r in raw.get("results", [])]
    return raw


def scan_results_directory(results_root: Path | None = None) -> dict[str, Any]:
    """Scan results directory for per-family unified-verification-report.json files.

    Scheme C: Replaces the single batch-report.json source of truth.
    Each family's cli.py run writes its report independently; the dashboard
    scans the directory to assemble the full view.

    Returns a report dict in the same format as load_batch_report().
    """
    if results_root is None:
        results_root = Path(__file__).resolve().parents[3] / "results" / "foundation-dll"

    if not results_root.exists():
        return {"total_families": 0, "results": [], "parsed": [], "elapsed_seconds": 0}

    results: list[dict[str, Any]] = []
    total_start = time.time()

    # Walk assembly/slug/unified-verification-report.json
    for assembly_dir in sorted(results_root.iterdir()):
        if not assembly_dir.is_dir():
            continue
        for family_dir in sorted(assembly_dir.iterdir()):
            if not family_dir.is_dir():
                continue
            report_file = family_dir / "unified-verification-report.json"
            if not report_file.exists():
                continue
            try:
                raw = json.loads(report_file.read_text(encoding="utf-8"))
                entry = {
                    "slug": raw.get("family", family_dir.name),
                    "status": raw.get("overall_status", "unknown"),
                    "duration_seconds": round(raw.get("duration_ms", 0) / 1000, 1),
                    "stages": raw.get("stages", {}),
                    "coverage": raw.get("coverage"),
                    "dashboard": raw.get("dashboard"),
                    "regression": raw.get("regression"),
                }
                results.append(entry)
            except (OSError, json.JSONDecodeError) as e:
                print(f"  [WARN] Skipping {report_file}: {e}")

    elapsed = round(time.time() - total_start, 1)
    parsed = [parse_family(r) for r in results]

    return {
        "total_families": len(results),
        "results": results,
        "parsed": parsed,
        "elapsed_seconds": elapsed,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
    }


# ──────────────────────────────────────────────────────────────────────
# Benchmark performance comparison (reads perf store JSONL)
# ──────────────────────────────────────────────────────────────────────

_RESULTS_BASE = Path(__file__).resolve().parents[2] / "results" / "foundation-dll" / "System.Private.CoreLib"


def _perf_store_path(slug: str) -> Path:
    """Return the path to benchmark-history.jsonl for a given family slug."""
    return _RESULTS_BASE / slug / "perf" / "benchmark-history.jsonl"


def _load_perf_jsonl(slug: str, stages_data: dict[str, Any] | None = None) -> list[dict[str, Any]]:
    """Load benchmark-history.jsonl records for a slug.

    Falls back to extracting benchmark data from stage details when the
    perf store JSONL file doesn't exist (e.g. batch-report.json has the
    data but auto_save_perf_data hasn't been run yet).

    Note: Not cached when stages_data is provided (dict is unhashable).
    """
    # Try perf store JSONL first (cached)
    cached = _load_perf_jsonl_cached(slug)
    if cached:
        return cached

    # Fallback: extract from stage details (batch-report.json embedded data)
    if stages_data:
        records = _extract_benchmark_from_stages(stages_data, slug=slug)
        if records:
            return records

    return []


@lru_cache(maxsize=128)
def _load_perf_jsonl_cached(slug: str) -> list[dict[str, Any]]:
    """Load benchmark-history.jsonl records from disk (cached by slug)."""
    path = _perf_store_path(slug)
    if not path.exists():
        return []
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                try:
                    records.append(json.loads(line))
                except json.JSONDecodeError:
                    pass
    return records


def _extract_benchmark_from_stages(stages_data: dict[str, Any], slug: str = "") -> list[dict[str, Any]]:
    """Extract benchmark records from stage details (JSONL-like format).

    Reads the 'benchmark' stage details and produces records in the same
    format as _load_perf_jsonl, so _compute_benchmark_comparisons and
    _build_benchmark_table can consume them identically.
    """
    bm = stages_data.get("benchmark", {})
    bm_details = bm.get("details", {}) if isinstance(bm, dict) else {}
    if not isinstance(bm_details, dict):
        return []

    records: list[dict[str, Any]] = []
    slug = slug or stages_data.get("slug", stages_data.get("family", "?"))

    # Map stage detail keys to technology labels
    _TECH_MAP = {
        "native-aot": "chaos-aot",
        "native-jit": "chaos-jit",
    }

    for detail_key, bench_data in bm_details.items():
        technology = _TECH_MAP.get(detail_key)
        if not technology or not isinstance(bench_data, dict):
            continue

        results = bench_data.get("results") or []
        for i, res in enumerate(results):
            if not isinstance(res, dict):
                continue
            records.append({
                "timestamp": "",
                "slug": slug,
                "technology": technology,
                "methodSubjectId": "",
                "methodIndex": res.get("methodIndex", i),
                "metrics": {
                    "elapsedMilliseconds": res.get("elapsedMilliseconds", 0),
                    "opsPerSecond": res.get("opsPerSecond", 0),
                    "calibratedMs": res.get("calibratedMs", 0),
                },
                "iterations": res.get("iterations", 100000),
                "status": "completed" if "error" not in res else "error",
            })

    # HotUpdate benchmark stages
    _HU_TECH_MAP = {
        "hotupdate_aot_benchmark": "chaos-hu-aot",
        "hotupdate_jit_benchmark": "chaos-hu-jit",
    }
    for stage_key, technology in _HU_TECH_MAP.items():
        sd = stages_data.get(stage_key, {})
        sd_details = sd.get("details", {}) if isinstance(sd, dict) else {}
        if not isinstance(sd_details, dict):
            continue
        results = sd_details.get("results") or []
        for i, res in enumerate(results):
            if not isinstance(res, dict):
                continue
            metrics: dict[str, Any] = {
                "elapsedMilliseconds": res.get("elapsedMilliseconds", 0),
            }
            if "postPatchNsPerOp" in res:
                metrics["postPatchNsPerOp"] = res["postPatchNsPerOp"]
            records.append({
                "timestamp": "",
                "slug": slug,
                "technology": technology,
                "methodSubjectId": "",
                "methodIndex": res.get("methodIndex", i),
                "metrics": metrics,
                "iterations": res.get("iterations", 100000),
                "status": "completed" if "error" not in res else "error",
            })

    return records


def _geometric_mean(values: list[float]) -> float:
    """Compute geometric mean of a list of positive values."""
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values)) if log_sum else 0.0


def _compute_benchmark_comparisons(slug: str, stages_data: dict[str, Any] | None = None) -> dict[str, Any]:
    """Compute benchmark performance comparisons for a family.

    Reads the perf store JSONL and computes geometric mean opsPerSecond
    for each available technology, then derives the 4 comparison metrics.

    Filters out runs where ALL methods have elapsedMilliseconds < 1.0ms,
    which indicates a measurement artifact where the benchmark harness
    failed to correctly time the operations (typically all methods report
    unrealistically identical high throughput).

    When stages_data is provided, falls back to extracting benchmark
    records from the stage details if the perf store JSONL doesn't exist.
    """
    records = _load_perf_jsonl(slug, stages_data=stages_data)
    if not records:
        return {"has_any_data": False, "technologies": {}, "comparisons": {}}

    # Group by technology and then by timestamp
    from collections import defaultdict
    by_tech: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for r in records:
        by_tech[r.get("technology", "")].append(r)

    def _is_valid_run(recs: list[dict[str, Any]]) -> bool:
        """Check if a run's measurements are valid.

        Skips runs where ALL methods complete in under 1ms (measurement artifact).
        Valid runs have at least some methods taking measurable time (>1ms).
        """
        elapsed_values = [
            r.get("metrics", {}).get("elapsedMilliseconds", 0)
            for r in recs if r.get("metrics", {}).get("elapsedMilliseconds", 0) > 0
        ]
        if not elapsed_values:
            return False
        return max(elapsed_values) >= 1.0

    def _latest_valid_run(recs: list[dict[str, Any]]) -> list[dict[str, Any]]:
        """Find the latest timestamp group that passes validation."""
        by_ts: dict[str, list[dict[str, Any]]] = defaultdict(list)
        for r in recs:
            by_ts[r.get("timestamp", "")].append(r)

        valid_groups = [
            (ts, group) for ts, group in by_ts.items()
            if _is_valid_run(group)
        ]
        if not valid_groups:
            return []
        # Return the latest valid group
        valid_groups.sort(key=lambda x: x[0])
        return valid_groups[-1][1]

    technologies: dict[str, dict[str, Any] | None] = {}
    for tech, recs in by_tech.items():
        latest = _latest_valid_run(recs)
        if not latest:
            technologies[tech] = None
            continue

        # Extract opsPerSecond (or convert from postPatchNsPerOp)
        ops_values = []
        statuses = set()
        method_count = len(latest)
        for r in latest:
            statuses.add(r.get("status", "unknown"))
            metrics = r.get("metrics", {})

            # HotUpdate records use postPatchNsPerOp instead of opsPerSecond
            if "postPatchNsPerOp" in metrics and metrics["postPatchNsPerOp"] > 0:
                ops_values.append(1e9 / metrics["postPatchNsPerOp"])
            elif "opsPerSecond" in metrics and metrics["opsPerSecond"] > 0:
                ops_values.append(metrics["opsPerSecond"])

        if ops_values:
            gm_ops = _geometric_mean(ops_values)
            technologies[tech] = {
                "gm_ops": round(gm_ops, 0),
                "method_count": method_count,
                "ok_count": len(ops_values),
                "status": "completed" if "error" not in statuses else "partial",
            }
        else:
            technologies[tech] = None

    # Compute comparisons
    def _gm(tech: str) -> float | None:
        t = technologies.get(tech)
        return t["gm_ops"] if t and t.get("gm_ops", 0) > 0 else None

    def _ratio_pct(num: float | None, den: float | None) -> dict[str, Any]:
        if num is not None and den is not None and den > 0 and num > 0:
            ratio = num / den
            return {"ratio": round(ratio, 2), "pct": round((ratio - 1) * 100, 1)}
        return {"ratio": None, "pct": None}

    chaos_aot = _gm("chaos-aot")
    chaos_jit = _gm("chaos-jit")
    net8_jit = _gm("net8-jit")
    hu_aot = _gm("chaos-hu-aot")
    hu_jit = _gm("chaos-hu-jit")

    comparisons = {
        "aot_vs_net8": {**_ratio_pct(chaos_aot, net8_jit),
                         "status": "completed" if (chaos_aot and net8_jit) else "无托管数据"},
        "jit_vs_net8": {**_ratio_pct(chaos_jit, net8_jit),
                         "status": "completed" if (chaos_jit and net8_jit) else "无托管数据"},
        "hu_aot_vs_aot": {**_ratio_pct(hu_aot, chaos_aot),
                           "status": "completed" if (hu_aot and chaos_aot) else "无数据"},
        "hu_jit_vs_jit": {**_ratio_pct(hu_jit, chaos_jit),
                           "status": "completed" if (hu_jit and chaos_jit) else "无数据"},
    }

    return {
        "has_any_data": any(t is not None for t in technologies.values()),
        "technologies": technologies,
        "comparisons": comparisons,
    }


def _short_method_label(method_subject_id: str) -> str:
    """Extract a short readable label from a methodSubjectId.

    Input:  "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)"
    Output: "ToChar(UInt32)"
    """
    if not method_subject_id:
        return "?"
    # Extract after ::
    m = re.search(r'::(\w+)', method_subject_id)
    method_name = m.group(1) if m else ""
    # Extract parameter types from the part after return type
    # Format: ...::MethodName:ReturnType(ParamType1,ParamType2)
    m = re.search(r'\(([^)]*)\)', method_subject_id)
    params = m.group(1) if m else ""
    # Shorten type names: System.UInt32 -> UInt32, etc.
    param_parts = []
    for p in params.split(","):
        p = p.strip()
        p = p.split(".")[-1]  # Take last segment after dot
        p = p.replace("`1", "").replace("`2", "")
        param_parts.append(p)
    param_str = ",".join(param_parts)
    if method_name and param_str:
        return f"{method_name}({param_str})"
    # Fallback: return last meaningful segment
    parts = method_subject_id.split("::")
    return parts[-1] if len(parts) > 1 else method_subject_id.split("/")[-1]


_BENCHMARK_TECH_ORDER = [
    "net8-jit", "net10-jit", "mono",
    "chaos-aot", "chaos-jit",
    "chaos-hu-aot", "chaos-hu-jit",
]
_BENCHMARK_TECH_LABELS = {
    "net8-jit": ".NET 8 JIT", "net10-jit": ".NET 10 JIT", "mono": "Mono",
    "chaos-aot": "Chaos AOT", "chaos-jit": "Chaos JIT",
    "chaos-hu-aot": "HU AOT", "chaos-hu-jit": "HU JIT",
}
_BENCHMARK_TECH_SHORT = {
    "net8-jit": "NET8", "net10-jit": "NET10", "mono": "Mono",
    "chaos-aot": "AOT", "chaos-jit": "JIT",
    "chaos-hu-aot": "HU-A", "chaos-hu-jit": "HU-J",
}


# ──────────────────────────────────────────────────────────────────────
# Utility helper (used by both data and rendering modules)
# ──────────────────────────────────────────────────────────────────────


def _escape_html(s: str) -> str:
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace('"', "&quot;")


# ──────────────────────────────────────────────────────────────────────
# Re-export generate_html from the renderer module (lazy to avoid circular import)
# ──────────────────────────────────────────────────────────────────────


def generate_html(report: dict[str, Any]) -> str:
    """Lazy delegate to dashboard_renderer.generate_html to avoid circular import."""
    from .dashboard_renderer import generate_html as _generate_html
    return _generate_html(report)


# ──────────────────────────────────────────────────────────────────────
# Module C: CLI entry point
# ──────────────────────────────────────────────────────────────────────


def main() -> None:
    import argparse

    _testing_root = Path(__file__).resolve().parents[3]

    parser = argparse.ArgumentParser(description="Generate deep test detail dashboard HTML")
    parser.add_argument("--report", default=None, help="Path to batch-report.json")
    parser.add_argument("--from-results", action="store_true",
                        help="Scan testing/results/foundation-dll/ for per-family reports")
    parser.add_argument("--output", "-o", default=None, help="Output HTML path")
    parser.add_argument("--run-missing", action="store_true",
                        help="Run benchmark stages for families without perf data (requires native binaries)")

    args = parser.parse_args()

    if args.from_results:
        print("Scanning results directory for per-family reports...")
        report = scan_results_directory()
        total = report.get("total_families", 0)
        n_with_data = sum(1 for f in report.get("parsed", []) if f.get("has_stage_data"))
        print(f"Found {total} families ({n_with_data} with stage data)")
        # Derive output path: testing/results/deep-dashboard.html
        report_path = _testing_root / "results" / "batch-report.json"
    elif args.report:
        report_path = Path(args.report)
        if not report_path.exists():
            print(f"Error: report not found: {report_path}")
            sys.exit(1)
        print(f"Loading report: {report_path}")
        report = load_batch_report(report_path)
        total = len(report.get("parsed", []))
        n_with_data = sum(1 for f in report["parsed"] if f.get("has_stage_data"))
        print(f"Parsed {total} families ({n_with_data} with stage data)")
    else:
        # Default: try batch-report.json, fall back to results scan
        report_path = _testing_root / "results" / "batch-report.json"
        if report_path.exists():
            print(f"Loading report: {report_path}")
            report = load_batch_report(report_path)
        else:
            print("No batch-report.json found, scanning results directory...")
            report = scan_results_directory()
        total = len(report.get("parsed", []))
        n_with_data = sum(1 for f in report["parsed"] if f.get("has_stage_data"))
        print(f"Found {total} families ({n_with_data} with stage data)")

    # ── Run benchmarks for families without perf data ──────────────
    if args.run_missing:
        _run_benchmarks_for_missing_families(report)

    html = generate_html(report)

    if args.output:
        output_path = Path(args.output)
    else:
        output_path = report_path.with_name("deep-dashboard.html")

    output_path.write_text(html, encoding="utf-8")
    print(f"Dashboard written to: {output_path}")
    print(f"File size: {len(html) / 1024:.0f} KB")


def _run_benchmarks_for_missing_families(report: dict[str, Any]) -> None:
    """Run benchmark-only pipeline for families that lack perf data.

    Uses subprocess to call the CLI (which handles its own sys.path setup).
    Only runs for families with native binaries (entry.exe / entry-jit.exe).
    """
    _testing_root = Path(__file__).resolve().parents[3]
    _families_root = _testing_root / "foundation-dll" / "System.Private.CoreLib"
    _cli_script = str(_testing_root / "foundation-dll" / "verification" / "entry_points" / "cli.py")

    parsed = report.get("parsed") or [parse_family(r) for r in report.get("results", [])]
    families_without_data = [f for f in parsed if not _has_any_benchmark_data(f["slug"])]

    if not families_without_data:
        print("  All families already have benchmark data, nothing to run.")
        return

    print(f"\n  Families without benchmark data: {len(families_without_data)}")
    print(f"  Checking native binary availability...")

    _SKIP_ALL = ["preflight", "codegen", "jit_codegen", "fact", "fact_jit",
                 "audit", "asm_compare", "microbench", "hotupdate",
                 "hotupdate_jit_fact"]

    import subprocess

    ran_count = 0
    for f in families_without_data:
        slug = f["slug"]
        native_dir = _families_root / slug / "native"

        has_aot = (native_dir / "entry.exe").exists()
        has_jit = (native_dir / "entry-jit.exe").exists()

        if not has_aot and not has_jit:
            print(f"    {slug}: no native binaries found, skipping")
            continue

        print(f"    {slug}: running benchmark stages (AOT={'Y' if has_aot else 'N'}, JIT={'Y' if has_jit else 'N'})...")

        try:
            result = subprocess.run(
                [sys.executable, _cli_script, slug,
                 "--skip"] + _SKIP_ALL,
                capture_output=True, text=True, timeout=300,
                cwd=str(_testing_root.parent),
            )
            ran_count += 1
            if result.returncode != 0:
                print(f"    {slug}: done (exit code {result.returncode})")
            else:
                print(f"    {slug}: done (all benchmark stages passed)")
        except subprocess.TimeoutExpired:
            print(f"    {slug}: benchmark timed out (>5 min), skipping")
        except Exception as e:
            import traceback
            print(f"    {slug}: benchmark run failed: {e}")
            traceback.print_exc()

    # Clear perf data cache so re-parsed families get the new data
    _load_perf_jsonl_cached.cache_clear()
    # Re-parse the families to pick up new perf data
    report["parsed"] = [parse_family(r) for r in report.get("results", [])]

    n_with_data = sum(1 for f in report["parsed"] if _has_any_benchmark_data(f["slug"]))
    print(f"  Ran benchmarks for {ran_count} families. Total families with data: {n_with_data}/{len(parsed)}")


def _has_any_benchmark_data(slug: str) -> bool:
    """Quick check if a family has any benchmark data in the perf store."""
    result = _compute_benchmark_comparisons(slug)
    return result.get("has_any_data", False)


if __name__ == "__main__":
    main()
