# Chunk 3：Native Contract Pack（ABI / Bridge）

## 目标

把 `contracts/native/` 落成完整的 `v0` contract pack：header、docs、example 与 compile-only smoke 全部到位。

### 任务 1：创建 ABI header、docs 与 example

**文件：**
- 创建：`contracts/native/v0/runtime_abi.h`
- 创建：`contracts/native/docs/v0/runtime-abi.md`
- 创建：`contracts/native/examples/v0/abi/runtime-lifecycle.cpp`
- 创建：`contracts/native/examples/v0/abi/reflection-handles.cpp`

- [ ] **步骤 1：创建 native contract 目录**

运行：
```powershell
New-Item -ItemType Directory contracts/native/v0,contracts/native/docs/v0,contracts/native/examples/v0/abi -Force
```

- [ ] **步骤 2：编写 `runtime_abi.h` 的句柄与状态类型**

必须包含不透明句柄：
- `TypeInfo`
- `MethodInfo`
- `FieldInfo`
- `PropertyInfo`
- `EventInfo`
- `ParameterInfo`
- `GenericContext`
- `Assembly/Image`
- `Exception`

并定义状态类型与基本 API 获取入口。

- [ ] **步骤 3：补齐 ABI v0 的基础函数族**

至少覆盖：
- runtime init / shutdown
- thread attach / detach
- object / array / string allocation
- class init
- GC handle new / free
- raise managed exception
- 通过句柄驱动的字段访问 / 方法调用 / 反射查询入口

- [ ] **步骤 4：写 `runtime-abi.md`**

必须说明：
- ABI 是低层基础操作面
- 错误边界更底层、更显式
- 托管异常通过专门入口显式触发
- 高阶句柄按完整语义承诺处理

- [ ] **步骤 5：编写 2 个 ABI example**

`runtime-lifecycle.cpp`：
- 展示 init / shutdown / thread attach / detach

`reflection-handles.cpp`：
- 展示 handle 获取与最小反射查询路径

### 任务 2：创建 bridge header、docs 与 example

**文件：**
- 创建：`contracts/native/v0/codegen_bridge.h`
- 创建：`contracts/native/docs/v0/codegen-bridge.md`
- 创建：`contracts/native/examples/v0/bridge/registration.cpp`
- 创建：`contracts/native/examples/v0/bridge/invoke-and-delegate.cpp`

- [ ] **步骤 1：创建 bridge example 目录**

运行：
```powershell
New-Item -ItemType Directory contracts/native/examples/v0/bridge -Force
```

- [ ] **步骤 2：编写 `codegen_bridge.h` 的 helper 分类**

至少包含：
- registration helper
- runtime metadata helper
- boxing / unboxing helper
- virtual invoke helper
- delegate helper
- icall resolve helper

- [ ] **步骤 3：把高阶 helper 冻结到全细节级**

要求在 header 和 docs 中写清：
- 函数签名
- 参数语义
- 调用约定
- 错误模型

错误模型必须采用：
- 查询/解析类返回状态或空指针
- 托管语义失败走受控异常出口

- [ ] **步骤 4：写 `codegen-bridge.md`**

文档要明确：
- bridge 是 generated C++ 优先依赖的高层 helper 面
- `virtual invoke` / `delegate` / `icall resolve` 已进入 `bridge v0`
- 与 ABI 的职责分界

- [ ] **步骤 5：编写 2 个 bridge example**

`registration.cpp`：
- 展示 code registration / metadata registration / bootstrap 相关调用

`invoke-and-delegate.cpp`：
- 展示 virtual invoke / delegate / icall resolve 的调用形态

### 任务 3：建立 compile-only smoke

**文件：**
- 创建：`tests/contracts/native/abi/CMakeLists.txt`
- 创建：`tests/contracts/native/abi/compile_only_runtime_abi_smoke.cpp`
- 创建：`tests/contracts/native/bridge/CMakeLists.txt`
- 创建：`tests/contracts/native/bridge/compile_only_codegen_bridge_smoke.cpp`

- [ ] **步骤 1：创建测试目录**

运行：
```powershell
New-Item -ItemType Directory tests/contracts/native/abi,tests/contracts/native/bridge -Force
```

- [ ] **步骤 2：写 ABI compile-only smoke**

要求：
- 只 include `runtime_abi.h`
- 编译时覆盖生命周期、分配、句柄类型引用
- 不要求真实 runtime 链接

- [ ] **步骤 3：写 bridge compile-only smoke**

要求：
- 只 include `codegen_bridge.h`
- 编译时覆盖 registration、invoke、delegate、icall resolve
- 不要求真实 bridge/runtime 链接

- [ ] **步骤 4：写两份最小 CMakeLists**

要求：
- 输出目录落到 `artifacts/`
- 只做 compile-only target

- [ ] **步骤 5：运行 ABI smoke**

运行：
```powershell
cmake -S tests/contracts/native/abi -B artifacts/native-abi-smoke -G Ninja
cmake --build artifacts/native-abi-smoke
```

预期：configure 和 build 均通过。

- [ ] **步骤 6：运行 bridge smoke**

运行：
```powershell
cmake -S tests/contracts/native/bridge -B artifacts/native-bridge-smoke -G Ninja
cmake --build artifacts/native-bridge-smoke
```

预期：configure 和 build 均通过。

- [ ] **步骤 7：提交当前 chunk**

运行：
```bash
git add contracts/native tests/contracts/native
git commit -m "docs: add roadmap0 native contract pack"
```

预期：提交只包含 native contract pack 与对应 compile-only smoke。
