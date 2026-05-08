# AI Code Review: type-runtime-handles
## Type: hotupdate
## Date: 2026-05-08T11:46:50.389891+00:00
## Verdict: FAIL

## Summary
- Total issues: 4
- C++ LOC: 92
- C# methods: 25
- C++ functions: 13

## Issues Found
1. No sentinel return values found in patch code
2. No fallthrough/delegate pattern found — patch may not chain to original
3. No matching C++ function found for C# method 'n'
4. No matching C++ function found for C# method 'Run'

## Check Results
- patch_correctness: FAIL
- signature_alignment: FAIL