import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()

# Find CHAOS_EH_TRY block
start = data.find(b'CHAOS_EH_TRY')
if start < 0:
    print("CHAOS_EH_TRY not found")
    sys.exit(1)

# Find the end - CHAOS_EH_END + \n
end_marker = b'CHAOS_EH_END'
end = data.find(end_marker, start)
if end < 0:
    print("CHAOS_EH_END not found")
    sys.exit(1)

# Include everything after CHAOS_EH_END up to the \n after it
end_of_line = data.find(b'\n', end)
end = end_of_line + 1

print(f"Block from {start} to {end}")
original_block = data[start:end]
# Show first and last 100 bytes
print(f"FIRST: {repr(original_block[:100])}")
print(f"LAST:  {repr(original_block[-100:])}")

# Build replacement: direct try/catch
# Preserve exact indentation by deriving from original
indent_try = original_block[:len(b'CHAOS_EH_TRY')].replace(b'CHAOS_EH_TRY', b'\ttry { // direct')

# Actually, let's read the original indentation first
lines = original_block.split(b'\n')
print(f"Block has {len(lines)} lines")
print(f"Line 0: {repr(lines[0])}")
print(f"Line 1: {repr(lines[1])}")

# Get the indentation before CHAOS_EH_TRY
pre = data[:start]
last_newline = pre.rfind(b'\n')
base_indent = pre[last_newline+1:]  # spaces/tabs before CHAOS_EH_TRY
print(f"Base indent before CHAOS_EH_TRY: {repr(base_indent)}")

# The indent inside the try body would be base_indent + \t
inner_indent = base_indent + b'\t'

# Build the replacement
# Replace CHAOS_EH_TRY with try {
# Replace CHAOS_EH_CATCH_BEGIN... with } catch (const chaos_managed_exception& e) {
# Replace CHAOS_EH_END with }

# Find the CHAOS_EH_CATCH_BEGIN line
catch_begin_idx = original_block.find(b'CHAOS_EH_CATCH_BEGIN')
if catch_begin_idx >= 0:
    print(f"CHAOS_EH_CATCH_BEGIN at offset {catch_begin_idx} in block")
    # Get indentation before CHAOS_EH_CATCH_BEGIN
    before_catch = original_block[:catch_begin_idx]
    last_nl = before_catch.rfind(b'\n')
    catch_indent = before_catch[last_nl+1:] if last_nl >= 0 else b''
    print(f"Catch indent: {repr(catch_indent)}")

    # Replace CHAOS_EH_CATCH_BEGIN line with catch line
    # The CHAOS_EH_CATCH_BEGIN is on its own line, so the replacement looks like:
    # base_indent} catch (const chaos_managed_exception& e) {
    catch_line_replacement = catch_indent + b'} catch (const chaos_managed_exception& e) {'

    # Replace CHAOS_EH_END with }
    # The CHAOS_EH_END is also on its own line
    end_indent = b'\t\t'  # from the end of the block
    end_line_replacement = end_indent + b'}'

    # Build new block
    new_block = original_block

    # Replace CHAOS_EH_TRY
    try_indent_orig = original_block[:original_block.find(b'\n')+1]
    # Actually, CHAOS_EH_TRY starts at position 0 in the block, the whitespace before it is from the original file
    # CHAOS_EH_TRY is at position 0 in the block
    # The first line is just CHAOS_EH_TRY. Replace it with "try {"
    first_line_end = original_block.find(b'\n')
    try_replacement = base_indent + b'try {'
    print(f"try replacement: {repr(try_replacement)}")

    # Now let's build it step by step
    # The block structure is:
    # [base_indent]CHAOS_EH_TRY
    # [inner_indent]_s0 = ...
    # ...
    # [catch_indent]CHAOS_EH_CATCH_BEGIN
    # [inner_indent]if ...
    # ...
    # [end_indent]CHAOS_EH_END
    #
    # Replace with:
    # [base_indent]try {
    # [inner_indent]_s0 = ...  (same body)
    # ...
    # [catch_indent]} catch (const chaos_managed_exception& e) {
    # [inner_indent]if ... (same handler)
    # ...
    # [end_indent]}

    # So just:
    # 1. CHAOS_EH_TRY -> try {
    # 2. CHAOS_EH_CATCH_BEGIN -> } catch (const chaos_managed_exception& e) {
    # 3. CHAOS_EH_END -> }

    new_block = new_block.replace(b'CHAOS_EH_TRY', b'try {', 1)
    new_block = new_block.replace(b'CHAOS_EH_CATCH_BEGIN', b'} catch (const chaos_managed_exception& e) {', 1)
    new_block = new_block.replace(b'CHAOS_EH_END', b'}', 1)

    # Also replace CHAOS_EH_EXCEPTION_OBJ with e.object_value
    new_block = new_block.replace(b'CHAOS_EH_EXCEPTION_OBJ', b'e.object_value')

    # Also replace CHAOS_EH_RETHROW with throw
    new_block = new_block.replace(b'CHAOS_EH_RETHROW', b'throw')

    new_data = data[:start] + new_block + data[end:]

    with open(sys.argv[1], 'wb') as f:
        f.write(new_data)
    print("Replacement done!")

    # Verify
    with open(sys.argv[1], 'rb') as f:
        verify = f.read()
    v_start = verify.find(b'try {')
    if v_start >= 0:
        print(f"VERIFIED try at {v_start}")
        print(repr(verify[v_start:v_start+200]))
else:
    print("CHAOS_EH_CATCH_BEGIN not found in block")
