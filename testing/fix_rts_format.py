import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

old_section = (
    b'\t\t_s0 = e.object_value;'
    b'\r\n\t\t\t{{'
    b'\r\n\t\t\t\t_s0 = CHAOS_IL2CPP_STRING_ID("RTS_EXC_HANDLED=-1");'
    b'\r\n\t\t\t}}'
    b'\r\n\t\t\t{'
    b'\r\n\t\t\t\tconst auto chaos_arg_0 = _s0;'
    b'\r\n\t\t\t\tchaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);'
    b'\r\n\t\t\t}'
    b'\r\n\t}'
    b'\r\n\t\t}'
    b'\r\n\t\t{{'
    b'\r\n\t\t\t_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DONE=ok");'
    b'\r\n\t\t}}'
    b'\r\n\t{'
    b'\r\n\t\tconst auto chaos_arg_0 = _s0;'
    b'\r\n\t\tchaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);'
    b'\r\n\t}'
    b'\r\n\treturn;'
)

count = data.count(old_section)
print(f'Old section count: {count}')

if count == 1:
    new_section = (
        b'\t\t_s0 = e.object_value;'
        b'\r\n\t}'
        b'\r\n\t{{'
        b'\r\n\t\t_s0 = CHAOS_IL2CPP_STRING_ID("RTS_EXC_HANDLED=-1");'
        b'\r\n\t}}'
        b'\r\n\t{'
        b'\r\n\t\tconst auto chaos_arg_0 = _s0;'
        b'\r\n\t\tchaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);'
        b'\r\n\t}'
        b'\r\n\t{{'
        b'\r\n\t\t_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DONE=ok");'
        b'\r\n\t}}'
        b'\r\n\t{'
        b'\r\n\t\tconst auto chaos_arg_0 = _s0;'
        b'\r\n\t\tchaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);'
        b'\r\n\t}'
        b'\r\n\treturn;'
    )

    data = data.replace(old_section, new_section, 1)
    with open(sys.argv[1], 'wb') as f:
        f.write(data)
    print('Fixed!')
else:
    print(f'Unexpected count: {count}')
    idx = data.find(b'_s0 = e.object_value;')
    if idx >= 0:
        print(f'Found _s0 = e.object_value; at {idx}')
        print(repr(data[idx:idx+600]))
