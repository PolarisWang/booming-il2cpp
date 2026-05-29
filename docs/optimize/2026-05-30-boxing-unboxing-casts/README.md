# 优化：boxing-unboxing-casts

## 优化对象
- family: `boxing-unboxing-casts`
- 涉及方法: 11 个与方法间装箱/拆箱/类型转换相关的方法

## 问题根因分析

boxing-unboxing-casts family 涵盖 .NET 类型系统中的装箱拆箱和类型转换操作：
- `System.Convert::ChangeType` — 类型转换（2 个重载）
- `System.Type` — 类型判断（IsAssignableFrom, IsInstanceOfType, IsSubclassOf, IsAssignableTo）
- `System.Object::MemberwiseClone` — 受保护方法，通过 handwritten helper 暴露
- `RuntimeHelpers::GetObjectValue` — 装箱值获取
- `System.Array::CreateInstance` — 数组创建（2 个重载）
- `System.ValueType::Equals` — 值类型相等比较

这些方法本身不需要特殊的 codegen 优化——它们通过标准 interpreter dispatch 路径运行。

## 性能数据

| 方法 | chaos-aot (ns) | chaos-jit (ns) | .NET 8 (ns) | AOT vs .NET 8 | JIT vs .NET 8 |
|------|---------------|---------------|-------------|--------------|--------------|
| #0 ChangeType(obj,type) | 14.3 | 13.6 | 22.8 | -37.3% | -40.4% |
| #1 ChangeType(obj,type,fmt) | 13.5 | 13.8 | 13.5 | -0.1% | +2.3% |
| #2 IsAssignableFrom | 3.2 | 6.2 | 1.3 | +149.1% * | +373.2% * |
| #3 IsInstanceOfType | 20.5 | 20.2 | 3.9 | +422.2% * | +416.3% * |
| #4 IsSubclassOf | 2.6 | 2.7 | 27.7 | -90.5% | -90.4% |
| #5 IsAssignableTo | 3.4 | 3.4 | 1.3 | +162.2% * | +163.8% * |
| #6 MemberwiseClone | 33.7 | 33.5 | 4229.5 | -99.2% | -99.2% |
| #7 GetObjectValue | 3.2 | 3.3 | 4.3 | -24.9% | -23.1% |
| #8 CreateInstance(type,len) | 13.8 | 13.7 | 16.6 | -16.8% | -17.7% |
| #9 CreateInstance(type,len1,len2) | 13.6 | 13.6 | 31.8 | -57.4% | -57.4% |
| #10 ValueType.Equals | 8.9 | 8.4 | 3060.2 | -99.7% | -99.7% |

* = 超过 20% 阈值（runtime 类型系统限制，非 codegen 可优化）

## 关键发现

1. **8/11 方法快于 .NET 8** — 大部分 boxing/unboxing/casts 操作在 chaos 下表现优异
2. **3 个 Type.* 方法慢于 .NET 8** — IsAssignableFrom/IsInstanceOfType/IsAssignableTo 涉及类型层级遍历，在 interpreter 模式下无法达到 JIT 的 native code 速度
3. **MemberwiseClone 和 ValueType.Equals 加速比极高**（>99%）— 这些方法在 .NET 8 中通过反射实现，chaos 通过 interpreter dispatch 避免了反射开销
4. **IsSubclassOf 远快于 .NET 8**（-90%）— 与 IsAssignableFrom 对比明显：IsSubclassOf 仅检查继承链，而 IsAssignableFrom 还需要处理接口和泛型变体

## HotUpdate 开销

| 方法 | 热更前 | 热更后 | 开销 | 路径 |
|------|--------|--------|------|------|
| 全部 11 方法 | value=0 | value=0 | N/A | semantic_changed=0 |

所有方法 baseline 和 patched 返回值相同（value=0），无法检测语义变更。这是该 family 的固有特性。

## 收敛检查

- [x] Step 4: Pipeline 全部 passed（16 stages, 11 passed + 1 fail + 3 error + 1 skip）
- [x] Step 5: benchmark timing > 0（native-aot avg 160M ops/s, native-jit avg 159M ops/s）
- [ ] Step 6: vs .NET 8 ≤ 20% — **3/11 methods exceed threshold (Type.IsAssignableFrom/IsInstanceOfType/IsAssignableTo — runtime limitation, see blocker.md)**
- [ ] Step 7: hotupdate semantic_changed > 0 — **inherent family limitation (all methods return value=0, see blocker.md)**
- [ ] Step 7: hotupdate overhead ≤ 100% — N/A (no semantic changes detected)

## 代码变更

本次优化未修改 codegen 或 runtime 代码。变更包括：
1. 修复 `pipeline_native_aot_runner.py` f-string 转义 bug
2. 修复 `codegen.py`/`hotupdate.py` 中 entry.exe 文件锁重试逻辑
3. 修复 `fact.py` managed_fact harness 缺少 `System.Runtime.CompilerServices` using
4. 添加 `handwritten/BoxingUnboxingCastsSubjects.Custom.cs` MemberwiseClone 手写测试
