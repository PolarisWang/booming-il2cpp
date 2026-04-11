# Mobile Runtime And Host Matrix 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。  
> **目标：** 把 Android/iOS 从工具链 gate 推进到具备 runtime/host 证据的移动端验证路径，并建立 mobile host harness。  
> **架构：** 新建 `src/mobile/` 目录（android/ios/shared），不属于 IL2CPP 核心；再通过 subject-scoped mobile host root 把移动端证据挂回 subject。  
> **技术栈：** C/C++, Java/Kotlin (Android), Objective-C (iOS), CMake, Gradle, Xcode

**设计文档：** `phase-architecture-v1-01.md` Phase 4 节

---

## 步骤

### Batch 1: Shared Host Framework

- [x] **1.1** 创建 `src/mobile/shared/host_main.cpp`：统一入口（init runtime -> run entry -> report result）
- [x] **1.2** 创建 `src/mobile/shared/result_reporter.cpp`：平台适配日志（stdout/logcat/NSLog）
- [x] **1.3** 定义 host harness 接口：`int il2cpp_host_run(int argc, const char** argv)`

### Batch 2: Android Host

- [x] **2.1** 创建 `src/mobile/android/app/`：最小 Android Activity（Kotlin/Java）
- [x] **2.2** 创建 `src/mobile/android/jni/`：JNI bridge（Java -> C -> host_main）
- [x] **2.3** 创建 `src/mobile/android/CMakeLists.txt`：NDK 构建配置
- [ ] **2.4** Android 启动验证：HelloWorld runtime 正常输出
- [ ] **2.5** 建立 Android crash 收集：`ndk-stack` + tombstone 解析

### Batch 3: iOS Host

- [x] **3.1** 创建 `src/mobile/ios/app/`：最小 iOS App Delegate（Objective-C）
- [x] **3.2** 创建 `src/mobile/ios/bridge/`：ObjC -> C -> host_main
- [x] **3.3** 创建 `src/mobile/ios/CMakeLists.txt`：Xcode 构建配置
- [ ] **3.4** iOS simulator 启动验证：HelloWorld runtime
- [ ] **3.5** iOS device：code signing + provisioning profile 配置
- [ ] **3.6** iOS device 启动验证
- [ ] **3.7** 建立 iOS crash 收集：dsym symbolication

### Batch 4: Subject + Performance

- [x] **4.1** 创建 `subjects/MobileHelloWorldProof/`：专为移动端设计的最小 subject
- [ ] **4.2** Android arm64 perf baseline：冷启动 <= 500ms，pkg <= 5MB，idle memory <= 20MB
- [ ] **4.3** iOS arm64 perf baseline：冷启动 <= 300ms，pkg <= 3MB，idle memory <= 15MB
- [ ] **4.4** 建立 package size tracking（stripped binary size per platform）
