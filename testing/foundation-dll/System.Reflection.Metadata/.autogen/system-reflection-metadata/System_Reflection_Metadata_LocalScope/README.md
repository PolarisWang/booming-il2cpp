# AutoTestGenerator: System.Reflection.Metadata.LocalScope

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.LocalScope`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 2 |

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

- `GetChildren (ref struct return type: LocalScopeHandleCollection.ChildrenEnumerator)` — contains generic parameters, needs manual handling
