# Chaos IL2CPP Test Framework

## 测试分层

| 层级 | 类型 | 框架 | 位置 |
|------|------|------|------|
| L1 | IL 指令解析与验证 | xUnit | `src/managed/Chaos.IL2CPP.Tests.IL.Parsing/` |
| L2 | CodeGen 翻译正确性 | xUnit | `src/managed/Chaos.IL2CPP.Tests.CodeGen.Translation/` |
| L3 | Scriban 模板渲染 | xUnit | `src/managed/Chaos.IL2CPP.Tests.Templates/` |
| L4 | Native runtime 单元测试 | Google Test | `src/native/` |
| L5 | Foundation DLL Family 功能测试 | xUnit | `foundation-dll/<assembly>/<family>/test/` |
| L6 | Foundation DLL 验证管线 | Python | `foundation-dll/_core/python/` |
| L7 | 跨 Family 聚合分析 | Python | `foundation-dll/_core/python/aggregate.py` |

## Foundation DLL 验证框架（新）

Foundation DLL 验证框架按 capability family 组织，每个 family 是一个独立验证单元。

### 目录结构

```
foundation-dll/
├── _contracts/                 # 合约注册表
│   └── ledger.json             # 全量 family 清单
├── _core/python/               # 13-stage 验证管线核心
│   ├── models.py               # 数据模型
│   ├── orchestrator.py         # 管线编排
│   ├── preflight.py            # Stage 0: 合约校验
│   ├── codegen_runner.py       # Stage 1-2: Codegen
│   ├── fact_verifier.py        # Stage 3-4: Fact 验证
│   ├── audit_runner.py         # Stage 5: 审计
│   └── ...                      # 其他 stage runner
├── System.Private.CoreLib/     # 按 assembly 分组
│   ├── convert-char/           # 具体 family
│   │   ├── contract.json       # 能力族合约
│   │   ├── test/               # L5 xUnit 测试
│   │   ├── managed/            # C# subject 源码
│   │   └── native/             # native 构建输出
│   ├── _assembly.json          # assembly 级元数据
│   └── ...
└── scripts/
    └── verify-family.sh        # 验证单个 family
```

### 运行验证

```bash
# 验证单个 family（标准模式）
bash scripts/run-foundation-dll-family.sh convert-char

# 验证单个 family（严格模式，含 hotupdate）
bash scripts/run-foundation-dll-family.sh convert-char --strict

# 验证所有已导入的 family
bash scripts/run-foundation-dll-all.sh

# 跳过某些阶段
bash scripts/run-foundation-dll-family.sh convert-char --skip benchmark hotupdate
```

### 导入新 Family

1. 在 `foundation-dll/<assembly>/<family-slug>/` 创建目录
2. 创建 `contract.json`（参考 convert-char 的合约格式）
3. 添加 managed/subjects/（C# subject 源码）
4. 创建 test/*Tests.cs（L5 xUnit 测试）
5. 更新 `_contracts/ledger.json`，设置 `migrationStatus: "imported"`
6. 运行验证确认通过

## 运行测试

```bash
# 全部 C# 测试
dotnet test src/managed/

# 指定 project
dotnet test src/managed/Chaos.IL2CPP.Tests.IL.Parsing/

# Foundation DLL L5 测试
dotnet test foundation-dll/System.Private.CoreLib/convert-char/test/

# Native 测试
cmake -S src/native -B build/native -G "Visual Studio 17 2022" -A x64
cmake --build build/native --config RelWithDebInfo
ctest --test-dir build/native --output-on-failure --config RelWithDebInfo

# 全量运行
bash scripts/run-all.sh
```

## 添加测试

- **IL Parsing**: 在 `data/il/` 下添加 JSON 指令描述，在相应 test class 中添加 Fact
- **CodeGen Translation**: 在 `data/cpp-expected/` 下添加预期输出，验证 codegen 输出匹配
- **Native**: 在 `src/native/` 相应子目录下添加 `*_test.cpp`，使用 `add_chaos_test()` 注册
