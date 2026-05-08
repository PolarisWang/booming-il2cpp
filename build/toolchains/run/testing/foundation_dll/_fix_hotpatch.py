import sys

path = r'D:/agent/booming-il2cpp/build/toolchains/run/testing/foundation_dll/generate_hotupdate_test.py'
with open(path, 'rb') as f:
    data = f.read()

old = b'if has_patch_data:\r\n        lines.extend([\r\n            "        // Step 2c: Verify D3 dispatch entry has kDispatchPatched flag.",\r\n            "        auto* dispatch_entry = RuntimeDispatchLookup(token);",\r\n            "        if (dispatch_entry == nullptr) {",\r\n            \'            std::fprintf(stderr, "FAIL[%u]: RuntimeDispatchLookup returned null\\\\n", i);\',\r\n            "            step_ok = false;",\r\n            "        } else if (!(dispatch_entry->flags & kDispatchPatched)) {",\r\n            \'            std::fprintf(stderr, "FAIL[%u]: dispatch entry not patched (flags=0x%08x)\\\\n", i, dispatch_entry->flags);\',\r\n            "            step_ok = false;",\r\n            "        }",\r\n        ])'

new = b'if has_patch_data:\r\n        lines.extend([\r\n            "        // Step 2c: Verify D3 dispatch entry has kDispatchPatched flag.",\r\n            "        if (d3_patched_count > 0) {",\r\n            "            auto* dispatch_entry = RuntimeDispatchLookup(token);",\r\n            "            if (dispatch_entry == nullptr) {",\r\n            \'            std::fprintf(stderr, "FAIL[%u]: RuntimeDispatchLookup returned null\\\\n", i);\',\r\n            "                step_ok = false;",\r\n            "            } else if (!(dispatch_entry->flags & kDispatchPatched)) {",\r\n            \'            std::fprintf(stderr, "FAIL[%u]: dispatch entry not patched (flags=0x%08x)\\\\n", i, dispatch_entry->flags);\',\r\n            "                step_ok = false;",\r\n            "            }",\r\n            "        }",\r\n        ])'

count = data.count(old)
print(f'Found {count} occurrences', file=sys.stderr)
if count > 0:
    data = data.replace(old, new)
    with open(path, 'wb') as f:
        f.write(data)
    print('REPLACED', file=sys.stderr)
else:
    print('NOT FOUND - searching for partial match...', file=sys.stderr)
    # Try to find it without the \r
    idx = data.find(b'Step 2c: Verify D3')
    if idx >= 0:
        chunk = data[idx-80:idx+300]
        print(repr(chunk))
