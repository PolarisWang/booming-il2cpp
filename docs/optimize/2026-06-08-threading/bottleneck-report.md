# Bottleneck Analysis: threading

## Summary
- Benchmark methods: 602
- Total duration: 269.7ms
- Mean duration: 0.448ms
- Max duration: 50.343ms
- Total alloc: 19,000,000 B

## Top 10 slowest methods
- entryIndex=?: mean=50.343ms max=50.839ms cv=0.0084
- entryIndex=?: mean=49.541ms max=50.122ms cv=0.0127
- entryIndex=?: mean=49.251ms max=49.649ms cv=0.0069
- entryIndex=?: mean=48.903ms max=49.409ms cv=0.0079
- entryIndex=?: mean=1.073ms max=1.076ms cv=0.0023
- entryIndex=?: mean=1.071ms max=1.072ms cv=0.0013
- entryIndex=?: mean=0.607ms max=0.615ms cv=0.0097
- entryIndex=?: mean=0.380ms max=0.551ms cv=0.3192
- entryIndex=?: mean=0.357ms max=0.441ms cv=0.1673
- entryIndex=?: mean=0.358ms max=0.439ms cv=0.1608

## ROI estimate
threading: 602 bench methods, 269.7ms total, 19MB alloc.
Top 4 methods take ~50ms each (74% of total time).
Optimizing these 4 could reduce total time by ~135ms.

**Expected gain**: ~50%
**Complexity**: Medium
**Scope**: High (threading primitives benefit other chunks)

**Decision**: PASS (expected gain > 20%)
