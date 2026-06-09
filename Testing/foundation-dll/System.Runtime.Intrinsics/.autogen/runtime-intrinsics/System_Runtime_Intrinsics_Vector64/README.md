# AutoTestGenerator: System.Runtime.Intrinsics.Vector64

- Assembly: `System.Private.CoreLib`
- Type: `System.Runtime.Intrinsics.Vector64`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 239 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 184 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 53 |
| Total Subjects | 258 |

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

- `Abs` — contains generic parameters, needs manual handling
- `Clamp` — contains generic parameters, needs manual handling
- `CopyTo (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Dot` — contains generic parameters, needs manual handling
- `Lerp` — contains generic parameters, needs manual handling
- `Lerp` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Log2` — contains generic parameters, needs manual handling
- `Log2` — contains generic parameters, needs manual handling
- `Max` — contains generic parameters, needs manual handling
- `Min` — contains generic parameters, needs manual handling
- `MultiplyAddEstimate` — contains generic parameters, needs manual handling
- `MultiplyAddEstimate` — contains generic parameters, needs manual handling
- `Sum` — contains generic parameters, needs manual handling
- `TryCopyTo (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenLower` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
- `WidenUpper` — contains generic parameters, needs manual handling
