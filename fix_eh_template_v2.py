"""Extract lambda body from RunFactJsonMode to a separate function.
Fixes MSVC C2712/C2713: __try/__except cannot coexist with C++ lambda in same function.
"""
import re

TPL = "src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban"

with open(TPL, "r", encoding="utf-8") as f:
    content = f.read()

# Step 1: Find the CreateThread lambda
# Pattern: HANDLE worker = CreateThread(... [](LPVOID) -> DWORD { ... }, nullptr, 0, nullptr);
start_marker = "HANDLE worker = CreateThread(nullptr, 4 * 1024 * 1024, [](LPVOID) -> DWORD {"
if start_marker not in content:
    print("ERROR: lambda not found")
    raise SystemExit(1)

# Find the end: ", nullptr, 0, nullptr);"
lambda_func_start = content.find(start_marker)
# Find the opening brace of the lambda body
brace_start = content.find("{", lambda_func_start)
# Balance braces to find the matching closing brace
depth = 1
pos = brace_start + 1
while pos < len(content) and depth > 0:
    if content[pos] == "{":
        depth += 1
    elif content[pos] == "}":
        depth -= 1
    pos += 1
# pos is now past the closing brace
# Find the ", nullptr, 0, nullptr);" after the closing brace
end_marker = ", nullptr, 0, nullptr);"
end_pos = content.find(end_marker, pos)
if end_pos < 0:
    print("ERROR: end marker not found")
    raise SystemExit(1)
lambda_end = end_pos + len(end_marker)

# Extract the lambda body (everything between the braces)
lambda_body = content[brace_start + 1 : pos - 1].strip()

# Step 2: Create the worker function
# Need to handle Scriban's {{ }} tags properly
# The lambda body has CHAOS_FACT_CHECK() which is a macro defined in RunFactJsonMode
# and {{ if is_jit }} for RemoveVectoredExceptionHandler
worker_func = f"""// ── Fact dispatch worker (extracted from RunFactJsonMode) ──
// Fixes MSVC C2712/C2713: __try/__except cannot coexist with C++ lambda.
static DWORD WINAPI FactDispatchWorker(LPVOID) {{
    void* g_fact_veh = AddVectoredExceptionHandler(1, FactVehHandler);
    const int kCount = kSubjectEntryCount;
    printf("{{\\"factResults\\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {{
        int i = kSubjectSlotMap[si];
        int64_t result = 0;
        bool caught = false;
        signal(SIGABRT, [](int) {{ longjmp(t_abort_jmp, 1); }});
        if (_setjmp(t_abort_jmp) == 0) {{
            __try {{
                result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                    GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            }} __except(EXCEPTION_EXECUTE_HANDLER) {{ caught = true; }}
        }} else {{ caught = true; }}
        if (!first) printf(",");
        printf("{{\\"si\\":%d,\\"methodIndex\\":%d,\\"contractIndex\\":-1,\\"passed\\":%s,\\"value\\":%" PRId64 "}}",
               si, i, caught ? "false" : "true", caught ? -1 : result);
        first = false;
    }}
    printf("]}}\\n");
    std::fflush(stdout);
    CHAOS_FACT_CHECK();
{{{{ if is_jit }}}}
    RemoveVectoredExceptionHandler(g_fact_veh);
{{{{ end }}}}
    return 0;
}}

// ── Fact JSON mode ─────────────────────────────────────────────────"""

# Step 3: Find where to insert the worker function
# Insert right before RunFactJsonMode
fact_json_marker = "static int RunFactJsonMode()"
insert_pos = content.find(fact_json_marker)
if insert_pos < 0:
    print("ERROR: RunFactJsonMode not found")
    raise SystemExit(1)

# Insert worker function before RunFactJsonMode
content = content[:insert_pos] + worker_func + "\n\n" + content[insert_pos:]

# Step 4: Replace the lambda call
old_call = content[lambda_func_start:lambda_end]
new_call = "HANDLE worker = CreateThread(nullptr, 4 * 1024 * 1024, FactDispatchWorker, nullptr, 0, nullptr);"

# The old_call position shifted due to insertion
# Re-find it
adjusted_start = lambda_func_start + len(worker_func) + 2  # +2 for \n\n
old_call_new = content[adjusted_start:adjusted_start + (lambda_end - lambda_func_start)]
if "CreateThread" in old_call_new and "[](LPVOID)" in old_call_new:
    content = content.replace(old_call_new, new_call, 1)
    print("Lambda replaced with FactDispatchWorker")
else:
    # Search for any remaining lambda
    idx = content.find("[](LPVOID) -> DWORD")
    if idx >= 0:
        print(f"Found lambda at {idx}")
        # Replace from HANDLE worker to the end
    else:
        print("No lambda remaining (already replaced)")

# Verify brace balance
opens = content.count("{")
closes = content.count("}")
print(f"Brace balance: {opens} open, {closes} close, delta={opens-closes}")

with open(TPL, "w", encoding="utf-8") as f:
    f.write(content)
print("Template updated successfully")
