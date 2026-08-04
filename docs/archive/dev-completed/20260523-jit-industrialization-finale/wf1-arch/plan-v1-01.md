# wf1-arch: IEncoder/ISehHandler 接口抽象 — 实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 执行。步骤使用复选框（`- [ ]`）语法跟踪。完成时先归档当前子任务，再同步父 roadmap。

**目标：** 将 jit_engine.cpp 中硬编码的 x64 编码器内联函数调用和 SEH 平台相关代码抽象为接口，使 ARM64 编码器和 Linux SEH 可独立实现而不改 jit_engine.cpp。

**架构：** 纯虚接口 + 现有实现包装。NativeCodeGenerator 通过 IEncoder& 发射指令，通过 ISehHandler& 管理 SEH 注册。

**架构审核模式：** critical

**结构告警重点：** 文件职责拆分（x64_encoder.h 928 行保持完整，IEncoder 只抽象使用到的函数签名；jit_seh.cpp 按平台分离）

**设计文档：** 父 roadmap brainstrom 拍板

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交"固定链路。

---

## 待创建/修改文件

### 新建文件

| 文件 | 职责 | 
|------|------|
| `src/native/jit/IEncoder.h` | 编码器纯虚接口 + x64 寄存器常量（跨平台共享） |
| `src/native/jit/ISehHandler.h` | SEH handler 纯虚接口 + 异常代码常量 |

### 修改文件

| 文件 | 修改内容 |
|------|----------|
| `src/native/jit/jit_engine.cpp` | NativeCodeGenerator 通过 IEncoder& 和 ISehHandler& 调用 |
| `src/native/jit/jit_engine.h` | Compile 签名不变；内部实现改为接收 IEncoder |
| `src/native/jit/jit_seh.h` | 保留 extern "C" 入口函数（ChaosT4RaiseException 等），注册/查找函数改为接口实现 |
| `src/native/jit/jit_seh.cpp` | 抽出 WinSehHandler 类实现，保留外部 C 链接函数 |
| `src/native/jit/x64_encoder.h` | 保持不变（作为 X64Encoder 的实现细节） |
| `src/native/jit/CMakeLists.txt` | 添加新文件 |

---

## 步骤

### Step 1: 创建 IEncoder.h

新建接口文件，定义 NativeCodeGenerator 使用的编码器抽象：

```cpp
namespace chaos::il2cpp::jit {

// x64/ARM64 共享的寄存器常量
// （x64 用 Intel SDM 编号，ARM64 用自身的寄存器编号）
// 架构特定的编码实现使用各自的寄存器编号

class IEncoder {
public:
    virtual ~IEncoder() = default;

    // 数据移动
    virtual void EmitMovRR(uint8_t dst, uint8_t src) = 0;
    virtual void EmitMovRM(uint8_t dst, uint8_t base, int32_t disp) = 0;
    virtual void EmitMovMR(uint8_t base, int32_t disp, uint8_t src) = 0;
    virtual void EmitMovImm64(uint8_t dst, uint64_t imm) = 0;
    virtual void EmitMovRI32(uint8_t dst, int32_t imm) = 0;
    virtual void EmitMovRIImm32(uint8_t dst, uint32_t imm) = 0;
    // ... 约 60 个虚函数
};

}  // namespace
```

**接口方法清单**（从 jit_engine.cpp 调用的 Emit* 函数提取）：

MovRR, MovRM, MovMR, MovImm64, MovRI32, MovRIImm32, MovMI32, LeaRM, LeaRipRel,
AddRR, AddRI, AddMR, Add32RR,
SubRI, SubRR, Sub32RR,
ImulRR, Imul32RR,
AndRR, And32RR,
OrRR, Or32RR,
XorRR, XorRR, Xor32RR, Xor32ZR, XorMR, XorRM, XorZR, XorpsRR,
Neg32, Neg, Not32, Not,
ShlRI, ShrRI, SarRI, ShlRCL, ShrRCL, SarRCL,
CmpRR, CmpRI, CmpMI,
TestRR, Setcc, Cmovcc,
JmpRel32, JmpRel8, JccRel32, JccRel8, JmpReg,
CallRel32, CallReg,
Ret, Push, Pop,
Movsxd,
MovSDRR, MovSSRR, MovSDMR, MovSDRM, MovUPSMR, MovUPRM, MovSSMR, MovSSRM,
AddSDRR, SubSDRR, MulSDRR, DivSDRR,
AddSSRR, SubSSRR, MulSSRR, DivSSRR,
Cvtsi2sd, Cvtsi2ss, Cvttsd2si, Cvttss2si, Cvtsd2ss, Cvtss2sd,
Ucomisd, Ucomiss, Comisd,
MovdXrm, MovdMrx, MovqXrm, MovqMrx, PxorRR,
REX, ModRM, EmitREX, EmitREXB, EmitModRM (这些是辅助函数)

### Step 2: 创建 X64Encoder 类

X64Encoder 实现 IEncoder，内部直接调用 x64_encoder.h 的现有内联函数。

关键设计：
- `X64Encoder(CodeBuffer& buf)` — 构造时绑定 CodeBuffer
- 每个 Emit* 方法转发到 x64_encoder.h 中对应的内联函数
- 不需要重写编码逻辑，完全复用现有代码

```cpp
class X64Encoder : public IEncoder {
public:
    explicit X64Encoder(CodeBuffer& buf) : buf_(buf) {}
    
    void EmitMovRR(uint8_t dst, uint8_t src) override {
        EmitMovRR(buf_, dst, src);  // 调用 x64_encoder.h 的内联函数
    }
    // ... 约 60 个转发函数

private:
    CodeBuffer& buf_;
};
```

### Step 3: 创建 ISehHandler.h

```cpp
class ISehHandler {
public:
    virtual ~ISehHandler() = default;
    
    virtual void RegisterCode(void* code_start, uint32_t code_size,
                              const JitMethod* nm, uint32_t patch_method_token) = 0;
    virtual void UnregisterCode(void* code_start) = 0;
    virtual const JitMethod* FindCodeByAddress(const void* address) = 0;
    virtual uint32_t DemoteByToken(uint32_t method_token) = 0;
    virtual uint32_t DemoteByCallSiteToken(uint32_t method_token) = 0;
    virtual void ReclaimDemoted() = 0;
    virtual void Initialize() = 0;  // 注册 VEH/signal handler
};
```

注意：`ChaosT4RaiseException` **不**放入接口，因为它有 `extern "C"` 链接要求（被生成代码直接调用）。它继续保留为 jit_seh.cpp 中的自由函数。

### Step 4: 创建 WinSehHandler 类

将 jit_seh.cpp 中的全局函数和全局状态封装为 `WinSehHandler : ISehHandler`：
- `g_t4_code_entries[]` → WinSehHandler 成员
- `g_t4_code_lock` → WinSehHandler 成员  
- `g_t4_lookup_cache` → 保留 thread_local（静态成员或自由 thread_local）

静态函数 `T4VectoredExceptionHandler`、`T4PersonalityRoutine` 保持为 WinSehHandler 的静态成员或自由函数（它们需要被 Windows API 调用）。

### Step 5: 重构 jit_engine.cpp

NativeCodeGenerator 的主要变更：

```cpp
class NativeCodeGenerator {
public:
    NativeCodeGenerator(const RegisterMethod& rm,
                        const CompileConfig& config,
                        IEncoder& encoder,
                        ISehHandler& seh)
        : rm_(rm), config_(config), enc_(encoder), seh_(seh) {}
private:
    IEncoder& enc_;       // 替换直接调用 x64_encoder.h 内联函数
    ISehHandler& seh_;   // 替换直接调用 RegisterT4Code 等
};
```

所有 `EmitXxx(buf_, ...)` 改为 `enc_.EmitXxx(...)`。

全局函数调用：
- `RegisterT4Code(...)` → `seh_.RegisterCode(...)`
- `UnregisterT4Code(...)` → `seh_.UnregisterCode(...)`
- `FindT4CodeByAddress(...)` → `seh_.FindCodeByAddress(...)`

### Step 6: 更新 Compile() 入口

```cpp
JitMethod* Compile(const RegisterMethod& rm, const CompileConfig& config) noexcept {
    static X64Encoder x64_enc(g_code_buffer);  // 或每次构造传入
    static WinSehHandler win_seh;
    
    NativeCodeGenerator gen(rm, config, x64_enc, win_seh);
    return gen.Generate();
}
```

### Step 7: 更新 CMakeLists.txt

添加 `IEncoder.h`、`ISehHandler.h` 到头文件列表。

### Step 8: 构建验证

```bash
cmake --build build/testing --target chaos_jit
ctest --test-dir build/testing -R test_jit --output-on-failure
```

---

## 验证命令

```bash
# 构建
cmake -S . -B build/testing --preset debug 2>&1 | tail -5
cmake --build build/testing --target chaos_jit 2>&1 | tail -20

# 运行 JIT 测试
ctest --test-dir build/testing -R test_jit --output-on-failure

# 运行 codegen 测试
ctest --test-dir build/testing -R test_codegen --output-on-failure
```
