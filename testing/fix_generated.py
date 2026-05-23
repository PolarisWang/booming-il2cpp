import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

changes = 0

# Fix 1: Replace []() lambdas that capture locals with [&]()
# These appear in multiple functions and reference chaos_eval_stack, chaos_stack_top, _s1, etc.
# The pattern is [](){ ... chaos_eval_stack ... } or [](){ ... _s1 ... }
# Replace []() with [&]() only when followed by { and captures local vars

# Fix: replace all []() that appear to be jit invocation lambdas
# The pattern is [](){ with the body referencing local variables
# Safer approach: replace [](){ with [&]() { in jit context
# But we need to be careful not to break real stateless lambdas

# Count []() occurrences
count_empty = data.count(b'[]()')
print(f'[]() occurrences: {count_empty}')

# Check the specific lambdas that error:
# 1. Line 1615: [](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack...
# 2. Line 2366: [](){ ... _s1 ...

# Fix specific instances by context
# Lambda 1: chaos_eval_stack reference
old1 = b'[](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);'
if old1 in data:
    data = data.replace(old1, b'[&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);')
    changes += 1
    print('Fix 1a applied: lambda with chaos_eval_stack')

# Lambda 2: _s1 reference (in Program_Main)
old2 = b'[](){ return static_cast<CHAOS_IL2CPP_INTPTR>(_s1); }()'
if old2 in data:
    data = data.replace(old2, b'[&](){ return static_cast<CHAOS_IL2CPP_INTPTR>(_s1); }()')
    changes += 1
    print('Fix 1b applied: lambda with _s1')

# Check if there are other problematic []() lambdas
# These would be []() lambdas that reference local variables
# Let's find all []() lambdas and check if they reference _s or chaos_ variables
import re
for m in re.finditer(b'\\[\\]\\(\\)', data):
    offset = m.start()
    # Look at what follows: should be { ... }
    # Check if it references local vars
    chunk = data[offset:offset+200]
    if b'chaos_' in chunk or b'_s' in chunk:
        print(f'INFO: Potential capture issue at byte {offset}: {repr(chunk[:80])}')

# Fix 2: Replace the EH block in Program_Main with direct try/catch
# Find the unique CHAOS_EH block
if b'CHAOS_EH_TRY' in data:
    old_eh = (
        b'\tCHAOS_EH_TRY'
        b'\r\n\t\t\t_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);'
    )
    if old_eh in data:
        new_eh_try = (
            b'\ttry {'
            b'\r\n\t\t\t_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);'
        )
        data = data.replace(old_eh, new_eh_try, 1)

        # Replace CATCH_BEGIN
        data = data.replace(b'CHAOS_EH_CATCH_BEGIN', b'} catch (const chaos_managed_exception& e) {', 1)

        # Replace EH_END
        data = data.replace(b'CHAOS_EH_END', b'}', 1)

        # Replace EXCEPTION_OBJ
        data = data.replace(b'CHAOS_EH_EXCEPTION_OBJ', b'e.object_value')

        # Replace RETHROW
        data = data.replace(b'CHAOS_EH_RETHROW', b'throw')

        changes += 1
        print('Fix 2 applied: EH macros -> direct try/catch')

# Fix 3: Add return 0 to RunNativeAot
idx = data.find(b'RunNativeAot')
if idx >= 0:
    # Find: kBenchmarkWrappers[chaos_entry_index]();
    wrapper_line = data.find(b'kBenchmarkWrappers[chaos_entry_index]();', idx)
    if wrapper_line >= 0:
        after_wrapper = wrapper_line + len(b'kBenchmarkWrappers[chaos_entry_index]();')
        # Find: \t}\n} (else closing + function closing)
        close1 = data.find(b'\t}\r\n}', after_wrapper)
        if close1 >= 0:
            data = data[:close1] + b'\t}\r\n\treturn 0;\r\n}' + data[close1+5:]
            changes += 1
            print('Fix 3 applied: return 0 in RunNativeAot')

# Fix 4: Fix the wrong indentation in the catch handler
# The RTS_EXC_HANDLED block is inside the catch handler at wrong indent
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
    changes += 1
    print('Fix 4 applied: fixed RTS_EXC_HANDLED/RTS_DONE indentation')
else:
    print(f'Fix 4: old section count = {count} (not 1, skipping)')

with open(sys.argv[1], 'wb') as f:
    f.write(data)

print(f'\nTotal changes: {changes}')
print(f'New file size: {len(data)}')
