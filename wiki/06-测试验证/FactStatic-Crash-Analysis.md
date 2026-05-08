# Fact Static Crash 分析方法论

## 问题

Fact Static verify exe (如 `fact_static_convert-char.exe`) 在运行中 crash，只输出 `failed (0/0)`。由于 exe 是静态链接的，无法直接用 VS 调试器 attach。需要用 **MAP 文件 + cdb** 将 crash offset 解析为源码级符号。

## 前置条件

1. **MAP 文件**：`fact_verifier.py` 的 link 命令已添加 `/MAP` flag（2026-05-08 起默认启用）。每次 Fact Static rebuild 会在 build 目录生成 `fact_static_<family>.map`。
2. **cdb.exe**：Windows Debugger（WinDbg），包含在 Windows SDK 中。典型路径：
   - `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe`

## 工作流

### 第一步：一键分析

```bash
python build/toolchains/run/testing/foundation_dll/debug_fact_verify.py <family>
```

脚本自动：
1. 定位 `build/fact_static_<family>.map`
2. 解析 map 文件中所有 section 0001（`.text`）的符号表
3. 在 cdb 下运行 exe，捕获 crash offset + 调用栈
4. 将每个栈帧的 RVA 映射为符号名

输出示例：
```
  Map: .../build/fact_static_convert-char.map  (2006 code symbols)
  Running cdb on: .../build/fact_static_convert-char.exe
============================================================
CRASH ANALYSIS
============================================================
  Exception:  c0000005
  Fault addr: 0000000000000040 (read)

  Stack (7 frames):
    #0:  ConvertCharNativeEntry_ConvertCharNativeEntry_Method4 + 0xcf
    ...

  CRASH SITE: ConvertCharNativeEntry_ConvertCharNativeEntry_Method4 + 0xcf
```

### 第二步：手动指定 offset

如果已经在 cdb 中拿到了 crash offset（如 `fact_static_convert_char+0x1bef`），可以跳过 cdb 重跑：

```bash
python build/toolchains/run/testing/foundation_dll/debug_fact_verify.py <family> --offset 0x1bef
```

### 第三步：反查源码

根据解析出的符号（如 `ConvertCharNativeEntry_ConvertCharNativeEntry_Method4`），在 generated `.cpp` 中搜索该函数，定位到 `+ 0xcf` 偏移附近的代码行。

## 手工流程（脱离脚本）

如果不使用 debug_fact_verify.py，也可以手工完成相同的分析：

### 1. 获取 crash offset

```bash
cdb -c "g; .exr -1; kb 10; q" -o fact_static_<family>.exe
```

输出中 `fact_static_<family>+0x1bef` 即为 crash offset。`.exr -1` 显示异常详细信息（读/写、地址值）。

### 2. 从 MAP 文件查询符号

map 文件的 `Publics by Value` 段列出了每个符号的 section:offset。找到最大不超过 crash offset 的符号：

```
0001:00001b20 ConvertCharNativeEntry_ConvertCharNativeEntry_Method4
```

对于 offset `0x1bef`，`0x1b20` <= `0x1bef` 且差值最小 → crash 在 `Method4 + 0xcf`。

### 3. 确认调用链

cdb 的 `kb 10` 输出多个栈帧，每个 frame 的 offset 都按上述方法查 map。调用链可以揭示 crash 是由哪个上层调用触发的。

## 常见 crash 类型及根因

| 异常码 | 访问地址 | 典型根因 | 排查方向 |
|--------|---------|---------|---------|
| `c0000005` (AV) | 低地址如 `0x40` | 空指针成员访问 | 检查参数是否为 nullptr，或返回值是否为 `0` 被当作指针使用 |
| `c0000005` (AV) | 奇数地址如 `0x41` | bit-0 tag 碰撞 | 检查 `chaos_managed_pointer_local_slot_tag` 是否未在 verify mode 下被 override |
| `c0000005` (AV) | 静态数据区 | 跨 TU static init 时序 | 检查无序的动态初始化（`std::vector`、`std::unordered_map`）是否被生成代码的 static init 访问 |
| `c0000094` | - | 整数除零 | 检查除法运算中除数是否为 0 |

## MAP 文件格式参考

```
 Start         Length     Name                   Class
 0001:00000000 00000800H .text$di                CODE
 0001:00000800 0003f8d0H .text$mn                CODE
 ...

  Address         Publics by Value              Rva+Base               Lib:Object
 0001:00000860       main                       0000000140001860 f   fact_verify_combined.obj
 0001:00000bc0       ?chaos_normalize_native_int_... 0000000140001bc0 f   fact_verify_combined.obj
 0001:00001440       ConvertCharNativeEntry_Con... 0000000140002440 f   fact_verify_combined.obj
```

- `0001:00000860` = section `0001` + offset `0x860` = 实际 RVA
- `Rva+Base` = 首选加载基址（`0x140000000`）+ RVA
- `f` = function（`f`），`i` = static function（`f i`）
- `fact_verify_combined.obj` = 符号来源的对象文件

## 工具参考

- **`debug_fact_verify.py`**：`build/toolchains/run/testing/foundation_dll/debug_fact_verify.py` — 一键式 crash 分析
- **cdb**：`C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe` — 命令行调试器
- **dumpbin**：随 MSVC 工具链提供 — `dumpbin /SYMBOLS *.obj` 可以列出对象文件的符号偏移
- **MAP 文件**：`build/fact_static_<family>.map` — link.exe 生成的函数地址映射
