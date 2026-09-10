# AutoTestGenerator: System.Collections.Frozen.FrozenSet<System.Int32>

- Assembly: `System.Collections.Immutable`
- Type: `System.Collections.Frozen.FrozenSet<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 17 |

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

- `CopyTo (ICollection non-generic leak: Int32[])` — contains generic parameters, needs manual handling
- `CopyTo (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `GetEnumerator (ref struct return type: FrozenSet<System.Int32>.Enumerator)` — contains generic parameters, needs manual handling
- `GetAlternateLookup` — contains generic parameters, needs manual handling
- `TryGetAlternateLookup` — contains generic parameters, needs manual handling
