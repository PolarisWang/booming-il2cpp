# Brainstorm v1-03

## 新问题

- Android skeleton 已建立。
- 现在需要推进 iOS Host 的 3.1 / 3.2 / 3.3：
  - `src/mobile/ios/app/`
  - `src/mobile/ios/bridge/`
  - `src/mobile/ios/CMakeLists.txt`

## 方案比较

### 方案 A：立刻把现有 `tests/gate/ios-smoke` 改成 `src/mobile/ios/`

- 优点
  - 路径一次到位
- 缺点
  - 会把 iOS packaging gate 与新骨架同时改动
  - 当前 Windows 宿主无法对 ObjC/Xcode 流程做真实验证

### 方案 B：保留现有 `tests/gate/ios-smoke`，新增 `src/mobile/ios/` 独立骨架

- 优点
  - 与 Android 路线一致
  - iOS app / bridge / CMake 可以先独立收敛
  - 后续切换 root preset 时更可控

## 结论

- 选择方案 B。
- 这一轮只建立 `src/mobile/ios/` 的最小 App Delegate / ObjC bridge / CMake 骨架，并确保 bridge 明确转发到 `il2cpp_host_run(...)`。
