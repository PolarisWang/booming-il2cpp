# ReflectionLite

## 负责的 contract 面

- `Type`
- `Field`
- `Method`
- `Property`
- `Event`
- `Parameter`
- 部分 `GenericContext`

这里是高阶反射 contract 的主体项目。后续只要属于成员发现、签名读取、泛型上下文闭合与最小反射访问序列，都优先归到这里。

## 不负责的内容

- interop / native 边界
- host lifecycle 与 bootstrap 流
- 完整运行时 metadata cache 实现

## 子场景清单

- 闭合类型的 `Type` 发现
- 字段、属性、事件、方法与参数签名读取
- 泛型方法闭合后的返回类型观察
- 泛型类型定义与闭合实例之间的最小上下文切换
