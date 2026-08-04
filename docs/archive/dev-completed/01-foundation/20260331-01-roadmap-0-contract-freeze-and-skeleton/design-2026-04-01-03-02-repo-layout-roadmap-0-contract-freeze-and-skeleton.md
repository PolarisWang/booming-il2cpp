# 第 2 段：仓库骨架、目录责任与落盘位置

## 顶层骨架

`Roadmap 0` 采用全骨架 upfront 策略。
顶层至少落下以下目录：

- `frontend/`
- `analysis/`
- `runtime/`
- `codegen/`
- `build/`
- `tests/`
- `contracts/`
- `third_party/`
- `artifacts/`

其中：

- `frontend/analysis/runtime/codegen` 主要承担未来模块归属与边界说明
- `contracts/` 承担正式 contract pack
- `third_party/` 承担第三方 vendored 源码
- `artifacts/` 承担构建与验证运行输出

## 正式 contract 资产位置

### 分析/产物格式 contract

- `analysis/contracts/schemas/`
- `analysis/contracts/examples/`
- `tests/contracts/schema/`

约定：

- `schemas/` 放正式 schema
- `examples/` 放人维护 example
- `tests/contracts/schema/` 放权威 snapshot

### 公共 native contract pack

- `contracts/native/v0/`
- `contracts/native/docs/v0/`
- `contracts/native/examples/v0/`
- `tests/contracts/native/abi/`
- `tests/contracts/native/bridge/`

约定：

- `contracts/native/v0/` 放 header
- `contracts/native/docs/v0/` 放人读文档
- `contracts/native/examples/v0/` 放 example
- `tests/contracts/native/abi/` 与 `tests/contracts/native/bridge/` 放 smoke 与验证

## 第三方依赖规则

### 目录与范围

第三方依赖目录统一采用：

- `third_party/`

该目录只承载 vendored 源码，不承载：

- 工具链
- 下载缓存
- 构建产物
- 临时文件

### 引入方式

默认采用手动 vendor snapshot。
不默认采用：

- git submodule
- git subtree

### 元数据记录

第三方依赖采用双层记录：

- 每个依赖目录内放 `README.md`
- 根级放 `third_party/INDEX.md`

依赖目录内 `README.md` 采用扩展字段集合，至少包括：

- 来源
- 版本 / commit
- 许可证
- 引入原因
- 局部修改说明
- 升级注意事项
- 构建接入方式
- 包含路径
- 平台限制

根级 `INDEX.md` 采用精简总表，只列：

- 名称
- 版本 / commit
- 许可证
- 用途
- 目录路径

## 工具链、缓存与产物目录

### 工具链描述

工具链描述保留在仓库内：

- `build/toolchains/`

### 仓库外内容

以下内容全部放仓库外：

- 实际下载缓存
- 二进制依赖
- 外部工具链下载物

### 产物目录

仓库内统一使用：

- `artifacts/`

承载：

- CMake build 目录
- 日志
- 打包产物
- 运行时生成的快照副本
- diff 输出

注意：

- 权威 snapshot 继续留在源码树中
- `artifacts/` 不承接版本管理中的 canonical snapshot
