"""Post-process generated native-aot.generated.cpp to add goto-based loop control
flow for b__0 function, whose structured IR recovery fails (br/blt emitted as NO-OP
comments)."""
import os

FILE = r'D:/agent/booming-il2cpp/testing/project-test/runtime-self-test/codegen/RuntimeSelfTest/generated/native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# Match the function DEFINITION (with opening brace), not the declaration
fn_marker = b'// Managed method: RuntimeSelfTest/IntegrationProof+<>c__DisplayClass4_0::<ThreadedGcAllocation>b__0()'
fn_idx = data.find(fn_marker)
assert fn_idx >= 0, "b__0 managed method comment not found!"

# Find the opening brace of the function (after extern "C" ...)
fn_body_start = data.find(b'{', fn_idx)
assert fn_body_start >= 0, "b__0 opening brace not found!"

# Find closing brace of b__0
fn_end_marker = b'\treturn;\r\n}\r\n'
fn_end = data.find(fn_end_marker, fn_body_start)
assert fn_end >= 0, "b__0 end (return;}) not found!"

b0_section = data[fn_body_start:fn_end + len(fn_end_marker)]
print(f"b__0 body section: {fn_body_start} to {fn_end + len(fn_end_marker)} ({len(b0_section)} bytes)")

# br pattern: \t// br (handled via structured EH branches)\r\n\t{
br_pattern = b'\t// br (handled via structured EH branches)\r\n\t{'
br_count = b0_section.count(br_pattern)
print(f"br count: {br_count}")

# blt pattern: \t// blt (structured EH branch)
blt_pattern = b'\t// blt (structured EH branch)'
blt_count = b0_section.count(blt_pattern)
print(f"blt count: {blt_count}")

assert br_count == 1, f"br count != 1: {br_count}"
assert blt_count == 1, f"blt count != 1: {blt_count}"

# Replace br with label
new_br = b'\tchaos_b0_body:\r\n\t{'

# Replace blt with condition check + goto
# At the blt point: _s0 = chaos_locals[1] (i), _s1 = iterations (from chaos_args[0]->iterations)
# But _s1 is transient - we need to reload iterations from chaos_args[0]
# chaos_locals[1] = i (loop counter), chaos_args[0] = this pointer (closure struct)
new_blt = (
    b'\tchaos_b0_cond:\r\n'
    b'\t{\r\n'
    b'\t\tauto* _chaos_b0_this = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(chaos_args[0]);\r\n'
    b'\t\tCHAOS_IL2CPP_INT32 _chaos_b0_iterations = _chaos_b0_this->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;\r\n'
    b'\t\tif (static_cast<CHAOS_IL2CPP_INT32>(chaos_locals[1]) < _chaos_b0_iterations)\r\n'
    b'\t\t{\r\n'
    b'\t\t\tgoto chaos_b0_body;\r\n'
    b'\t\t}\r\n'
    b'\t}'
)

b0_section = b0_section.replace(br_pattern, new_br, 1)
b0_section = b0_section.replace(blt_pattern, new_blt, 1)

new_data = data[:fn_body_start] + b0_section + data[fn_body_start + len(b0_section):]

with open(FILE, 'wb') as f:
    f.write(new_data)

print("Done!")
print("  Added chaos_b0_body: label before loop body")
print("  Added chaos_b0_cond: if (i < iterations) goto chaos_b0_body;")
