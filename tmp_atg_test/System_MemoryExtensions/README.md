# AutoTestGenerator: System.MemoryExtensions

- Assembly: `System.Private.CoreLib`
- Type: `System.MemoryExtensions`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 263 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 25 |
| Skipped (generic / unresolvable) | 238 |
| Total Subjects | 74 |

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

- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Contains (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Contains (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Contains (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExceptInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAny (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ContainsAnyExceptInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExceptInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAnyExceptInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExceptInRange (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAnyExceptInRange (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceEqual (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceCompareTo (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `IndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `LastIndexOfAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceEqual (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceEqual (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceEqual (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceCompareTo (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SequenceCompareTo (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Reverse (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `AsSpan (ref struct return type: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `CopyTo (ICollection non-generic leak: T[])` — contains generic parameters, needs manual handling
- `CopyTo (ICollection non-generic leak: T[])` — contains generic parameters, needs manual handling
- `Overlaps (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Overlaps (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Overlaps (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Overlaps (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `BinarySearch (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Sort (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Replace (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Replace (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Replace (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Replace (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ReplaceAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ReplaceAny (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `ReplaceAnyExcept (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `ReplaceAnyExcept (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `CommonPrefixLength (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `CommonPrefixLength (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `CommonPrefixLength (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `CommonPrefixLength (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Split (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Split (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SplitAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `SplitAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Split (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Split (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `SplitAny (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `SplitAny (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Count (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `CountAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `CountAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `CountAny (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryWrite (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `IsWhiteSpace (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Contains (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `CompareTo (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `IndexOf (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `LastIndexOf (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ToLower (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ToLowerInvariant (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ToUpper (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ToUpperInvariant (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `EndsWith (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `StartsWith (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `EnumerateRunes (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `EnumerateRunes (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `EnumerateLines (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `EnumerateLines (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `Trim (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `TrimStart (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `TrimEnd (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `Trim (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TrimStart (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TrimEnd (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
