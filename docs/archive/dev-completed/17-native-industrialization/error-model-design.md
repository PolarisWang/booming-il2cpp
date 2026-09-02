# Native 模块错误处理模式统一方案 — Design

> 任务：T-B3 错误类型统一方案（native-industrialization batch-1）
> 日期：2026-09-02
> 代码域：`src/native/`（约 123,000 行 C++ / 576 文件）
> 状态：建议稿（待 review）
> 所属阶段：T-B 架构设计 → P1 编码规范审计落地

---

## 1. 背景与审计范围

native 模块当前错误处理风格高度混合，同一编译单元内可能存在
宏断言（`CHAOS_IL2CPP_FAIL`）、异常（`throw` / `CHAOS_EH_*`）、平台错误码
（`GetLastError` / `errno`）、`nullptr` 投机、`bool` / `int` 返回值等多种模式。
这不仅增加 review 心智负担，也导致错误语义在跨层调用（AOT → Codegen → 运行时 →
Python 测试）时难以稳定表达。

本设计基于对本仓库的真实代码审计得出，审计方法为全文 Grep/Read，
所有数字为实际命中统计（`src/native` 下）。审计不修改任何源码。

---

## 2. 当前错误处理模式审计

### 2.1 关键定义（先于调用统计说明）

| 宏/类型 | 定义位置 | 语义 |
|---------|---------|------|
| `CHAOS_IL2CPP_ABORT()` | `native_types.h:103` | `std::abort()` 封装；真实崩溃路径 |
| `CHAOS_IL2CPP_FAIL(...)` | `native_types.h:114-120` | 非致命失败指示；优先走 `g_chaos_fail_hook`（验证模式可注入 throw），否则 abort |
| `CHAOS_IL2CPP_FAIL_FAST()` | `native_types.h:128-134` | 边界检查用直接 trap（`__fastfail`/`__builtin_trap`），无间接跳转，可让编译器提升循环不变量 |
| `CHAOS_IL2CPP_FAILED(hr)` | `native_types.h:137` | HRESULT 风格成功判定，`((hr) < 0)` |
| `CHAOS_IL2CPP_SUCCEEDED(hr)` | `native_types.h:138` | `((hr) >= 0)` |
| `enum class Status` | `status.h:10-21` | 统一状态码枚举 `kOk..kRuntimeCallFailed`，含 `is_ok/is_failure` 帮助函数；注释声明覆盖 RuntimeAbi 与 CodegenBridge 两段区间 |
| `struct ParseResult` | `json_reader.h:73-76` | 值 + `error` 指针的 Result 式结构（error==nullptr 即成功） |
| `CHAOS_EH_TRY/CATCH/THROW` | `eh.h` | 三态异常抽象：C++ try/catch，Win32 SEH，SETJMP/longjmp |

**关键发现 A**：`CHAOS_IL2CPP_FAILED` / `CHAOS_IL2CPP_SUCCEEDED`（HRESULT 风格宏）
与 `enum class Status`（及 `is_ok/is_failure`）**在代码库中只有定义、没有任何业务调用点**——
它们是早就预留但从未大规模启用的“统一错误类型”雏形。这是本方案 A 的最大既有资产。

### 2.2 使用频率统计

| 风格 | 命中计数 | 涉及文件 | 说明 |
|------|---------|:-------:|------|
| `CHAOS_IL2CPP_FAIL`（调用点） | ~9 文件内 | 9 | 集中在数据结构/热补丁/受控转换 |
| `CHAOS_IL2CPP_FAIL_FAST` | 5 | 2 | 边界检查（`array_stubs` 等） |
| `CHAOS_IL2CPP_FAILED/SUCCEEDED`（使用者） | **0** | 0 | 仅定义，未启用 |
| `enum class Status` / `is_ok`/`is_failure`（使用者） | **0** | 0 | 仅定义，未启用 |
| `class/struct*Result` 自定义类型 | 29 | 16 | 多为局部 `ParseResult`/`QueryResult`，非统一 `Result<T,E>` |
| `throw` | 259 | 81 | 含 `CHAOS_EH_THROW`、`eh_benchmark` 实验 |
| `try` / `catch` | 135 / 104 | ~47 | 含 SEH Handler、fast_dispatch、interpreter_vm |
| `CHAOS_EH_*` 宏体系 | 25 | 4 | 统一 EH 抽象（C++/SEH/setjmp 三态） |
| `assert` / `chaos_assert` | 11 / 0 | 8 / 0 | assert 极少；chaos_assert 无 |
| `abort()`（含宏） | 90 | 54 | 真实崩溃路径，多经 `CHAOS_IL2CPP_ABORT` 或 `g_chaos_fail_hook` |
| `exit(` | 10 | 3 | 仅 reference/host main 与工具 main |
| `terminate()` | 0（native） | 0 | 命中均为 MSVC 头文件兼容注释 |
| `GetLastError`/`LASTERROR`/`errno` | 60 | 27 | 集中在 PAL；`pal_error.h` 已抽象 `PalSetLastError/PalGetLastError` |
| `return nullptr`（错误即空指针） | 38（精确匹配） | 15 | 全库普遍采用（如 `com_factory`、`mutex_stubs`） |
| `==0`/`>0`/`return 0`/`return -1` int 错误码 | 5314（含所有零值分支） | 356 | 大量为 codegen 后端/寄存器分配；需细分真错误码 |
| `bool` 返回值 | 广泛 | 数百 | **成功方向无统一约定**（true=成功 与 false=成功 并存） |
| `HRESULT`（`S_OK`/`E_FAIL`/`E_INVALIDARG`...） | 127 | 25 | 集中在 COM（`com_ccw`/`com_typelib`） |

> 说明：`==0`/`!=0` 一行同时包含算术比较、空指针判空与错误码判断，无法有效
> 区分；此处仅作“int 返回错误码风格普遍存在”的数量级参考，精确语义需逐点抽取。

### 2.3 风格分布小结

按可归类调用点估计（宏调用、throw、Result 结构、getlasterror、HRESULT、abort），
当前大致为：

- **断言/中止（FAIL/FAIL_FAST/abort）**：约 20%，用于“不该发生”的内部不变量破坏。
- **C++ 异常（throw/try/catch + CHAOS_EH）**：约 25–30%，用于托管异常传播、SEH 边界、
  验证模式失败注入；被三层 EH 抽象隔离。
- **平台错误码（GetLastError/errno）**：约 5%，集中在 PAL，已用 `PalGetLastError` 半抽象。
- **nullptr / bool / int（返回值式）**：约 40–45%，**最无约定、最分散**，
  是本次统一的重点目标。
- **HRESULT / Status / Result<T,E>（相对统一）**：目前占比极低且仅局部（COM、
  json_reader），尚未形成体系。

除已预留但未启用的 `Status` / `FAILED/SUCCEEDED` 外，**没有单一主导风格**，返回式
错误处理（nullptr/bool/int）为最大存量。

---

## 3. 候选方案对照

| 维度 | A：推广 CHAOS_IL2CPP_FAILED/SUCCEEDED（HRESULT 风格） | B：引入 `Result<T, Error>` 模板（Rust 风格） | C：保持混合，文档化各约定（Heather 风格） | D：新代码统一 + 存量渐进迁移 |
|------|------|------|------|------|
| **统一收益** | 高：单一 `int32 状态码`，判别宏已存在 | 高：编译期强制显式处理错误 | 低：仅文档，无法机械强制 | 中高：可持续收敛 |
| **性能影响** | 极低：一个整数寄存器 + 比较分支，零拷贝零分配 | **中等偏高：值+错误可能引入宽返回、移动/拷贝、`std::optional` 式包装开销** | 零 | 同所选基底（若沿用纯 int 则低） |
| **与 codegen 输出兼容** | **高**：`FAILED/SUCCEEDED` 本来就是为 native-aot codegen 预留的；生成的 C++ 可直接用宏判整型。溢出检查大量输出 `==0` 寄存器分支，能自然对齐 | **低**：codegen 生成的是扁平 C++ 函数，插入模板 `Result` 返回类型需改生成器全部函数签名，牵动 Scriban 模板与 AOT-IR→C++ 映射 | 高：不动 codegen | 中：新增 codegen 输出可用，存量不动 |
| **与四层红线一致性（AOT→Codegen→TPG→Python）** | 高：整型状态码在 AOT IR、生成 C++、运行时、Python 校验四处都易序列化/打印 | 中：`Result<T,E>` 只在 C++ 内部有意义，跨 Python/JSON 边界需序列化，反而多一层映射 | 低：各层各说，无法形成统一契约 | 中高：可先固定契约再逐层推广 |
| **学习曲线** | 低：C/C++ 工程师对 HRESULT/整型错误码天然熟悉；宏已在 | 中高：需引入 optional/变体、破坏式传播习惯；团队需新训练 | 零 | 低（分阶段） |
| **错误信息密度** | 中：状态码 → 查表/字符串 | 高：可携带对象与消息 | 中 | 视基底 |
| **误用/遗漏风险** | 中：整型码易被忽略返回值（靠 `[[nodiscard]]` 缓解） | **最低：编译期要求处理** | 高：文档靠自觉 | 中 |
| **改造工作量** | 中：存量 bool/int/nullptr 收敛为状态码；涉及面广但语义平移直接 | 高：全量重写核心层返回类型；codegen 联动大 | 极小 | 中 |
| **对现有 `g_chaos_fail_hook` 验证注入的兼容** | 高：验证模式与 FAIL 共存，不冲突 | 高（也可行） | 高 | 高 |

---

## 4. 推荐方案

### 4.1 推荐模型

**基底选择：方案 A（HRESULT 风格整型状态码）**
**落地节奏：继承方案 D（新代码强制统一 + 存量渐进迁移），由 P1 阶段执行。**

推荐落地形态：

1. 以已预留但未启用的 `enum class Status`（`status.h`）作为唯一权威状态码枚举，
   覆盖 0=成功、1..9=失败 的现有取值，并随需求扩充（保持 `0=成功/负=致命` 语义）。
2. 让 `CHAOS_IL2CPP_SUCCEEDED(status)` / `CHAOS_IL2CPP_FAILED(status)`（`native_types.h`
   第 137-138 行）真正启用并作为唯一判别宏，替代散落的 `==0` / `!=0` 判定。
3. 迁移最大的最混乱区间——nullptr/bool/int 返回式错误——统一为返回 `Status`；
   返回非状态数据但可能失败的函数，改为“`Status` 出参 + 数据出参”或
   `Status ReturnOr...` 组合，而非引入重量级 `Result<T,E>` 模板。
4. `PalGetLastError/PalSetLastError` 继续做平台错误码落袋层；`json_reader::ParseResult`
   类局部 `Result` 收敛为 `Status` + 字符串出参。

```cpp
// 目标形态示例（语义主导，供 codegen/运行时端一致使用）
using chaos::il2cpp::common::Status;

Status DoInit();
Status ParseConfig(/* out */ Config* cfg);

// 调用处：
if (CHAOS_IL2CPP_FAILED(Status initSt = DoInit())) {
    CHAOS_IL2CPP_LOG_ERR_PAL;   // 状态码落袋
    return initSt;
}
```

### 4.2 为何选择 A 而非 B（Result<T,E>）

- **性能（P1 最高优先级）**：`Result<T,E>` 在核心热路径引入宽返回类型与可选/变体包装，
  值拷贝与移动成本高于单个 `int32` 状态码。对照 production il2cpp 的扁平 codegen 输出，
  整型状态码几乎零开销，是硬性红线不破坏性能的关键。
- **codegen 输出兼容**：`FAILED/SUCCEEDED` 宏本就是为 native-aot codegen 预留的判别原语，
  生成 C++ 用整型判 `==0/!=0` 的既有寄存器分支能自然对齐；若改为模板返回类型，需改写
  Scriban 生成器全部函数签名，超出 T-B3 责任边界且项目「不改 codegen 输出格式」是非目标
  （design-v1 §7）。
- **跨层一致性**：整型状态码在 AOT IR、生成 C++、运行时、Python 校验四处都易序列化、打印、
  比对；`Result<T,E>` 仅在 C++ 内部有形状，跨 Python/JSON 边界反而需额外映射层。
- **团队与治理**：整型错误码是 C/C++ 工程师直觉范式，认知成本低；且 `Status` 枚举与判别
  宏已就位（零成本复用），无需引入新依赖/新类型系统。

### 4.3 为何选择 A 而非 C（保持混合，仅文档）

- C 无法机械强制约定，最大存量区间（bool/int/nullptr）得不到收敛，跨子模块的
  成功方向歧义（true=成功 vs false=成功）会持续制造缺陷与重复防御代码。
- 前文统计显示 codegen 后端大量 `==0` 寄存器分支即属此类，是历史缺陷重要来源，
  仅文档无法根治。

---

## 5. 统一模型定义

### 5.1 权威类型 `enum class Status`

以 `status.h` 为唯一事实源，扩展语义分组：

| 值 | 名称 | 语义 |
|:--:|------|------|
| 0 | `kOk` | 成功（与 `SUCCEEDED` 判别一致） |
| 1 | `kInvalidArgument` | 参数非法 |
| 2 | `kInvalidState` | 对象/子系统状态非法 |
| 3 | `kNotFound` | 未找到资源/元数据 |
| 4 | `kNotSupported` | 平台/能力不支持 |
| 5 | `kManagedException` | 托管异常（绑定 `CHAOS_EH` 传播） |
| 6 | `kInternalError` | 内部不变量破坏 |
| 7 | `kMetadataResolutionFailed` | 元数据解析失败 |
| 8 | `kClassInitFailed` | 类初始化失败 |
| 9 | `kRuntimeCallFailed` | 运行时调用失败 |

> 约定：**0=成功、正=可恢复失败、负=致命（走 fail_hook/abort 前落袋）**，与
> `FAILED(hr)=(hr)<0` 的判别宏语义完全兼容。

### 5.2 统一七条规则

1. **返回“是否成功”的函数**一律返回 `Status`，用 `SUCCEEDED/FAILED` 判别；禁裸 `bool`。
2. **需要“成功 + 数据”**：`Status` 头部 + 数据出参（指针/引用）；禁 `nullptr 即失败`。
3. **平台错误详情**一律经 `PalSetLastError/PalGetLastError` 落袋，供 `MapStatusToHr` 转换。
4. **致命不可恢复**经 `CHAOS_IL2CPP_FAIL/FAIL_FAST` 统一路径（先落袋状态码）。
5. **托管异常传播**走 `CHAOS_EH_*` + 返回 `kManagedException`；不改三态 EH 抽象。
6. **禁止再新增**局部 `*Result` 自定义错误结构；存量 29 处收敛为 `Status`+出参。
7. **状态码 → 字符串**提供单一 `StatusToString`，供打印与 Python 校验回溯。

### 5.3 映射层（兼容告警）

- `HRESULT → Status`：`MapHrToStatus`（COM 边界，127 处局部 HRESULT）。
- `errno → Status`：经 `PalGetLastError` 转换。
- `Status → HRESULT`：返回给 COM 时映射回。

---

## 6. 迁移路径（红线条目）

对接 dispatch：`T-B3` 产出本模型 → `batch-2 / P1`“错误类型落地”。

| 步骤 | 动作 | 产出 | 门禁 |
|:----:|------|------|------|
| 1 | 批准本模型，将 `Status`/判别宏启用为唯一权威；新增函数一律走 `Status` | 头文件契约冻结 | T-B 评审 merge_gate |
| 2 | 在现有 head 实现 `MapHrToStatus`/`StatusToString`/映射层 | 映射工具就位 | clang-tidy 0 P0 |
| 3 | 按热点收敛存量：先 PAL→ 平台层，再 codegen 出参，最后运行时 | 逐模块 PR | 每 PR 单测+review |
| 4 | 强制存量新改动用 `[[nodiscard]]`，驱动忽略返回值的调用点显式处理 | 编译告警级约束 | CI + review |

> **红线**：全迁移期间**不得改动 codegen 输出格式与 AOT→Codegen→TPG→Python 四层契约
> 边界**；只改 C++ 侧内部错误返回形状，Python 校验通过既有 StatusCode 序列化读取。

---

## 7. 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|------|:----:|:----:|------|
| P1 性能受模板/宽返回影响 | 低（采用整型 A） | 高 | 已否决 B；整型状态码零拷贝 |
| codegen 输出被回归破坏 | 低 | 高 | 红线：不改生成格式，只判整型 |
| 存量 bool/int/nullptr 收敛面广、周期长 | 高 | 中 | 分模块渐进（方式 D），非一次性 |
| 忽略返回值的新增遗漏 | 中 | 中 | `[[nodiscard]]` + clang-tidy 强制 |
| COM/errno 映射语义失真 | 中 | 中 | 独立映射层 + 单测覆盖转换矩阵 |
| 团队学习/惰性退回老风格 | 中 | 中 | P1 审计 + review 硬门禁 |

**综合评价**：以**方案 A 基底 + 方案 D 节奏**，在**性能（P1）优先、不破坏 codegen 与四层契约**
的约束下给出错误类型统一路线。交付本模型文档后，实际落地交由 P1“错误类型落地”执行。

---

## 8. 附：审计命令可回溯清单（针对 src/native/）

```bash
# 宏风格：FAIL/FAIL_FAST/FAILED/SUCCEEDED/ABORT
grep -rnE "CHAOS_IL2CPP_(FAIL|FAIL_FAST|FAILED|SUCCEEDED|ABORT)" src/native --include=*.h --include=*.cpp
# Status / is_ok / is_failure
grep -rnE "\b(chao)?(is_ok|is_failure)\b" src/native
# Result 自定义结构
grep -rnE "struct [a-zA-Z_]+Result|class [a-zA-Z_]+Result|using .*= Result<" src/native
# 异常三态
grep -rnE "\b(throw|try|catch)\b|CHAOS_EH_(TRY|CATCH|THROW|RETHROW)" src/native
# 断言/中止
grep -rnE "(assert|chaos_assert|abort\(|std::abort|exit\(|std::exit|terminate)" src/native
# 平台错误码
grep -rnE "(GetLastError|SetLastError|LASTERROR|errno|Pal(Get|Set)LastError)" src/native
# HRESULT
grep -rnE "\b(S_OK|S_FALSE|E_\w+)|HRESULT\b" src/native
# 返回式（数量级参考，需人工细分）
grep -rnE "return (0|-1|nullptr|true|false)[ ;]" src/native | wc -l
```

（以上命令以只读方式运行，未修改任何源文件。）

---

*文档完 —— 设计随 T-B 评审与用户确认后冻结，P1 阶段落地。*
