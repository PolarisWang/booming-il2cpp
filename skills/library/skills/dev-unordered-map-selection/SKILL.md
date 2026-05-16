# unordered_map 选型规则

编写 native C++ 代码时，按以下决策链选择宏：

**先用 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY`，降级按以下链判断：**

1. Key 类型是指针、整数、枚举、已哈希值（非 string）→ `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY`（`chaos::il2cpp::common::identity_hash` 跳过 wyhash）
2. Key 类型是 string，但无数据依赖需求 → `CHAOS_IL2CPP_UNORDERED_DENSE_MAP`（默认 wyhash）

**退回到 `CHAOS_IL2CPP_UNORDERED_MAP` (std) 的条件：**
1. 需要 iterator/pointer 在 insert/erase 后仍有效
2. 需要 heterogeneous lookup (`find("literal"sv)` on `map<string,T>`)
3. 代码属于 codegen 输出/热更新路径（保持语义一致）
4. value 类型 > 128 字节且 insert/erase 频繁
5. map 持有元素指针/引用在跨 insert 后访问

**经验法则**：查找表、注册表、缓存、id→ptr 映射、type→handler 映射，无脑用 `MAP_IDENTITY`。string key 的查找表用 `MAP`。依赖 std 链式桶语义时才回退。
