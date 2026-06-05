# AutoTestGenerator: System.Collections.Frozen.FrozenDictionary<System.Int32, System.Int32>

- Assembly: `System.Collections.Immutable`
- Type: `System.Collections.Frozen.FrozenDictionary<System.Int32, System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 3 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 4 |
| Total Subjects | 11 |

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

- `CopyTo (ref struct parameter: System.Span<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>)` — contains generic parameters, needs manual handling
- `GetEnumerator (ref struct return type: FrozenDictionary<System.Int32,System.Int32>.Enumerator)` — contains generic parameters, needs manual handling
- `GetAlternateLookup` — contains generic parameters, needs manual handling
- `TryGetAlternateLookup` — contains generic parameters, needs manual handling
