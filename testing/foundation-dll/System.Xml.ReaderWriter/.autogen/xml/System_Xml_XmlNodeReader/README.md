# AutoTestGenerator: System.Xml.XmlNodeReader

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlNodeReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 20 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 19 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 45 |

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
