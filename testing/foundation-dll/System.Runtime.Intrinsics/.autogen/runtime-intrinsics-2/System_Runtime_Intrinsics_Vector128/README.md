# AutoTestGenerator: System.Runtime.Intrinsics.Vector128

- Assembly: `System.Private.CoreLib`
- Type: `System.Runtime.Intrinsics.Vector128`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 273 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 197 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 74 |
| Total Subjects | 271 |

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
- `AsVector2` — contains generic parameters, needs manual handling
- `AsVector3` — contains generic parameters, needs manual handling
- `AsVector4` — contains generic parameters, needs manual handling
- `AsVector (unboxable return type: System.Numerics.Vector`1[[System.Int32, System.Private.CoreLib, Version=9.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e]])` — contains generic parameters, needs manual handling
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
