# P1-1: POSIX EH — PalTryCallNoExcept 实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `parent_task_id`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 将 POSIX 平台上 PalTryCallNoExcept 从 passthrough 改造为使用 sigsetjmp/siglongjmp + signal handler 的完整异常保护，使 POSIX EH 与 Windows SEH `__try/__except` 语义对等。

**架构：** Thread-local sigjmp_buf 保存上下文 + SIGSEGV/SIGBUS 全局信号处理程序 + TLS 标志位区分保护区域。信号处理程序在 `g_pal_try_active` 为 true 时 siglongjmp 恢复执行，否则链式传递到前序 handler。同时修正 JIT signal handler (LinuxSehHandler) 的安装方式以支持链式传递。

**技术栈：** sigsetjmp/siglongjmp, sigaction, TLS (thread_local), POSIX信号

**架构审核模式：** normal

**结构告警重点：** 信号处理程序的链式架构 vs 各自为政；避免全局状态的竞态

**权责图审核主题：** n/a

**设计文档：** n/a (上游问题已清零)

**问题清零来源：** parent-roadmap (20260604-execution-path-risk-remediation)

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 修改文件清单

| 文件 | 修改类型 | 职责 |
|------|---------|------|
| `src/native/pal/pal_eh_posix.cpp` | **重写** | sigsetjmp/siglongjmp 保护层 + signal handler 安装/拆卸 |
| `src/native/pal/chaos/pal/pal_eh.h` | **更新注释** | 同步 POSIX 行为描述（不再返回 false） |
| `src/native/jit/LinuxSehHandler.cpp` | **修改** | sigaction 安装改为保存旧 handler + JitSignalHandler 链式传递 |
| `src/native/jit/LinuxSehHandler.h` | **修改** | 新增 `s_prev_segv` / `s_prev_bus` 静态成员保存旧 handler |

## 设计

### PalTryCallNoExcept 实现

```cpp
// pal_eh_posix.cpp

#include <chaos/pal/pal_eh.h>
#include <csignal>
#include <csetjmp>

namespace chaos::il2cpp::pal {
namespace {

// TLS: sigjmp_buf for current protected region
thread_local sigjmp_buf g_pal_try_jmp_buf;
// TLS: flag indicating we're inside a protected region
thread_local bool g_pal_try_active = false;

// Saved previous SIGSEGV handler (global, installed once)
static struct sigaction s_prev_segv;
static struct sigaction s_prev_bus;
static std::atomic<bool> s_handler_installed{false};

// Signal handler for SIGSEGV/SIGBUS
void PalEhSignalHandler(int sig, siginfo_t* info, void* ucontext) noexcept {
    if (g_pal_try_active) {
        // Inside PalTryCallNoExcept — longjmp back to saved context
        siglongjmp(g_pal_try_jmp_buf, 1);
    }
    // Chain to previous handler
    struct sigaction* prev = (sig == SIGSEGV) ? &s_prev_segv : &s_prev_bus;
    if (prev->sa_flags & SA_SIGINFO) {
        if (prev->sa_sigaction) {
            prev->sa_sigaction(sig, info, ucontext);
        }
    } else if (prev->sa_handler != SIG_DFL && prev->sa_handler != SIG_IGN) {
        prev->sa_handler(sig);
    }
    // SIG_DFL — default action (terminate)
}

void EnsureHandlerInstalled() noexcept {
    if (s_handler_installed.load(std::memory_order_acquire)) return;
    
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = PalEhSignalHandler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    
    sigaction(SIGSEGV, &sa, &s_prev_segv);
    sigaction(SIGBUS, &sa, &s_prev_bus);
    
    s_handler_installed.store(true, std::memory_order_release);
}

}  // anonymous namespace

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept
{
    EnsureHandlerInstalled();
    
    g_pal_try_active = true;
    if (sigsetjmp(g_pal_try_jmp_buf, 1) == 0) {
        // Normal path: call the function
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
        g_pal_try_active = false;
        return false;
    } else {
        // Signal was caught and we longjmp'd back
        g_pal_try_active = false;
        return true;
    }
}

}  // namespace chaos::il2cpp::pal
```

### JIT Signal Handler 链式修正

JIT handler 安装时需保存旧 handler，在 handler 末尾（找不到对应 JIT code 时）链式传递给旧 handler：

```cpp
// LinuxSehHandler.h — add static members for saved handlers
static struct sigaction s_prev_segv;
static struct sigaction s_prev_bus;

// LinuxSehHandler.cpp — in Initialize()
sigaction(SIGSEGV, &sa, &s_prev_segv);  // was: nullptr
sigaction(SIGBUS,  &sa, &s_prev_bus);    // was: nullptr

// In JitSignalHandler(), at the end when no JIT code matches:
// Chain to previous handler
struct sigaction* prev = ...;
if (prev->sa_flags & SA_SIGINFO) {
    if (prev->sa_sigaction) prev->sa_sigaction(sig, info, ucontext);
} else if (prev->sa_handler != SIG_DFL && prev->sa_handler != SIG_IGN) {
    prev->sa_handler(sig);
}
// No explicit fallback; SIG_DFL will be applied by OS if nobody handles
```

### 链式流程

```
SIGSEGV/SIGBUS occurs
    │
    ├─ Is fault in JIT code? (JitSignalHandler runs first)
    │   ├─ YES → Handle managed exception (SEH table lookup)
    │   └─ NO  → Chain to previous handler
    │               │
    │               ├─ Is g_pal_try_active? (PalEhSignalHandler)
    │               │   ├─ YES → siglongjmp back to PalTryCallNoExcept
    │               │   └─ NO  → Chain to previous handler
    │               │               │
    │               │               └─ SIG_DFL → OS default (terminate + core dump)
    │
```

## 验证计划

1. 构建 CHECK tier: `cmake --preset debug && cmake --build --preset debug`
2. 运行 foundation-dll 测试确认无 regression
3. 通过基础测试在 interpreter 路径制造 SIGSEGV 并确认 PalTryCallNoExcept 捕获

## 执行步骤

- [ ] Step 1: 实现 pal_eh_posix.cpp 的 sigsetjmp/siglongjmp + signal handler
- [ ] Step 2: 更新 pal_eh.h 注释说明 POSIX 行为
- [ ] Step 3: 修正 JIT signal handler 安装，支持链式传递
- [ ] Step 4: 构建 CHECK tier 验证
- [ ] Step 5: 测试验证（foundation-dll tests）
- [ ] Step 6: 结构告警与架构审视
- [ ] Step 7: 归档 completed 并同步父任务
