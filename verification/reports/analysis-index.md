| 日期 | Family | 评分 | 结论 | 摘要 |
|------|--------|------|------|------|
| 2026-05-10 | convert-char | 5.0/5 | approved | 18/18 fact, avg_speedup=75.34%, 11/11 native faster, BenchmarkMethod 消除 dispatch 开销, evidence 污染修复, managed test 补充, README 扩展 |
| 2026-05-10 | convert-char | 4.9/5 | approved | (v2) 18/18 fact, avg_speedup=76.11%, 11/11 native faster, principle CONCERN (waived), 验证结果稳定 |
| 2026-05-11 | convert-char | 4.1/5 | review-needed | 18/18 fact 100%, avg_speedup=75.29%, 11/11 native faster, false_passing=0, but review/managed_test/handwrite 缺失, dashboard hotupdate schema 显示问题 |
| 2026-05-11 | primitive-numeric-conversions | 4.5/5 -> 5.0/5 | approved | strict 15/15 fact, avg_speedup=67.57%, 15/15 native faster, hotupdate 15/15, principle ALIGNED, review+测试已补全, contract obligations 已启用, DIAG 已清理 |
| 2026-05-11 | array-indexing-copy | 3.5/5 | blocked | strict 10/13 fact (76.92%), pipeline overall=partial, 3 methods failed (Resize ref param, Sort/BinarySearch interface/Object), benchmark all invalid, principle CONCERN, managed_test missing |
| 2026-05-12 | array-indexing-copy | 3.0/5 | review-needed | standard 14/14 fact (100%), false_passing=0, principle CONCERN (2 runtime_stub calls), benchmark all managed_harness_unavailable, hotupdate 0/0 failed, no managed_test |