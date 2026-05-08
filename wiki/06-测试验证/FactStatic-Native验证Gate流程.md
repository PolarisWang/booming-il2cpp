# Fact Static Native 验证 Gate 流程

## 概述

Fact Static (Semantic Correctness，原名 L2) 验证是 foundation DLL verification pipeline 中的 native gate。它将 IL2CPP 生成的 C++ AOT 代码编译为 native exe，通过 return value checksum + assert intrinsic 双机制验证语义正确性。

## 架构

```
managed entrypoint DLL  →  fact_verifier.py  →  expected_checksums.h
                                              →  static native exe
                                              →  checksum comparison → l2-results.json
```