# AutoTestGenerator: System.IO.Path

- Assembly: `System.Private.CoreLib`
- Type: `System.IO.Path`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 45 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 18 |
| Benchmark-only (pure void / non-deterministic) | 10 |
| Skipped (generic / unresolvable) | 17 |
| Total Subjects | 62 |

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

- `GetDirectoryName (ref struct)` — contains generic parameters, needs manual handling
- `GetExtension (ref struct)` — contains generic parameters, needs manual handling
- `GetFileName (ref struct)` — contains generic parameters, needs manual handling
- `GetFileNameWithoutExtension (ref struct)` — contains generic parameters, needs manual handling
- `IsPathFullyQualified (ref struct)` — contains generic parameters, needs manual handling
- `HasExtension (ref struct)` — contains generic parameters, needs manual handling
- `Combine (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `TryJoin (ref struct)` — contains generic parameters, needs manual handling
- `TryJoin (ref struct)` — contains generic parameters, needs manual handling
- `TrimEndingDirectorySeparator (ref struct)` — contains generic parameters, needs manual handling
- `EndsInDirectorySeparator (ref struct)` — contains generic parameters, needs manual handling
- `IsPathRooted (ref struct)` — contains generic parameters, needs manual handling
- `GetPathRoot (ref struct)` — contains generic parameters, needs manual handling
