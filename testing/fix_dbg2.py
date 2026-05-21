import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

# Add a DBG print before ChaosDiv(5, 0) to confirm try block is entered
old = b'= static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv('
replacement = b'= static_cast<CHAOS_IL2CPP_INTPTR>((std::fprintf(stderr, "DBG: about to call ChaosDiv(5, %d)\\n", static_cast<int>(_s1)), ChaosDiv('
count = data.count(old)
print(f'Found {count}')
if count > 0:
    data = data.replace(old, replacement)
    with open(sys.argv[1], 'wb') as f:
        f.write(data)
    print('Applied')
else:
    idx = data.find(b'ChaosDiv')
    if idx >= 0:
        print(f'Context: {repr(data[idx-40:idx+100])}')
