# Brainstorm v1-04

## 新问题

- Batch 1/2/3 已经把 `src/mobile/shared/`、`src/mobile/android/`、`src/mobile/ios/` 骨架立起来。
- 当前缺口不再是“有没有 mobile host 目录”，而是“mobile 证据是否归属于某个真实 subject”。
- 现有 `tests/gate/android-smoke` / `tests/gate/ios-smoke` 只能证明 preset routing，不表达 subject 归属，也没有 iOS `build-target` worker 接线。

## 方案比较

### 方案 A：直接复用 `HelloWorld`

- 优点
  - 不新增 subject，代码最少。
- 缺点
  - 会把“桌面 managed smoke”与“移动端 buildable proof”混在一个 subject 里。
  - 后续 mobile 专项矩阵、host root、runtime evidence 会继续污染通用 `HelloWorld`。

### 方案 B：新增 `MobileHelloWorldProof`，并让 Android/iOS host root 归属于 subject

- 优点
  - mobile 证据与通用 smoke 隔离，后续可以独立扩展 matrix、runtime、perf。
  - root CMake 只需要增加 subject-scoped cache 变量，保留 `tests/gate/*` 作为 fallback。
  - `build-target` worker 可以最小增量补齐 iOS validate-only 路径。
- 缺点
  - 需要复制一个最小 managed source。

### 方案 C：立即把 root preset 全量切到 `src/mobile/android` / `src/mobile/ios`

- 优点
  - 长期结构更直接。
- 缺点
  - 这一步会把“subject 接线”“平台 skeleton 切换”“真实 runtime 证据”三件事绑死到同一轮。
  - 当前 Windows 宿主不适合同时验证 iOS 非 validate-only 路径。

## 结论

- 选择方案 B。
- 本轮目标限定为：
  - 新增 `subjects/MobileHelloWorldProof/`
  - 为 subject 增加 `windows-managed-output`、`windows-android-buildable`、`windows-ios-buildable`
  - root CMake 增加 `CHAOS_SUBJECT_ANDROID_HOST_ROOT` / `CHAOS_SUBJECT_IOS_HOST_ROOT`
  - `subject_workers.py` 优先路由到 `subjects/<SubjectId>/validation/mobile/{android-host,ios-host}`
- 本轮不宣称：
  - Android 真机/模拟器 runtime 已完成
  - iOS simulator/device 已完成
  - `src/mobile/android` / `src/mobile/ios` 已完全替代通用 gate
