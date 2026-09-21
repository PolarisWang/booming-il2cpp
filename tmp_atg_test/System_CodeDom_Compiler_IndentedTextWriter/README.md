# AutoTestGenerator: System.CodeDom.Compiler.IndentedTextWriter

- Assembly: `System.Private.CoreLib`
- Type: `System.CodeDom.Compiler.IndentedTextWriter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 48 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 31 |
| Benchmark-only (pure void / non-deterministic) | 15 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 101 |

## Build

```bash
# Build with verification (Assert enabled)
dotnet build -p:DefineConstants=VERIFY

# Build for benchmark (Assert eliminated)
dotnet build
```

## Run

```bash
# Run with verification
dotnet run --project .
```

## Skipped Methods

- `Write (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `WriteLine (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
