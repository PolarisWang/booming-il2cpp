import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

# The DBG lines have literal newlines instead of \n in string literals
# Fix by finding and fixing the broken string

# Fix 1: catch handler line
old = b'DBG: catch handler, obj=%lld\n'
new = b'DBG: catch handler, obj=%lld\\n'
count = data.count(old)
print(f'Fix 1: found {count}')
data = data.replace(old, new)

# Fix 2: Only apply the after-EH line if it's also broken
old2 = b'DBG: after CHAOS_EH_END\n'
new2 = b'DBG: after CHAOS_EH_END\\n'
count2 = data.count(old2)
print(f'Fix 2: found {count2}')
data = data.replace(old2, new2)

with open(sys.argv[1], 'wb') as f:
    f.write(data)
print('Done')
