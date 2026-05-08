# AI Code Review: io-streams-basics
## Type: hotupdate
## Date: 2026-05-08T11:46:15.435867+00:00
## Verdict: FAIL

## Summary
- Total issues: 4
- C++ LOC: 122
- C# methods: 30
- C++ functions: 18

## Issues Found
1. No sentinel return values found in patch code
2. No fallthrough/delegate pattern found — patch may not chain to original
3. No matching C++ function found for C# method 'Run'
4. No matching C++ function found for C# method 'n'

## Check Results
- patch_correctness: FAIL
- signature_alignment: FAIL