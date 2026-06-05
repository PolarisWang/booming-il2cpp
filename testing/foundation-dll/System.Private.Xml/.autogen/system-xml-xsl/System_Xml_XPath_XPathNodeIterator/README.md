# AutoTestGenerator: System.Xml.XPath.XPathNodeIterator

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XPath.XPathNodeIterator`

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

- `MoveNext (enumerator leak - target has nested Enumerator type)` — contains generic parameters, needs manual handling
