import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

# Find RunNativeAot function
idx = data.find(b'RunNativeAot')

# Find the wrapper call line
wrapper_line = data.find(b'kBenchmarkWrappers[chaos_entry_index]();', idx)
after_wrapper = wrapper_line + len(b'kBenchmarkWrappers[chaos_entry_index]();')

# After wrapper line: \t}\r\n}\r\n
# Need to insert return 0 between them
idx_close1 = data.find(b'\t}\r\n}', after_wrapper)
if idx_close1 >= 0:
    new_data = data[:idx_close1] + b'\t}\r\n\treturn 0;\r\n}' + data[idx_close1+5:]
    with open(sys.argv[1], 'wb') as f:
        f.write(new_data)
    print('Fixed: added return 0')
else:
    print('Pattern not found')
    print(f'Context: {repr(data[after_wrapper:after_wrapper+40])}')
