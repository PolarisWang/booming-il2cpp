# Release Checklist

## 目的

- 约束每次发布都通过同一套 productization gates。
- 防止只凭单次 smoke 或局部 proof 就进入分发环节。

## 发布前必查项

### 1. 版本兼容

- 已确认 `AOT 主包版本` 与 `hot update patch` 落在允许的 compatibility band。
- 已根据 `version-compatibility-matrix.md` 复核版本兼容规则。

### 2. rollback

- 已执行 apply -> verify -> rollback -> verify AOT-only -> re-apply 演练。
- rollback 后 active patch 清单归零，运行模式回到 AOT-only。

### 3. mobile runtime host

- 若本次发布包含移动端范围，必须附带最新 `mobile runtime host` 真实运行证据。
- Android 证据需要来自真实 NDK / emulator / device。
- iOS 证据需要来自 macOS/Xcode 对应 host 环境。

### 4. iOS distribution policy

- 若发布面向 iOS，必须复核 `iOS distribution policy`。
- 需要明确本次变更属于配置/数据热更新，还是逻辑热更新。
- 若触及逻辑热更新，必须附带额外合规评估结论。

### 5. perf regression

- 已检查关键 subject 与目标平台的 `perf regression` 基线。
- 没有未经解释的性能回归进入发布包。

### 6. soak test

- 对需要长时间稳定性的发布，必须补齐 `soak test` 结果。
- soak test 结果需要包含运行时长、内存增长、CPU 和 crash 结论。

### 7. unsupported feature report

- 已生成并审阅 `unsupported feature report`。
- 所有新增限制、降级行为和未支持 CLR 特性都已明确告知。

## 发布结论

- 以上 gate 全部满足后，才允许进入最终发布评审。
- 任一 gate 缺失时，发布结论应为阻塞，而不是“先发再补”。
