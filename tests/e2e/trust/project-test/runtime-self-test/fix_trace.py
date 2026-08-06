with open('D:/agent/booming-il2cpp/testing/project-test/runtime-self-test/codegen/RuntimeSelfTest/generated/native-aot.generated.cpp', 'rb') as f:
    content = f.read()

# File uses CRLF (\r\n) line endings
old = b'\tfprintf(stderr, "TRACE b0: loop start\r\n");'
# Replace with proper \n escape: \t + fprintf(... "TRACE b0: loop start\n") + ; + CRLF
new = b'\tfprintf(stderr, "TRACE b0: loop start\\n");\r\n'

print(f"Found: {old in content}")

if old in content:
    content = content.replace(old, new)
    with open('D:/agent/booming-il2cpp/testing/project-test/runtime-self-test/codegen/RuntimeSelfTest/generated/native-aot.generated.cpp', 'wb') as f:
        f.write(content)
    print("Fixed!")

# Verify
with open('D:/agent/booming-il2cpp/testing/project-test/runtime-self-test/codegen/RuntimeSelfTest/generated/native-aot.generated.cpp', 'rb') as f:
    lines = f.readlines()
    for i in range(3255, 3260):
        if i < len(lines):
            print(f'{i+1}: {lines[i]!r}')
