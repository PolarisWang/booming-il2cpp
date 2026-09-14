"""Reconcile the claim trace against the coverage shortfall.

The instrumented run logged 128 claims for a 4096 range, of which 64 were
in-range and 64 were past the end.  Each claim advances next_index by 32
regardless of whether it lands inside the range, so the overshooting claims
consume indices that no worker will ever revisit.
"""
import re
import sys

CLAIM = re.compile(r'claim \[(\d+),(\d+)\)')
END = 4096
CHUNK = 32

claims = []
for line in open(sys.argv[1], errors='ignore'):
    m = CLAIM.search(line)
    if m:
        claims.append((int(m.group(1)), int(m.group(2))))

print(f'total claims logged      : {len(claims)}')
in_range = [(a, b) for a, b in claims if a < END]
print(f'claims inside the range  : {len(in_range)}')

# Every claim (in range or not) advanced next_index by CHUNK.  The final value
# of next_index is therefore n_claims * CHUNK; the index space actually
# covered by workers is [0, min(next_index, END)).
final_next = max(a for a, _ in claims) + CHUNK
print(f'final next_index         : {final_next}')
print(f'chunks worth of advance  : {final_next // CHUNK} x {CHUNK}')

# How many indices sit inside a *claimed in-range* span?
covered = set()
for a, b in in_range:
    covered.update(range(a, min(b, END)))
print(f'indices in a claimed span: {len(covered)} / {END}')

# The counts the program actually observed, for comparison.
print()
print('Observed body counts from the runs:')
print('  2310, 2286, 2207, 2248, 2197, 2454, 2365, 2444, 2299  <-- all < 4096')
print()
print('NOTE: if the claims are all in range and cover everything, but the body')
print('runs fewer times, the loss is NOT in the claim arithmetic.  Compare the')
print('claim spans above against the indices the body recorded.')
