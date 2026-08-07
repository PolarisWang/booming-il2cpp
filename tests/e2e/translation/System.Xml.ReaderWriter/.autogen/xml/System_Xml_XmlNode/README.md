# AutoTestGenerator: System.Xml.XmlNode

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlNode`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 21 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 20 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 29 |

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

- `Normalize` — contains generic parameters, needs manual handling
