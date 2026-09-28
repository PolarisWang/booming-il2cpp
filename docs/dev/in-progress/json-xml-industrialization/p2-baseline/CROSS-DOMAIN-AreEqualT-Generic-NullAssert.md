# 跨域问题：`Assert.AreEqual<T>` 泛型实例化的 null 断言路径

> 建档 2026-09-28，源于 XML 线收尾时对 `system-xml-schema` 两条 `failed` 的反查。
> **归属：非 XML 域** —— 属 `AreEqual<T>` 泛型 lowering + 反射成员名辅助。
> 本文件只做证据交接，不在 JSON/XML 线内修复。

---

## 1. 触发样本（最小可复现）

`System.Private.Xml / system-xml-schema` chunk：

| si | subject | 断言 | 结果 |
|:--:|:--------|:-----|:-----|
| 94 | `XmlSchemaTypeTests::GetBuiltInSimpleType_1_XmlTypeCode_0` | `Assert.AreEqual(default(XmlSchemaSimpleType)!, result);` | **failed** `caught=true, assertFailed=false` |
| 95 | `XmlSchemaTypeTests::GetBuiltInComplexType_2_XmlTypeCode_0` | `Assert.AreEqual(default(XmlSchemaComplexType)!, result);` | **failed** 同上 |
| 93 | `GetBuiltInSimpleType_0_XmlQualifiedName_0` | ATG 标 `// AOT-STUB-GAP`，降级 smoke | passed（stubGap） |
| 97 | `IsDerivedFrom_4_XmlSchemaType_...` | 返回 bool，无泛型 AreEqual | **real / passed** |

**对照组（同 chunk、同返回类型，仅参数类型不同）94/95 vs 93/96** —— 说明不是 XML stub 差异。

---

## 2. 已排除：「native stub 语义错误」

.NET 8 实测（`tmp_probe4`）：

```
default(XmlTypeCode) = 0 (None)
GetBuiltInSimpleType(default) => OK: <null>
GetBuiltInComplexType(default) => OK: <null>
GetBuiltInSimpleType(None)    => OK: <null>
```

→ 正确行为是**返回 null、不抛**。native 侧该静态方法落 catch-all 返回 `0`（= null），**行为已正确**，不是缺陷点。

---

## 3. 反查到的真实故障点（一手，来自生成体）

`assets: artifacts/foundation-dll/System.Private.Xml/chunks/system-xml-schema/native/codegen/generated/native-aot.generated.cpp`

`AreEqual<XmlSchemaSimpleType>` 的泛型实现体里，**构造断言失败消息**的路径：

```cpp
_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        chaos_mt_Chaos_TestFramework_Sdk_System_Xml_Schema_XmlSchemaSimpleType.AsTypeInfoHot());
const auto chaos_arg_0 = _s3;
if (chaos_arg_0 == 0) { raise_null_reference_exception(); }
const auto chaos_result = ChaosReflectionGetMemberName(chaos_arg_0);   // ← 传入裸 TypeInfoHot*
_s3 = chaos_result;
chaos_default_interpolated_string_handler_append_string(chaos_arg_0 = _s2, chaos_arg_1 = _s3);
```

### 关键点

1. **入参是裸 `TypeInfoHot*`**（`chaos_mt_*.AsTypeInfoHot()`），**不是** tagged `ReflectionQuery*` handle。
2. `ChaosReflectionGetMemberName`（`src/native/runtime-core/reflection/type_properties.cpp:68-104`）：
   - 依次尝试 `TryDecodeReflectionQueryHandle<Type/Method/FieldDescriptor>` —— 这些要求 **bit63 tag**，裸指针无 tag → 全部 miss
   - 退化到 `ResolveTypeFromReflectionOrGcHandle(member_handle)`，对裸 MethodTable 指针**同样可能 miss**
   - 两条都 miss → **`return 0`**
3. 返回 0 后进入 `chaos_default_interpolated_string_handler_append_string(handler, 0)` —— 把 `0` 当字符串对象/string_id 传递。

### 与既有已知缺陷同族

- 本仓既有记录 **「ABI is_string_id 桩 → 244 个 AV 单一根因」**：`_abi_is_string_id` 恒 false，插值拼接把非 string 当裸指针解引用。
- 本次是**同一类形状**的另一触发点：`ChaosReflectionGetMemberName` 对**裸 TypeInfoHot\*** 无法解码 → 0 → 进入同一拼接路径。

---

## 4. 影响面（跨域实测，非 XML 特有）

同形 `resultKind=failed && caught=true` 计数：

| chunk | 条数 |
|:------|----:|
| `system` | 217 |
| `xml` | 89–90 |
| `reflection` | 19 |
| `text` | 4 |
| `text-json` | 3 |
| `system-xml-schema` | 2 |

> 注：该计数**不等于**全部是本缺陷 —— `failed` 还包含其它根因。需按「是否走了 `AreEqual<T>` 失败消息路径」再筛。
> 但 94/95 已确认是**该路径**的首个坐实样本。

---

## 5. 待办（交接给对应域）

1. **确认** `ResolveTypeFromReflectionOrGcHandle` 对裸 `TypeInfoHot*`（MethodTable）的解析能力 —— 若本就支持，则问题在别处（如 `desc->name_utf8` 为空）。
2. **判定修法方向**（二选一）：
   - (a) 让 `ChaosReflectionGetMemberName` 接受裸 `TypeInfoHot*`：经 `ChaosFindReflectionTypeByTypeInfo()`（`reflection/type_resolve.cpp:31`）反查 descriptor 取 `name_utf8`。**该反查已存在**，是最小改动。
   - (b) 让 codegen 在失败消息路径上传 tagged handle 而非裸指针（改动面更大，涉及泛型实例化发射）。
3. **回归判据**：`AreEqual<T>` 在 T 为引用类型且两参数均为 null 时，**不应进入失败消息构造**（null==null 应判相等）。

   ### 首因已初步定位（2026-09-28 追加）

   从生成体的 `pc` 状态机反查，**装箱把 null 变成了非 null**，导致 null 分支被跳过：

   ```cpp
   // case 0: 装箱 chaos_args[0]（expected = null）
   auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_..._XmlSchemaSimpleType, {});
   chaos_boxed->header.type_info = chaos_mt_..._XmlSchemaSimpleType.AsTypeInfoHot();
   chaos_boxed->value = chaos_value;                  // value = 0（原始的 null）
   _s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
   if (_s0) { chaos_pc = 2; } else { chaos_pc = 1; }   // ← 箱对象地址非 0，恒走 pc=2
   ```

   **箱对象指针恒非 0**，所以"参数是否为 null"的判定在装箱之后做，永远得到"非 null"。
   对应 `case 1`（null 分支）与 `case 2`（非 null 分支）的产物：

   ```cpp
   case 1: { _s0 = ...; chaos_pc = 3; }                   // null 分支
   case 2: { _s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0); chaos_pc = 3; }  // 非 null → 置 0
   ```

   → 非 null 分支把比较结果置 **0（不相等）** → 走失败消息构造 → 撞上 §3 的 `ChaosReflectionGetMemberName` 返 0。

   **修法方向收敛为**：null 判定必须在**装箱之前**做（对原始 `chaos_fn_arg_*` 判 0），
   或对 `AreEqual<T>`（T 为引用类型）跳过装箱直接比较。

   > 注：这是**从生成体反推**的结论，未做插桩实证。接手者应以插桩（在 `case 1/2` 打印 `_s0/_s1`）坐实后再改。

---

## 6. 交接材料

- 生成体：`artifacts/.../system-xml-schema/native/codegen/generated/native-aot.generated.cpp`
  - `AreEqual<XmlSchemaSimpleType>` 泛型定义体（搜 `..._generic(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0`）
- fact 样本：`artifacts/.../system-xml-schema/results/fact-results.json` si=94/95
- .NET 8 探针：`tmp_probe4/Program.cs`（`GetBuiltInSimpleType(default)` → null）
- 同族既有记录：memory `abi-is-string-id-stub-causes-av.md`
