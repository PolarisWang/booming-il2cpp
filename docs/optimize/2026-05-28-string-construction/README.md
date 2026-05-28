# 优化：string-construction

## 优化对象
- family: `string-construction`
- 涉及方法: 8 个 System.String 构造/操作方法

## 验证结果

Pipeline 15/15 全部 passed，无需代码修改。

| 方法 | chaos-aot (ns) | chaos-jit (ns) | .NET 8 (ns) | vs .NET 8 |
|------|---------------|---------------|-------------|-----------|
| .ctor(char[]) | ~16ns | ~51ns | ~3507ns | -99.5% |
| .ctor(char,int) | ~14ns | ~1387ns | ~3602ns | -99.6% |
| .ctor(char[],int,int) | ~17ns | ~45ns | ~4479ns | -99.6% |
| Copy(string) | ~4ns | ~32ns | ~50ns | -93% |
| Concat(string,string) | ~4ns | ~49ns | ~50ns | -93% |
| Intern(string) | ~5ns | ~32ns | ~33ns | -86% |
| IsInterned(string) | ~4ns | ~33ns | ~87ns | -95% |

## HotUpdate 开销

| 路径 | 方法数 | 状态 |
|------|--------|------|
| AOT benchmark | 8/8 | passed |
| JIT benchmark | 8/8 | passed |

## 收敛检查

- [x] Step 2-4: Pipeline 全部 passed (15/15)
- [x] Step 5: benchmark timing > 0
- [x] Step 6: vs .NET 8 ≤ 20%
- [x] Step 7: hotupdate benchmark passed
