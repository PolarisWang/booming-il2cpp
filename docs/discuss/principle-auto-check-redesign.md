# Principle Auto Check 重新设计

## Status: 设计讨论

## 1. 现状问题

### 1.1 检查项描述与实际代码不一致

| SKILL.md 声明 | 实现状态 |
|---|---|
| p1_lowering | 实现（文件级） |
| p1_no_redundant_branch | 实现 |
| p1_no_unnecessary_indirection | 实现 |
| p1_benchmark | 实现 |
| p2_template_hygiene | 实现 |
| p2_layer_correctness（监控） | **不存在** |
| p2_no_code_duplication（监控） | **不存在** |
| p2_abstraction_balance（监控） | **不存在** |
| p3_patch_entry | 实现 |
| p3_patchdata | 实现 |
| p3_no_hardcoded_state（监控） | **不存在** |
| — | p2_no_bare_new（未在 SKILL.md 中记录） |

### 1.2 架构问题

1. **无插件化** — 所有检查硬编码在 `principle_auto_checks.py` 中，新增检查需改核心文件
2. **结果模型耦合** — `PrincipleCheckResult` 与调用方 `mechanism_audit.py` 紧耦合
3. **无权重系统** — 所有检查等权，`VIOLATION > CONCERN > ALIGNED` 硬编码
4. **单 family 运行** — 无跨 family 聚合分析（如 code duplicate 检测需要跨 family 比较）
5. **无缓存** — 每次运行重新扫描全部模板（`p2_template_hygiene`）
6. **check_p1_lowering 是文件级** — 检测到文件内有一处 `chaos_eval_stack` 就认为所有方法通过
7. **check_p1_benchmark 依赖外部产物** — benchmark-comparison-report.json 不存在时返回 NOT_APPLICABLE，掩盖了 benchmark 未运行的事实

---

## 2. 设计目标

1. **插件化检查注册** — 新增检查只需实现一个函数 + 声明 metadata，无需改 aggregator
2. **可配置权重** — 每项检查可独立配置是否阻塞门禁、影响 overall 评分
3. **跨 family 检查** — `p2_no_code_duplication` 和 `p3_no_hardcoded_state` 需要跨 family 聚合
4. **per-method 粒度** — lowering 检查下沉到方法级
5. **独立于 pipeline** — 可以在 codegen 产物上独立运行，不依赖 benchmark/hotupdate 阶段
6. **结果可比较** — 同 family 前后两次运行的结果可 diff

---

## 3. 架构设计

### 3.1 模块结构

```
foundation_dll/
├── principle/                         # 新目录
│   ├── __init__.py                    # aggregator + registry
│   ├── base.py                        # CheckResult, PrincipleCheck 基类
│   ├── checks/
│   │   ├── __init__.py                # 自动发现所有 check
│   │   ├── p1_lowering.py
│   │   ├── p1_no_redundant_branch.py
│   │   ├── p1_no_unnecessary_indirection.py
│   │   ├── p1_benchmark.py
│   │   ├── p2_template_hygiene.py
│   │   ├── p2_no_bare_new.py
│   │   ├── p2_layer_correctness.py    # 新增
│   │   ├── p2_no_code_duplication.py  # 新增（跨 family）
│   │   ├── p2_abstraction_balance.py  # 新增（跨 family）
│   │   ├── p3_patch_entry.py
│   │   ├── p3_patchdata.py
│   │   └── p3_no_hardcoded_state.py   # 新增
│   ├── context.py                     # 跨 family 上下文收集器
│   └── formatter.py                   # JSON/Markdown 输出
├── mechanism_audit.py                 # 改为调用 principle/
└── principle_auto_checks.py           # 废弃，迁移到 principle/
```

### 3.2 核心类型

```python
@dataclass
class CheckMeta:
    """Check 元数据，声明式注册。"""
    check_id: str                     # p1_lowering
    principle: str                    # P1 | P2 | P3
    severity: str = "blocking"        # blocking | monitoring
    scope: str = "family"             # family | cross_family
    description: str = ""
    dependencies: list[str] = field(default_factory=list)  # 依赖的外部产物


class PrincipleCheck(ABC):
    """所有 principle check 的基类。"""

    meta: ClassVar[CheckMeta]  # 子类覆盖

    def family_context(self, assembly: str, family_slug: str) -> FamilyContext:
        """提供当前 family 的上下文（C++ 文件路径、contract、contract_method_count）。"""
        ...

    @abstractmethod
    def run(self, ctx: FamilyContext) -> CheckResult:
        """执行检查，返回结果。"""
        ...


@dataclass
class CheckResult:
    check_id: str
    status: str                      # ALIGNED | CONCERN | VIOLATION | NOT_APPLICABLE | SKIP
    summary: str = ""
    details: list[dict] = field(default_factory=list)
    evidence: dict = field(default_factory=dict)
    per_method: list[dict] | None = None  # 可选：方法级结果
```

### 3.3 Aggregator

```python
def run_family_checks(assembly: str, family_slug: str,
                       skip_checks: list[str] | None = None,
                       cross_ctx: CrossFamilyContext | None = None,
                       ) -> PrincipleReport:
    """运行所有注册的 principle check。"""
    ...

def run_cross_family_checks(assembly: str,
                            cross_ctx: CrossFamilyContext,
                            ) -> list[CheckResult]:
    """运行跨 family 检查（p2_no_code_duplication, p3_no_hardcoded_state）。"""
    ...
```

#### overall 裁决算法

不再用硬编码的 VIOLATION > CONCERN > ALIGNED。改为加权评分：

```python
SCORE_MAP = {
    "ALIGNED": 1.0,
    "CONCERN": 0.5,
    "NOT_APPLICABLE": 0.0,   # 不参与评分
}
SEVERITY_WEIGHTS = {
    "blocking": 1.0,
    "monitoring": 0.0,       # 监控项不计入门禁分
}

def compute_overall(results: list[CheckResult]) -> str:
    """Compute overall from check results.

    blocking VIOLATION → VIOLATION
    无 blocking VIOLATION，但有 blocking CONCERN → CONCERN
    其余 → ALIGNED
    """
    violations = [r for r in results if r.status == "VIOLATION"]
    concerns = [r for r in results if r.status == "CONCERN"]

    blocking_violations = [r for r in violations
                           if get_check_meta(r.check_id).severity == "blocking"]
    blocking_concerns = [r for r in concerns
                         if get_check_meta(r.check_id).severity == "blocking"]

    if blocking_violations:
        return "VIOLATION"
    if blocking_concerns:
        return "CONCERN"
    return "ALIGNED"
```

---

## 4. 检查定义（8 项 + 4 项新增）

### P1 — 性能最优（blocking）

| check_id | scope | 当前状态 | 改进点 |
|---|---|---|---|
| p1_lowering | family | ✅ 有 | 改为 per-method 检测 |
| p1_no_redundant_branch | family | ✅ 有 | OK |
| p1_no_unnecessary_indirection | family | ✅ 有 | OK |
| p1_benchmark | family | ✅ 有 | 当 benchmark.json 不存在时 → CONCERN 而非 NOT_APPLICABLE |

### P2 — 方案完美性（blocking + monitoring）

| check_id | scope | 当前状态 | 改进点 |
|---|---|---|---|
| p2_template_hygiene | family | ✅ 有 | 加缓存，不重复扫描 |
| p2_no_bare_new | family | ✅ 有 | OK |
| p2_layer_correctness | family | ❌ 新增 | 检查变更文件的目录分布是否与架构层匹配 |
| p2_no_code_duplication | cross_family | ❌ 新增 | 跨 family fingerprint 相似度分析 |
| p2_abstraction_balance | cross_family | ❌ 新增 | 调用链深度与同类 family 偏差 |

#### p2_layer_correctness 实现方案

```python
LAYER_RULES = {
    "Emitter":       r"src/managed/.*CodeGen/Emission/",
    "Planner":       r"src/managed/.*CodeGen/Planning/",
    "RuntimeSupport": r"src/managed/.*CodeGen/RuntimeSupport/",
    "RuntimeCore":   r"src/native/runtime-core/",
    "Interpreter":   r"src/native/interpreter/",
}

def check_p2_layer_correctness(ctx: FamilyContext) -> CheckResult:
    """Verify that changes to this family touched expected layers.

    Reads git diff for the family's codegen files and checks that
    the modified file paths match the expected layer distribution.
    """
    # 读取本次变更的文件列表（或者验证目录中的 codegen 产出）
    changed_files = ctx.list_changed_files()
    if not changed_files:
        return CheckResult(check_id="p2_layer_correctness", status="NOT_APPLICABLE", ...)

    # 对每个变更文件，分配架构层
    violations = []
    for f in changed_files:
        layer = _assign_layer(f, LAYER_RULES)
        if layer is None:
            violations.append({"file": str(f), "reason": "unknown layer"})
        elif not ctx.is_expected_layer(layer):
            violations.append({"file": str(f), "layer": layer, "reason": "unexpected layer"})

    status = "ALIGNED" if not violations else "CONCERN"
    return CheckResult(check_id="p2_layer_correctness", status=status, ...)
```

#### p2_no_code_duplication 实现方案

```python
def check_p2_code_duplication(ctx: CrossFamilyContext) -> list[CheckResult]:
    """Cross-family code fingerprint similarity analysis.

    Computes normalized fingerprints (AST-normalized stmt sequences)
    for each method in each family, then clusters by similarity.
    Families with >80% fingerprint overlap on non-trivial methods
    are flagged for possible deduplication.
    """
    fingerprints = {}
    for family, cpp in ctx.all_family_codegen_files():
        for method in _extract_method_bodies(cpp):
            fp = _compute_fingerprint(method["body"])
            fingerprints.setdefault(family, {})[method["name"]] = fp

    # Cross-family pairwise comparison (only non-trivial methods)
    results = []
    for (fa, methods_a), (fb, methods_b) in itertools.combinations(fingerprints.items(), 2):
        overlap = _jaccard_similarity(set(methods_a.keys()), set(methods_b.keys()))
        if overlap > 0.8:
            results.append(CheckResult(
                check_id="p2_no_code_duplication",
                status="CONCERN",
                summary=f"{fa} ↔ {fb}: {overlap:.0%} method overlap",
            ))
    return results if results else [
        CheckResult(check_id="p2_no_code_duplication", status="ALIGNED", ...)
    ]
```

#### p3_no_hardcoded_state 实现方案

```python
HARDCODED_STATE_PATTERNS = [
    r'\bstatic\s+(int|bool|std::atomic)\b',             # primitive static state
    r'\bglobal_flag_\b',                                  # known flag pattern
    r'CHAOS_IL2CPP_STATIC_CONSTRUCTOR_HAS_RUN\b',        # static ctor guard
]

def check_p3_no_hardcoded_state(ctx: FamilyContext) -> CheckResult:
    """Scan generated C++ for hardcoded static state variables.

    Generated code should not contain persistent static state that
    can't be reverted during hotupdate.
    """
    cpp = ctx.generated_cpp_content()
    violations = []
    for pattern in HARDCODED_STATE_PATTERNS:
        matches = re.findall(pattern, cpp)
        if matches:
            violations.append({"pattern": pattern, "count": len(matches)})

    status = "ALIGNED" if not violations else "CONCERN" if len(violations) <= 3 else "VIOLATION"
    return CheckResult(check_id="p3_no_hardcoded_state", status=status, ...)
```

---

## 5. 自动发现注册

```python
# principle/checks/__init__.py
import pkgutil, importlib
from principle.base import PrincipleCheck

_registry: dict[str, type[PrincipleCheck]] = {}

def discover_checks():
    """Auto-discover all PrincipleCheck subclasses in checks/ directory."""
    for importer, modname, _ in pkgutil.iter_modules(__path__):
        module = importlib.import_module(f"principle.checks.{modname}")
        for attr in dir(module):
            cls = getattr(module, attr)
            if isinstance(cls, type) and issubclass(cls, PrincipleCheck) and cls is not PrincipleCheck:
                _registry[cls.meta.check_id] = cls
```

每个 check 文件自注册：

```python
# principle/checks/p1_lowering.py
class P1LoweringCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p1_lowering",
        principle="P1",
        severity="blocking",
        scope="family",
        description="Generated code uses chaos_eval_stack, not SimpleForward",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        cpp = ctx.generated_cpp_content()
        per_method = []
        for method in ctx.contract_methods():
            has = self._method_has_lowering(cpp, method)
            per_method.append({"name": method, "has_lowering": has})

        all_have = all(m["has_lowering"] for m in per_method)
        return CheckResult(
            check_id=self.meta.check_id,
            status="ALIGNED" if all_have else "VIOLATION",
            summary=f"{sum(1 for m in per_method if m['has_lowering'])}/{len(per_method)} methods with lowering",
            per_method=per_method,
        )
```

---

## 6. Cross-Family 上下文收集

```python
@dataclass
class CrossFamilyContext:
    """跨 family 上下文，供 p2_no_code_duplication、p2_abstraction_balance 使用。"""
    assembly: str
    family_contexts: dict[str, FamilyContext]

    @classmethod
    def build(cls, assembly: str, family_slugs: list[str] | None = None) -> CrossFamilyContext:
        """Build context for all families in an assembly (or specified subset)."""
        ...

    @classmethod
    def from_cache(cls, cache_path: Path) -> CrossFamilyContext:
        """从 JSON 缓存恢复上下文，避免每次重新编译。"""
        ...
```

---

## 7. 输出格式

```json
{
  "schemaVersion": 2,
  "assembly": "System.Private.CoreLib",
  "family": "convert-char",
  "generatedAt": "2026-05-19T10:00:00",
  "overall": "ALIGNED",
  "checks": {
    "p1_lowering": {
      "status": "ALIGNED",
      "summary": "17/18 methods with chaos_eval_stack",
      "evidence": { ... },
      "perMethod": [
        { "method": "ToChar(System.Int32)", "has_lowering": true }
      ]
    },
    "p2_no_code_duplication": {
      "status": "CONCERN",
      "summary": "convert-char ↔ primitive-numeric-conversions: 65% method overlap"
    }
  },
  "crossFamily": {
    "p2_no_code_duplication": [
      { "familyA": "convert-char", "familyB": "primitive-numeric-conversions", "overlap": 0.65 }
    ]
  }
}
```

---

## 8. 迁移路径

| 步骤 | 内容 | 预计工时 |
|---|---|---|
| 1 | 创建 `principle/` 目录 + `base.py` + `__init__.py` + 自动发现 | 小 |
| 2 | 逐个迁移现有 6 项 check 到插件化格式 | 中 |
| 3 | 实现 `p2_layer_correctness`（新增） | 中 |
| 4 | 实现 `p3_no_hardcoded_state`（新增） | 小 |
| 5 | 实现 `CrossFamilyContext` + `p2_no_code_duplication`（新增） | 大 |
| 6 | 实现 `p2_abstraction_balance`（新增） | 大 |
| 7 | mechanism_audit.py 切换为调用 principle/ 输出 | 小 |
| 8 | 废弃旧的 `principle_auto_checks.py` | 小 |
| 9 | 更新 SKILL.md 与新架构同步 | 小 |

---

## 9. 否决项（不在本次设计中覆盖）

1. **Real-time diff monitoring** — 本次不做自动 diff/regression 检测，只做单次快照评分
2. **Dashboard 集成** — p2_layer_correctness 等的趋势监控由外部 dashboard 做，principle 模块只输出结构化数据
3. **AI-assisted check** — 不引入 LLM 来做 principle 判断，所有检查保持 deterministic
