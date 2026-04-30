# 02-翻译路径参考 INDEX

> 按 IL 特性分类的翻译路径表。AI 开发时查表用，找到目标 IL 特性的翻译策略、文件位置和约束。

## 文档

| 文档 | 说明 |
|------|------|
| [`01-基础指令翻译表.md`](./01-%E5%9F%BA%E7%A1%80%E6%8C%87%E4%BB%A4%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | 加载常量、参数/局部变量、类型转换、算术/位运算、比较、分支、栈操作 |
| [`02-调用与字段指令翻译表.md`](./02-%E8%B0%83%E7%94%A8%E4%B8%8E%E5%AD%97%E6%AE%B5%E6%8C%87%E4%BB%A4%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | call/callvirt/newobj/ldftn/calli/ret、字段访问、ldtoken |
| [`03-数组与类型检查翻译表.md`](./03-%E6%95%B0%E7%BB%84%E4%B8%8E%E7%B1%BB%E5%9E%8B%E6%A3%80%E6%9F%A5%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | newarr/ldelem/stelem、castclass/isinst/box/unbox、对象/间接操作 |
| [`04-异常Shape翻译表.md`](./04-%E5%BC%82%E5%B8%B8Shape%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | 5种 EH shape 的翻译策略和约束 |
| [`05-委托与运行时Helper翻译表.md`](./05-%E5%A7%94%E6%89%98%E4%B8%8E%E8%BF%90%E8%A1%8C%E6%97%B6Helper%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | delegate 生命周期、16 类 helper 触发条件和文件 |
| [`06-静态初始化与对象相等性翻译表.md`](./06-%E9%9D%99%E6%80%81%E5%88%9D%E5%A7%8B%E5%8C%96%E4%B8%8E%E5%AF%B9%E8%B1%A1%E7%9B%B8%E7%AD%89%E6%80%A7%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | cctor 降级、Object.Equals/String.op_Equality |
| [`07-已消除的禁止模式.md`](./07-%E5%B7%B2%E6%B6%88%E9%99%A4%E7%9A%84%E7%A6%81%E6%AD%A2%E6%A8%A1%E5%BC%8F.md) | 已修复的性能反模式及其替代方案 |
| [`08-collections-kernel翻译表.md`](./08-collections-kernel%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | Collection 类型热路径 Kernel 优化翻译路径 |
| [`09-结构化控制流恢复翻译表.md`](./09-%E7%BB%93%E6%9E%84%E5%8C%96%E6%8E%A7%E5%88%B6%E6%B5%81%E6%81%A2%E5%A4%8D%E7%BF%BB%E8%AF%91%E8%A1%A8.md) | CFG 构建、结构化恢复算法与发射策略 |
