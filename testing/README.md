# Chaos IL2CPP Test Framework

## 测试分层

| 层级 | 类型 | 框架 | 位置 |
|------|------|------|------|
| L1 | IL 指令解析与验证 | xUnit | `src/managed/Chaos.IL2CPP.Tests.IL.Parsing/` |
| L2 | CodeGen 翻译正确性 | xUnit | `src/managed/Chaos.IL2CPP.Tests.CodeGen.Translation/` |
| L3 | Scriban 模板渲染 | xUnit | `src/managed/Chaos.IL2CPP.Tests.Templates/` |
| L4 | Native runtime 单元测试 | Google Test | `src/native/` |
| L5 | Foundation DLL 验证 | pytest | 见 foundation-dll 管线 |

## 运行测试

```bash
# 全部 C# 测试
dotnet test src/managed/

# 指定 project
dotnet test src/managed/Chaos.IL2CPP.Tests.IL.Parsing/

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
