# Brainstorm v1-02

## 新问题

- Batch 1 已把共享 host harness 落到 `src/mobile/shared/`。
- 现在需要推进 Android Host 的 2.1 / 2.2 / 2.3：
  - `src/mobile/android/app/`
  - `src/mobile/android/jni/`
  - `src/mobile/android/CMakeLists.txt`

## 方案比较

### 方案 A：立刻把现有 `tests/gate/android-smoke` 全部替换成 `src/mobile/android/`

- 优点
  - Android 路径一次切到目标目录
- 缺点
  - 会同时改变现有 gate 路由与新骨架
  - 出问题时很难区分是共享 host、JNI bridge 还是 preset 路由的问题

### 方案 B：保留现有 `tests/gate/android-smoke` 作为 gate，新增 `src/mobile/android/` 独立骨架

- 优点
  - 现有 gate 不回退
  - Android app / JNI / CMake 可以独立生长
  - 后续接入 root preset 时改动更可控
- 缺点
  - 短期内会有两层 Android 壳子并存

## 结论

- 选择方案 B。
- 这一轮只建立 `src/mobile/android/` 的最小结构与 JNI -> `il2cpp_host_run(...)` 的桥接，不在本轮修改 root preset 路由。
