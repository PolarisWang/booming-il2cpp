"""Deep Test Detail Dashboard — Chinese semantic HTML for all 47 families.

Generates a self-contained interactive HTML page showing:
  - Family-level overview with pass/fail/skip/error counts
  - Per-family expandable cards with 13-stage breakdown
  - Chinese semantic descriptions for every stage and metric
  - Search/filter interactivity (vanilla JS, no dependencies)

Usage:
    python deep_dashboard.py [--report path/to/batch-report.json]
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

_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]
_TESTING_ROOT = _CORE_PACKAGE.parent

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
    slug = entry.get("slug", "?")
    status = entry.get("status", "unknown")
    duration = entry.get("duration_seconds", 0)

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

    # Priority 1: structured stages/coverage from batch_run_all.py direct output
    stages_raw = entry.get("stages")
    if isinstance(stages_raw, dict) and len(stages_raw) > 0:
        base["stages"] = stages_raw
        base["has_stage_data"] = True
    coverage_raw = entry.get("coverage")
    if isinstance(coverage_raw, dict) and coverage_raw.get("stagesTotal"):
        base["coverage"] = coverage_raw
        base["has_stage_data"] = True

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


# ──────────────────────────────────────────────────────────────────────
# Benchmark performance comparison (reads perf store JSONL)
# ──────────────────────────────────────────────────────────────────────

_RESULTS_BASE = _TESTING_ROOT / "results" / "foundation-dll" / "System.Private.CoreLib"


def _perf_store_path(slug: str) -> Path:
    """Return the path to benchmark-history.jsonl for a given family slug."""
    return _RESULTS_BASE / slug / "perf" / "benchmark-history.jsonl"


@lru_cache(maxsize=128)
def _load_perf_jsonl(slug: str) -> list[dict[str, Any]]:
    """Load benchmark-history.jsonl records for a slug (cached)."""
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


def _geometric_mean(values: list[float]) -> float:
    """Compute geometric mean of a list of positive values."""
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values)) if log_sum else 0.0


def _compute_benchmark_comparisons(slug: str) -> dict[str, Any]:
    """Compute benchmark performance comparisons for a family.

    Reads the perf store JSONL and computes geometric mean opsPerSecond
    for each available technology, then derives the 4 comparison metrics.

    Filters out runs where ALL methods have elapsedMilliseconds < 1.0ms,
    which indicates a measurement artifact where the benchmark harness
    failed to correctly time the operations (typically all methods report
    unrealistically identical high throughput).
    """
    records = _load_perf_jsonl(slug)
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


def _build_benchmark_section(comparisons: dict[str, Any]) -> str:
    """Build HTML for the benchmark performance comparison section."""
    if not comparisons.get("has_any_data"):
        return ""

    comps = comparisons.get("comparisons", {})
    techs = comparisons.get("technologies", {})

    rows = ""
    for key, order in [
        ("aot_vs_net8", 0),
        ("jit_vs_net8", 1),
        ("hu_aot_vs_aot", 2),
        ("hu_jit_vs_jit", 3),
    ]:
        c = comps.get(key, {})
        cn = BENCHMARK_COMPARISON_CN.get(key, {})
        status = c.get("status", "无数据")
        pct = c.get("pct")

        if pct is not None:
            arrow = "&#9650;" if pct >= 0 else "&#9660;"
            color = "#2e7d32" if pct >= 0 else "#c62828"
            prefix = "+" if pct >= 0 else ""
            value_html = f'<div class="comp-value" style="color:{color};">{arrow} {prefix}{pct:.1f}%</div>'
        else:
            value_html = f'<div class="comp-value na">{status}</div>'

        rows += f"""\
    <div class="comparison-card">
      <div class="comp-label">{cn.get("name", key)}</div>
      {value_html}
      <div class="comp-detail">{cn.get("description", "")}</div>
    </div>"""

    # Build technology summary line
    tech_summary = ""
    if techs:
        tech_parts = []
        for tech in ["chaos-aot", "chaos-jit", "chaos-hu-aot", "chaos-hu-jit", "net8-jit", "net10-jit", "mono"]:
            t = techs.get(tech)
            if t:
                ops_str = _fmt_metric_value("averageOpsPerSecond", t["gm_ops"])
                tech_parts.append(f'<span class="tech-tag">{tech}: {ops_str}</span>')
        if tech_parts:
            tech_summary = f'<div class="tech-summary">{" | ".join(tech_parts)}</div>'

    return f"""\
  <div class="benchmark-section">
    <h4 class="section-title">性能对比 &#9881;</h4>
    {tech_summary}
    <div class="comparison-grid">
{rows}
    </div>
  </div>"""


# ──────────────────────────────────────────────────────────────────────
# Module C: HTML generation
# ──────────────────────────────────────────────────────────────────────


def _status_icon(status: str) -> str:
    icons = {"passed": "&#10003;", "failed": "&#10007;", "skipped": "&mdash;", "error": "!"}
    return icons.get(status, "?")


def _status_label(status: str) -> str:
    return STATUS_CN.get(status, status)


def _fmt_duration(ms: int | float) -> str:
    """Format milliseconds to human-readable Chinese string."""
    if not ms:
        return ""
    seconds = ms / 1000
    if seconds >= 60:
        m = int(seconds // 60)
        s = int(seconds % 60)
        return f"{m}分{s}秒"
    if seconds >= 1:
        return f"{seconds:.1f}秒"
    return f"{ms:.0f}毫秒"


def _fmt_seconds(s: float) -> str:
    if not s:
        return ""
    if s >= 60:
        m = int(s // 60)
        sec = int(s % 60)
        return f"{m}分{sec}秒"
    return f"{s:.1f}秒"


def _safe_json(obj: Any) -> str:
    """JSON serialize with Chinese-safe encoding."""
    return json.dumps(obj, ensure_ascii=False)


def _escape_html(s: str) -> str:
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace('"', "&quot;")


def _fmt_metric_value(key: str, value: float) -> str:
    """Format a metric value with appropriate precision."""
    unit = METRIC_CN.get(key, {}).get("unit", "")
    if key in ("irExpansionRatio",):
        return f"{value:.2f}{unit}"
    if key in ("stagePassRate", "asmPassRate", "nativeFasterRatio", "managedFasterRatio", "averageSpeedupPercent"):
        return f"{value:.1f}{unit}"
    if isinstance(value, float):
        if value >= 1e6:
            return f"{value / 1e6:.2f}M{unit}"
        if value >= 1e3:
            return f"{value / 1e3:.1f}K{unit}"
        return f"{value:.1f}{unit}"
    return str(value)


def generate_css() -> str:
    return """\
* { margin: 0; padding: 0; box-sizing: border-box; }
body { font-family: -apple-system, BlinkMacSystemFont, "Microsoft YaHei", "PingFang SC", "Noto Sans SC", "Segoe UI", Roboto, sans-serif; background: #f0f2f5; color: #333; padding: 0; }
.header { background: linear-gradient(135deg, #1a237e, #283593); color: #fff; padding: 24px 32px; }
.header h1 { font-size: 1.4rem; font-weight: 600; }
.header .subtitle { font-size: 0.85rem; opacity: 0.8; margin-top: 4px; }
.stats { display: flex; gap: 12px; padding: 16px 32px; background: #fff; border-bottom: 1px solid #e0e0e0; flex-wrap: wrap; }
.stat-card { padding: 8px 20px; text-align: center; border-radius: 6px; min-width: 90px; }
.stat-card .num { font-size: 1.6rem; font-weight: 700; }
.stat-card .label { font-size: 0.75rem; color: #888; margin-top: 2px; }
.stat-card.total .num { color: #1565c0; }
.stat-card.pass .num { color: #2e7d32; }
.stat-card.fail .num { color: #c62828; }
.stat-card.rate .num { color: #f57f17; }
.filter-bar { display: flex; gap: 10px; padding: 12px 32px; background: #fff; border-bottom: 1px solid #e0e0e0; align-items: center; flex-wrap: wrap; }
.filter-bar input { padding: 6px 12px; border: 1px solid #ccc; border-radius: 4px; font-size: 0.85rem; width: 200px; }
.filter-bar input:focus { outline: none; border-color: #1565c0; }
.filter-btn { padding: 6px 14px; border: 1px solid #ccc; border-radius: 14px; background: #fff; cursor: pointer; font-size: 0.8rem; transition: all 0.15s; }
.filter-btn:hover { background: #f5f5f5; }
.filter-btn.active { background: #1565c0; color: #fff; border-color: #1565c0; }
.filter-btn.active-all { background: #1565c0; color: #fff; border-color: #1565c0; }
.filter-btn.active-pass { background: #2e7d32; color: #fff; border-color: #2e7d32; }
.filter-btn.active-fail { background: #c62828; color: #fff; border-color: #c62828; }
.filter-btn.toggle-all { margin-left: auto; background: #f5f5f5; }
.container { padding: 16px 32px; max-width: 1200px; margin: 0 auto; }
.family-count { font-size: 0.85rem; color: #888; padding: 8px 32px 0; }
.family-card { background: #fff; border-radius: 8px; box-shadow: 0 1px 3px rgba(0,0,0,0.10); margin-bottom: 8px; overflow: hidden; transition: box-shadow 0.2s; }
.family-card:hover { box-shadow: 0 2px 8px rgba(0,0,0,0.15); }
.card-header { display: flex; align-items: center; padding: 14px 20px; cursor: pointer; gap: 12px; user-select: none; }
.card-header .expand-icon { width: 20px; text-align: center; color: #999; font-size: 0.8rem; flex-shrink: 0; }
.card-header .slug { font-weight: 600; font-size: 0.95rem; flex: 1; font-family: 'Consolas', 'Courier New', monospace; }
.card-header .coverage-summary { font-size: 0.8rem; color: #888; margin-right: 8px; }
.badge { display: inline-block; padding: 2px 10px; border-radius: 10px; font-size: 0.75rem; font-weight: 600; }
.badge-passed { background: #e8f5e9; color: #2e7d32; }
.badge-failed { background: #fbe9e7; color: #c62828; }
.badge-skipped { background: #fff8e1; color: #f57f17; }
.badge-error { background: #f3e5f5; color: #6a1b9a; }
.badge-na { background: #f5f5f5; color: #9e9e9e; }
.card-body { display: none; padding: 0 20px 16px; }
.card-body.open { display: block; }
.stage-table { width: 100%; border-collapse: collapse; margin-top: 8px; font-size: 0.85rem; }
.stage-table th { text-align: left; padding: 8px 10px; border-bottom: 2px solid #eee; font-weight: 600; color: #555; font-size: 0.8rem; }
.stage-table td { padding: 7px 10px; border-bottom: 1px solid #f5f5f5; vertical-align: top; }
.stage-row { transition: background 0.1s; }
.stage-row:hover { background: #fafafa; }
.stage-row.status-passed { border-left: 3px solid #4caf50; }
.stage-row.status-failed { border-left: 3px solid #f44336; }
.stage-row.status-skipped { border-left: 3px solid #ff9800; }
.stage-row.status-error { border-left: 3px solid #9c27b0; }
.stage-row.status-na { border-left: 3px solid #bdbdbd; }
.stage-icon { display: inline-block; width: 20px; text-align: center; font-weight: 700; }
.stage-icon.passed { color: #4caf50; }
.stage-icon.failed { color: #f44336; }
.stage-icon.skipped { color: #ff9800; }
.stage-icon.error { color: #9c27b0; }
.stage-icon.na { color: #bdbdbd; }
.stage-name-cn { font-weight: 600; }
.stage-name-en { font-size: 0.75rem; color: #999; }
.stage-desc { font-size: 0.8rem; color: #666; margin-top: 2px; }
.stage-error { background: #fff5f5; border-radius: 4px; padding: 6px 10px; margin-top: 4px; font-size: 0.8rem; }
.stage-error .err-title { font-weight: 600; color: #c62828; }
.stage-error .err-msg { color: #555; margin-top: 2px; }
.stage-error .err-cn { color: #888; margin-top: 2px; font-style: italic; }
.coverage-bar { height: 6px; border-radius: 3px; background: #e0e0e0; margin: 10px 0; overflow: hidden; }
.coverage-fill { height: 100%; border-radius: 3px; transition: width 0.5s; }
.metrics-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(180px, 1fr)); gap: 8px; margin-top: 8px; }
.metric-item { background: #fafafa; border-radius: 4px; padding: 8px 12px; }
.metric-item .m-name { font-size: 0.75rem; color: #888; }
.metric-item .m-value { font-size: 1rem; font-weight: 600; color: #333; }
.metric-item .m-desc { font-size: 0.7rem; color: #aaa; margin-top: 2px; }
.na-notice { text-align: center; padding: 24px; color: #999; font-size: 0.9rem; }
.footer { text-align: center; padding: 20px; color: #999; font-size: 0.8rem; }
.hidden { display: none !important; }

/* Benchmark performance comparison section */
.benchmark-section { margin-top: 16px; padding-top: 12px; border-top: 1px solid #eee; }
.benchmark-section .section-title { font-size: 0.85rem; color: #555; margin-bottom: 8px; }
.tech-summary { font-size: 0.75rem; color: #888; margin-bottom: 8px; line-height: 1.6; }
.tech-tag { background: #f5f5f5; padding: 1px 6px; border-radius: 3px; white-space: nowrap; }
.comparison-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
.comparison-card { background: #fafafa; border: 1px solid #e8e8e8; border-radius: 6px; padding: 10px 14px; }
.comparison-card .comp-label { font-size: 0.8rem; color: #666; font-weight: 600; }
.comparison-card .comp-value { font-size: 1.2rem; font-weight: 700; margin: 4px 0; }
.comparison-card .comp-value.na { color: #bdbdbd; font-size: 0.8rem; font-weight: 400; }
.comparison-card .comp-detail { font-size: 0.72rem; color: #999; }
@media (max-width: 700px) {
  .stats { padding: 12px 16px; gap: 8px; }
  .filter-bar { padding: 10px 16px; }
  .filter-bar input { width: 140px; }
  .container { padding: 12px 16px; }
  .card-header { padding: 12px 14px; }
  .card-body { padding: 0 14px 12px; }
  .metrics-grid { grid-template-columns: repeat(2, 1fr); }
}
"""


def generate_js(report_data_json: str) -> str:
    return f"""\
const DATA = {report_data_json};
const parsed = DATA.parsed || [];

function toggleCard(id) {{
  const body = document.getElementById('body-' + id);
  const icon = document.getElementById('icon-' + id);
  if (!body) return;
  const isOpen = body.classList.toggle('open');
  icon.textContent = isOpen ? '▼' : '▶';
}}

function expandAll() {{
  parsed.forEach((_, i) => {{
    const body = document.getElementById('body-' + i);
    const icon = document.getElementById('icon-' + i);
    if (body) {{ body.classList.add('open'); icon.textContent = '▼'; }}
  }});
}}

function collapseAll() {{
  parsed.forEach((_, i) => {{
    const body = document.getElementById('body-' + i);
    const icon = document.getElementById('icon-' + i);
    if (body) {{ body.classList.remove('open'); icon.textContent = '▶'; }}
  }});
}}

function filterFamilies() {{
  const search = (document.getElementById('searchInput').value || '').toLowerCase();
  const activeStatus = document.querySelector('.filter-btn.active');
  const statusFilter = activeStatus ? activeStatus.dataset.status : 'all';

  let visible = 0;
  parsed.forEach((f, i) => {{
    const card = document.getElementById('card-' + i);
    if (!card) return;

    const matchSearch = !search || f.slug.toLowerCase().includes(search);
    const matchStatus = statusFilter === 'all' || f.status === statusFilter;

    if (matchSearch && matchStatus) {{
      card.classList.remove('hidden');
      visible++;
    }} else {{
      card.classList.add('hidden');
    }}
  }});

  document.getElementById('visibleCount').textContent = visible;
}}

function setFilter(btn, status) {{
  document.querySelectorAll('.filter-btn[data-status]').forEach(b => {{
    b.classList.remove('active', 'active-all', 'active-pass', 'active-fail');
  }});
  btn.classList.add('active');
  if (status === 'all') btn.classList.add('active-all');
  else if (status === 'passed') btn.classList.add('active-pass');
  else if (status === 'failed') btn.classList.add('active-fail');
  filterFamilies();
}}
"""


def _build_stage_rows_from_coverage(slug: str, coverage: dict[str, Any] | None) -> str:
    """Build a summary row from coverage data when individual stage data is unavailable."""
    if not coverage:
        return ""
    passed = coverage.get("stagesPassed", 0)
    failed = coverage.get("stagesFailed", 0)
    skipped = coverage.get("stagesSkipped", 0)
    total = coverage.get("stagesTotal", 15)

    return f"""\
<tr class="stage-row status-{'passed' if failed == 0 else 'failed'}">
  <td style="width:22px;"><span class="stage-icon {'passed' if failed == 0 else 'failed'}">{'&#10003;' if failed == 0 else '&#10007;'}</span></td>
  <td colspan="2">
    <div class="stage-name-cn">汇总</div>
    <div class="stage-name-en">coverage summary</div>
  </td>
  <td colspan="2">
    <div style="font-size:0.85rem;">
      <span style="color:#2e7d32;">&#10003; 通过 {passed}</span>
      &nbsp;
      <span style="color:#c62828;">&#10007; 失败 {failed}</span>
      &nbsp;
      <span style="color:#ff9800;">&mdash; 跳过 {skipped}</span>
      &nbsp;|&nbsp; 共计 {total} 阶段
    </div>
    <div class="stage-desc" style="margin-top:4px;">
      &#9432; 阶段级详情数据在 batch 输出的 JSON 尾部被截断，无法完整恢复。
      可通过 <code>python -m testing.foundation_dll._core.python.cli {slug}</code> 单独运行以查看各阶段详情。
    </div>
  </td>
</tr>"""


def _build_stage_rows(slug: str, stages: dict[str, Any], has_data: bool, coverage: dict[str, Any] | None) -> str:
    """Build HTML for the stage detail table."""
    if not has_data:
        return f'<tr><td colspan="5" class="na-notice">暂无阶段数据（该 family 在第一轮 batch 中运行，未捕获详细输出）。<br>可通过 <code>python -m testing.foundation_dll._core.python.cli {slug}</code> 单独运行以获取详细数据。</td></tr>'

    # Check if we have actual stage data (from stdout_tail extraction)
    # Only show individual stage rows if at least 5 stages have data
    has_individual_stages = sum(1 for k in STAGE_KEYS if stages.get(k)) >= 5

    if not has_individual_stages:
        # We have coverage summary but no individual stage data (truncated tail)
        # Show the coverage summary row with explanation
        return _build_stage_rows_from_coverage(slug, coverage)

    rows = ""
    for key in STAGE_KEYS:
        cn = stage_cn(key)
        s = stages.get(key, {})
        stage_status = s.get("status", "unknown") if s else "na"
        status_cn = _status_label(stage_status)
        icon = _status_icon(stage_status)
        icon_cls = stage_status if stage_status in ("passed", "failed", "skipped", "error") else "na"
        duration = s.get("duration_ms", 0) if s else 0
        dur_str = _fmt_duration(duration) if duration else ""
        summary = _escape_html(s.get("summary", "")) if s else ""
        errors = s.get("errors", []) if s else []

        # Build error detail HTML
        error_html = ""
        for err in errors:
            err_text = _escape_html(err)
            # Try to match against common errors
            cn_errors = cn.get("common_errors", {})
            cn_hint = ""
            for pattern, hint in cn_errors.items():  # type: ignore[union-attr]
                if pattern.lower() in err.lower():
                    cn_hint = hint
                    break
            hint_html = f'<div class="err-cn">&#9656; {_escape_html(cn_hint)}</div>' if cn_hint else ""
            error_html += f'<div class="stage-error"><div class="err-title">&#9888; 错误</div><div class="err-msg">{err_text}</div>{hint_html}</div>'

        # Build failure_meaning if stage failed
        fail_hint = ""
        if stage_status == "failed":
            fail_hint = f'<div class="err-cn">{_escape_html(str(cn.get("failure_meaning", "")))}</div>'

        status_badge = f'<span class="badge badge-{stage_status if stage_status in ("passed","failed","skipped","error") else "na"}">{icon} {status_cn}</span>'

        rows += f"""\
<tr class="stage-row status-{stage_status if stage_status in ("passed","failed","skipped","error","na") else "na"}">
  <td style="width:22px;"><span class="stage-icon {icon_cls}">{icon}</span></td>
  <td style="width:180px;">
    <div class="stage-name-cn">{cn["name"]}</div>
    <div class="stage-name-en">{key}</div>
  </td>
  <td style="width:80px;">{status_badge}</td>
  <td style="width:80px;">{dur_str}</td>
  <td>
    <div class="stage-desc">{cn["description"]}</div>
    {fail_hint}
    {error_html}
    {("<div class=\"stage-desc\">" + summary[:200] + "</div>") if summary and not error_html else ""}
  </td>
</tr>"""

    return rows


def _build_metrics_grid(coverage: dict[str, Any] | None, dashboard: dict[str, Any] | None) -> str:
    """Build metrics grid from coverage and dashboard data."""
    items = ""

    if coverage:
        for key in ("stagesPassed", "stagesFailed", "stagesSkipped", "stagesTotal", "stagePassRate"):
            val = coverage.get(key)
            if val is not None:
                mc = METRIC_CN.get(key, {})
                items += f'<div class="metric-item"><div class="m-name">{mc.get("name", key)}</div><div class="m-value">{_fmt_metric_value(key, val)}</div></div>'

    if dashboard:
        key_ratios = dashboard.get("keyRatios", {}) if isinstance(dashboard, dict) else {}
        if isinstance(key_ratios, dict):
            for key in ("nativeFasterRatio", "managedFasterRatio", "irExpansionRatio", "asmPassRate"):
                val = key_ratios.get(key)
                if val is not None:
                    mc = METRIC_CN.get(key, {})
                    items += f'<div class="metric-item"><div class="m-name">{mc.get("name", key)}</div><div class="m-value">{_fmt_metric_value(key, val)}</div><div class="m-desc">{mc.get("description", "")}</div></div>'

        avg_speedup = dashboard.get("averageSpeedupPercent") if isinstance(dashboard, dict) else None
        if avg_speedup is not None:
            mc = METRIC_CN.get("averageSpeedupPercent", {})
            items += f'<div class="metric-item"><div class="m-name">{mc.get("name", "averageSpeedupPercent")}</div><div class="m-value">{_fmt_metric_value("averageSpeedupPercent", avg_speedup)}</div><div class="m-desc">{mc.get("description", "")}</div></div>'

    return items if items else '<div class="na-notice">暂无指标数据</div>'


def _build_coverage_bar(coverage: dict[str, Any] | None) -> str:
    """Build a visual coverage bar."""
    if not coverage:
        return ""
    passed = coverage.get("stagesPassed", 0)
    failed = coverage.get("stagesFailed", 0)
    skipped = coverage.get("stagesSkipped", 0)
    total = coverage.get("stagesTotal", 15)
    non_skipped = total - skipped
    rate = coverage.get("stagePassRate", 0)
    if total == 0:
        return ""

    # Bar segments: passed/failed proportion of TOTAL (including skipped as neutral)
    p_pct = passed / total * 100 if total else 0
    f_pct = failed / total * 100 if total else 0

    detail_parts = [f"通过 {passed}"]
    if failed:
        detail_parts.append(f"失败 {failed}")
    if skipped:
        detail_parts.append(f"跳过 {skipped}")

    color = "#4caf50" if rate >= 80 else ("#ff9800" if rate >= 50 else "#f44336")
    skipped_color = "#e0e0e0"

    bar_html = f"""\
    <div class="coverage-bar" style="display:flex;height:8px;border-radius:4px;overflow:hidden;background:{skipped_color};">"""
    if p_pct > 0:
        bar_html += f'<div style="width:{p_pct:.0f}%;background:{color};"></div>'
    if f_pct > 0:
        bar_html += f'<div style="width:{f_pct:.0f}%;background:#f44336;"></div>'
    if skipped > 0:
        s_pct = skipped / total * 100
        bar_html += f'<div style="width:{s_pct:.0f}%;background:#e0e0e0;"></div>'
    bar_html += "</div>"

    return f"""\
<div style="display:flex;justify-content:space-between;font-size:0.8rem;color:#888;margin-top:6px;">
  <span>阶段覆盖率 ({', '.join(detail_parts)})</span>
  <span>{passed}/{non_skipped} 非跳过 ({rate:.0f}%)</span>
</div>
{bar_html}"""


def _build_family_card(f: dict[str, Any], index: int) -> str:
    """Build a single expandable family card."""
    slug_raw = f["slug"]  # unescaped for file lookup
    slug = _escape_html(slug_raw)
    status = f.get("status", "unknown")
    status_cn = _status_label(status)
    status_cls = status if status in ("passed", "failed", "skipped", "error", "crashed") else "na"
    duration = _fmt_seconds(f.get("duration_seconds", 0))
    coverage = f.get("coverage")
    dashboard_data = f.get("dashboard")
    stages = f.get("stages", {})
    has_data = f.get("has_stage_data", False)

    # Coverage summary for header
    cov_summary = ""
    if coverage:
        p = coverage.get("stagesPassed", 0)
        t = coverage.get("stagesTotal", 0)
        cov_summary = f'{p}/{t} 阶段通过' if t else ''

    stage_rows = _build_stage_rows(slug, stages, has_data, coverage)
    metrics_grid = _build_metrics_grid(coverage, dashboard_data)
    cov_bar = _build_coverage_bar(coverage)
    benchmark_section = _build_benchmark_section(_compute_benchmark_comparisons(slug_raw))

    return f"""\
<div class="family-card" id="card-{index}" data-status="{status}">
  <div class="card-header" onclick="toggleCard({index})">
    <span class="expand-icon" id="icon-{index}">&#9654;</span>
    <span class="slug">{slug}</span>
    <span class="coverage-summary">{cov_summary}</span>
    <span class="badge badge-{status_cls}">{status_cn}</span>
    {('<span style="font-size:0.75rem;color:#999">' + duration + '</span>') if duration else ''}
  </div>
  <div class="card-body" id="body-{index}">
    <table class="stage-table">
      <thead>
        <tr><th></th><th>阶段</th><th>状态</th><th>耗时</th><th>详情</th></tr>
      </thead>
      <tbody>
        {stage_rows}
      </tbody>
    </table>
    {cov_bar}
    {('<h4 style="font-size:0.85rem;color:#555;margin:12px 0 4px;">关键指标</h4><div class="metrics-grid">' + metrics_grid + '</div>') if metrics_grid and metrics_grid != '<div class="na-notice">暂无指标数据</div>' else ''}
    {benchmark_section}
  </div>
</div>"""


def generate_html(report: dict[str, Any]) -> str:
    """Generate the complete self-contained HTML dashboard."""
    parsed = report.get("parsed") or [parse_family(r) for r in report.get("results", [])]
    total = len(parsed)
    n_passed = sum(1 for f in parsed if f.get("status") == "passed")
    n_failed = sum(1 for f in parsed if f.get("status") == "failed")
    n_skipped = sum(1 for f in parsed if f.get("status") == "skipped")
    n_crashed = sum(1 for f in parsed if f.get("status") == "crashed")
    n_other = total - n_passed - n_failed - n_skipped - n_crashed
    pass_rate = round(n_passed / total * 100) if total > 0 else 0
    elapsed = report.get("elapsed_seconds", 0)
    elapsed_str = f"{int(elapsed // 60)}分{int(elapsed % 60)}秒" if elapsed else ""

    # Build family cards
    cards_html = ""
    for i, f in enumerate(parsed):
        cards_html += _build_family_card(f, i)

    # Build JSON data for JS
    js_data = _safe_json({"parsed": [{"slug": f["slug"], "status": f["status"]} for f in parsed]})

    now_str = time.strftime("%Y-%m-%d %H:%M:%S")

    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Foundation-DLL 深度验证仪表盘</title>
<style>
{generate_css()}
</style>
</head>
<body>

<div class="header">
  <h1>Foundation-DLL 深度验证仪表盘</h1>
  <div class="subtitle">
    System.Private.CoreLib &mdash; {total} 个 Family &mdash;
    生成时间: {now_str} &mdash;
    总耗时: {elapsed_str}
  </div>
</div>

<div class="stats">
  <div class="stat-card total">
    <div class="num">{total}</div>
    <div class="label">总 Families</div>
  </div>
  <div class="stat-card pass">
    <div class="num">{n_passed}</div>
    <div class="label">通过 &#10003;</div>
  </div>
  <div class="stat-card fail">
    <div class="num">{n_failed}</div>
    <div class="label">失败 &#10007;</div>
  </div>
  {f'<div class="stat-card" style="background:#fff8e1;"><div class="num" style="color:#f57f17;">{n_skipped}</div><div class="label">跳过</div></div>' if n_skipped > 0 else ''}
  {f'<div class="stat-card" style="background:#fbe9e7;"><div class="num" style="color:#c62828;">{n_crashed}</div><div class="label">崩溃</div></div>' if n_crashed > 0 else ''}
  <div class="stat-card rate">
    <div class="num">{pass_rate}%</div>
    <div class="label">通过率</div>
  </div>
  {f'<div class="stat-card"><div class="num">{n_other}</div><div class="label">其他</div></div>' if n_other > 0 else ''}
</div>

<div class="filter-bar">
  <input type="text" id="searchInput" placeholder="搜索 family 名称..." oninput="filterFamilies()">
  <button class="filter-btn active active-all" data-status="all" onclick="setFilter(this, 'all')">全部</button>
  <button class="filter-btn" data-status="passed" onclick="setFilter(this, 'passed')">通过</button>
  <button class="filter-btn" data-status="failed" onclick="setFilter(this, 'failed')">失败</button>
  <button class="filter-btn toggle-all" onclick="expandAll()">展开全部 &#9660;</button>
  <button class="filter-btn toggle-all" onclick="collapseAll()">折叠全部 &#9654;</button>
</div>

<div class="family-count">
  显示 <span id="visibleCount">{total}</span> / {total} 个 family
</div>

<div class="container">
  {cards_html}
</div>

<div class="footer">
  Foundation-DLL 深度验证仪表盘 &mdash; 自动生成
</div>

<script>
{generate_js(js_data)}
</script>
</body>
</html>"""
    return html


# ──────────────────────────────────────────────────────────────────────
# Module D: CLI entry point
# ──────────────────────────────────────────────────────────────────────


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Generate deep test detail dashboard HTML")
    parser.add_argument("--report", default=None, help="Path to batch-report.json")
    parser.add_argument("--output", "-o", default=None, help="Output HTML path")
    parser.add_argument("--run-missing", action="store_true",
                        help="Run benchmark stages for families without perf data (requires native binaries)")

    args = parser.parse_args()

    if args.report:
        report_path = Path(args.report)
    else:
        report_path = _TESTING_ROOT / "results" / "batch-report.json"

    if not report_path.exists():
        print(f"Error: report not found: {report_path}")
        sys.exit(1)

    print(f"Loading report: {report_path}")
    report = load_batch_report(report_path)
    total = len(report.get("parsed", []))
    n_with_data = sum(1 for f in report["parsed"] if f.get("has_stage_data"))
    print(f"Parsed {total} families ({n_with_data} with stage data)")

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
    parsed = report.get("parsed") or [parse_family(r) for r in report.get("results", [])]
    families_without_data = [f for f in parsed if not _has_any_benchmark_data(f["slug"])]

    if not families_without_data:
        print("  All families already have benchmark data, nothing to run.")
        return

    print(f"\n  Families without benchmark data: {len(families_without_data)}")
    print(f"  Checking native binary availability...")

    import subprocess
    _FAMILIES_ROOT = _TESTING_ROOT / "foundation-dll" / "System.Private.CoreLib"
    _CLI_SCRIPT = str(_TESTING_ROOT / "foundation-dll" / "_core" / "python" / "cli.py")
    _SKIP_ALL = ["preflight", "codegen", "jit_codegen", "fact", "fact_jit",
                 "audit", "asm_compare", "microbench", "hotupdate",
                 "hotupdate_jit_fact"]

    ran_count = 0
    for f in families_without_data:
        slug = f["slug"]
        native_dir = _FAMILIES_ROOT / slug / "native"

        has_aot = (native_dir / "entry.exe").exists()
        has_jit = (native_dir / "entry-jit.exe").exists()

        if not has_aot and not has_jit:
            print(f"    {slug}: no native binaries found, skipping")
            continue

        print(f"    {slug}: running benchmark stages (AOT={'Y' if has_aot else 'N'}, JIT={'Y' if has_jit else 'N'})...")

        try:
            result = subprocess.run(
                [sys.executable, _CLI_SCRIPT, slug,
                 "--skip"] + _SKIP_ALL,
                capture_output=True, text=True, timeout=300,
                cwd=str(_TESTING_ROOT.parent),
            )
            ran_count += 1
            if result.returncode != 0:
                # Non-zero exit when some benchmark stages fail is expected
                # perf data still saved for passed stages
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
    _load_perf_jsonl.cache_clear()
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
