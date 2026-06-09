# AutoTestGenerator: System.Xml.XmlResolver

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlResolver`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 4 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 4 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 7 |

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
