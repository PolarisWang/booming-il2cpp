# AutoTestGenerator: System.Xml.XmlTextReader

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlTextReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 27 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 26 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 64 |

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
