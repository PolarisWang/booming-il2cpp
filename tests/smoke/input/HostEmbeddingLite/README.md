# HostEmbeddingLite

## 负责的 contract 面

- host lifecycle
- thread attach / detach
- managed entry
- delegate / callback
- 文件系统
- 时间 / 时区
- 动态库加载
- registration 流
- metadata / bootstrap 状态机

这是唯一允许承载 host 侧总控语义的 smoke 输入项目。凡是“由宿主驱动 managed world”的 contract，都应优先落到这里。

## 不负责的内容

- 复杂业务反射覆盖主体
- 独立 interop 主线
- 纯泛型调用主线
- 完整生产级 runtime / GC / loader 实现

## 子场景清单

- host 启停与生命周期状态变化
- 线程 attach / detach 计数
- managed entry 调用与 delegate / callback 往返
- 临时文件写入与读取
- 本地时间、偏移与时区读取
- 平台动态库探测与装载
- registration 流最小序列
- metadata / bootstrap 状态推进
- 首次反射访问触发的 lazy warmup

## 四段 bootstrap 状态机

- `registration complete`
  宿主完成程序集、managed export 与 callback 入口登记；这是后续 metadata 初始化的前置条件。
- `metadata cache initialize`
  metadata 查询所需的最小缓存完成初始化，但此时仍不假设所有 GC 相关依赖都已可用。
- `GC-dependent late-init`
  只在 GC 相关依赖已经可用后执行的迟后初始化阶段；它把后续 class world 准备与真实对象交互前置条件补齐。
- `class world / reflection cache warmup`
  这一段不是 eager 启动流程，而是按需触发的 lazy warmup；只有在第一次类世界访问或反射访问发生时才进入。

## lazy warmup 约束

最后一段 warmup 必须依赖 trace contract 验证触发顺序，而不是在进程启动时直接偷跑。验证时应确认：

- `registration complete` 先于 `metadata cache initialize`
- `metadata cache initialize` 先于 `GC-dependent late-init`
- `GC-dependent late-init` 先于第一次 reflection 请求
- `class world / reflection cache warmup` 只在 reflection 请求之后出现
