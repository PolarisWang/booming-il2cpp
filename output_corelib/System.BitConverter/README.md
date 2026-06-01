# AutoTestGenerator: System.BitConverter

- Assembly: `System.Private.CoreLib`
- Type: `System.BitConverter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 64 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 25 |
| Benchmark-only (pure void / non-deterministic) | 13 |
| Skipped (generic / unresolvable) | 26 |
| Total Subjects | 84 |

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

- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `ToChar (ref struct)` — contains generic parameters, needs manual handling
- `ToInt16 (ref struct)` — contains generic parameters, needs manual handling
- `ToInt32 (ref struct)` — contains generic parameters, needs manual handling
- `ToInt64 (ref struct)` — contains generic parameters, needs manual handling
- `ToInt128 (ref struct)` — contains generic parameters, needs manual handling
- `ToUInt16 (ref struct)` — contains generic parameters, needs manual handling
- `ToUInt32 (ref struct)` — contains generic parameters, needs manual handling
- `ToUInt64 (ref struct)` — contains generic parameters, needs manual handling
- `ToUInt128 (ref struct)` — contains generic parameters, needs manual handling
- `ToHalf (ref struct)` — contains generic parameters, needs manual handling
- `ToSingle (ref struct)` — contains generic parameters, needs manual handling
- `ToDouble (ref struct)` — contains generic parameters, needs manual handling
- `ToBoolean (ref struct)` — contains generic parameters, needs manual handling
