$f = 'D:\agent\booming-il2cpp\src\native\jit\jit_engine.cpp'
$c = [System.IO.File]::ReadAllText($f)

# Find StFld case block
$idx = $c.IndexOf('case IROpCode::StFld')
$end = $c.IndexOf('return true;', $idx) + 11  # include "return true;"

Write-Output "StFld block: $idx to $end"

$oldBlock = $c.Substring($idx, $end - $idx)
Write-Output "Old block length: $($oldBlock.Length)"

# New simple block
$simple = @'
    case IROpCode::StFld: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        // Simple path -- always call CodegenStFld (full SATB barrier).
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        LoadGpr(kR8, instr.src2_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
        EmitCallWithSpill(kRAX);
        return true;
'@

# Replace
$c = $c.Remove($idx, $oldBlock.Length).Insert($idx, $simple)
[System.IO.File]::WriteAllText($f, $c, [System.Text.UTF8Encoding]::new($false))
Write-Output "Replaced successfully. New file length: $($c.Length)"
