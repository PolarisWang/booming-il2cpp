# child-A-P1-1: A0 FastFrame 容量扩展 + A1 SEH in FastExecute

> 父任务: 20260523-codegen-industrialization
> 创建日期: 2026-05-23
> entry_skill: dev-writing-plans

---

## Scope

补齐解释器基础设施的两个关键缺失：
1. **A0 FastFrame 容量扩展**: 当前 FastFrame 在深度嵌套/大帧场景可能溢出，需要扩展容量并验证边界
2. **A1 SEH in FastExecute**: FastExecute 主循环缺少结构化异常处理支持，导致异常无法在解释器执行路径正确传播

这两个是 Thread A（托管 AOT codegen）后续所有工作的前提。

## Inputs

- 现有 FastFrame 实现: `src/native/interpreter/fast_frame_pool.h`
- FastExecute 实现: `src/native/interpreter/fast_dispatch.cpp`
- 异常处理架构设计: `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/05-异常处理架构.md`

## Expected Outputs

- FastFrame 容量验证/扩展实现
- SEH in FastExecute 实现
- 相关测试通过

## Exit Criteria

- FastFrame 在压力场景下稳定运行（不溢出）
- FastExecute 路径可正确传播异常
- 现有测试全部通过

## Parent Task ID

`20260523-codegen-industrialization`

## Source Task ID

`A-P1-1`

## Source Relation

`roadmap-child`

## Clearance Source

`parent-roadmap`

## Blocking Questions

`[]`

## Question Clearance

`cleared`

## Clearance Confirmed By User

`true`
