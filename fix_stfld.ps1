$f = 'D:\agent\booming-il2cpp\src\native\jit\jit_engine.cpp'
$c = [System.IO.File]::ReadAllText($f)

$arrow = [char]0x2192
$dash = [char]0x2014

$old = @"
    case IROpCode::StFld: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        // Tier 1: inline g_bgc_is_marking check to skip SATB barrier
        // when concurrent BGC marking is not active (common case).
        // Tier 0: always call the full barrier (keep path simple).
        if (!is_tier0_) {
            using chaos::il2cpp::runtime_core::g_bgc_is_marking;
            // Load args (shared by both paths)
            LoadGpr(kRCX, instr.src1_reg());
            enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
            LoadGpr(kR8, instr.src2_reg());
            // Check g_bgc_is_marking
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(&g_bgc_is_marking));
            // Emit: cmp byte [rax], 0  (opcode 80 /7 ib)
            buf_.EmitByte(0x80);
            buf_.EmitByte(0x38);
            buf_.EmitByte(0x00);
            uint32_t jmp_no_barrier = buf_.pos();
            enc_.EmitJccRel32(kCC_E, 0);  // je (not marking $arrow call CodegenStFldNoBarrier)
            // Marking active $arrow call full CodegenStFld (with SATB barrier)
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
            EmitCallWithSpill(kRAX);
            uint32_t jmp_done = buf_.pos();
            enc_.EmitJmpRel32(0);  // jmp done
            // Not marking $arrow call CodegenStFldNoBarrier (without SATB barrier)
            int32_t patch_no_barrier = static_cast<int32_t>(buf_.pos() - (jmp_no_barrier + 6));
            buf_.Patch32(jmp_no_barrier + 2, static_cast<uint32_t>(patch_no_barrier));
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFldNoBarrier));
            EmitCallWithSpill(kRAX);
            // done
            int32_t patch_done = static_cast<int32_t>(buf_.pos() - (jmp_done + 5));
            buf_.Patch32(jmp_done + 2, static_cast<uint32_t>(patch_done));
        } else {
            // Tier 0: simple path $dash always call CodegenStFld (full barrier)
            LoadGpr(kRCX, instr.src1_reg());
            enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
            LoadGpr(kR8, instr.src2_reg());
            enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
            EmitCallWithSpill(kRAX);
        }
        return true;
"@

$new = @"
    case IROpCode::StFld: {
        if (!instr.has_src1() || !instr.has_src2()) return false;
        // Simple path $dash always call CodegenStFld (full SATB barrier).
        LoadGpr(kRCX, instr.src1_reg());
        enc_.EmitMovRIImm32(kRDX, instr.imm.field_offset);
        LoadGpr(kR8, instr.src2_reg());
        enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(::CodegenStFld));
        EmitCallWithSpill(kRAX);
        return true;
"@

$idx = $c.IndexOf($old)
if ($idx -ge 0) {
    $c = $c.Remove($idx, $old.Length).Insert($idx, $new)
    [System.IO.File]::WriteAllText($f, $c, [System.Text.UTF8Encoding]::new($false))
    Write-Output "SUCCESS: Replaced StFld handler ($($old.Length) -> $($new.Length) chars)"
} else {
    Write-Output "FAIL: pattern not found"
    # Debug output
    $sf = $c.IndexOf("case IROpCode::StFld")
    if ($sf -ge 0) {
        Write-Output "--- Found at $sf ---"
        Write-Output $c.Substring($sf, 600)
    } else {
        Write-Output "StFld case not found in file!"
    }
}
