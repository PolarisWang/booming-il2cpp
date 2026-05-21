# 杩涘害璁板綍 v1-05

## 鏃堕棿

- 2026-04-06 01:21:23 +08:00

## 宸插畬鎴?

- 灏嗘湰浠诲姟鐨勬寮忔帴鍏ョ粨鏋滃悓姝ュ洖鐖?roadmap锛?
  - Stage 3 宸蹭笉鍐嶅彧鏄换鍔″唴涓存椂楠岃瘉锛岃€屾槸宸茬粡娌夋穩涓洪」鐩骇姝ｅ紡楠岃瘉瀵硅薄閾?
  - 鐖?roadmap 褰撳墠鍥炲埌 Stage 4 娲剧敓鍐崇瓥鐐?
- 瀹屾垚 Stage 3 褰掓。鏀跺彛锛?
  - 浠诲姟鐘舵€佸垏涓?`completed`
  - 褰撳墠闃舵鍒囦负 `archived`
  - active 涓婁笅鏂囧垏鍥炵埗 roadmap
  - 鍑嗗灏嗕换鍔＄洰褰曚粠 `docs/dev/in-progress/` 绉诲叆 `docs/dev/completed/`
- 鍚屾 `docs/dev/INDEX.md`銆乣docs/dev/in-progress/INDEX.md`銆乣docs/dev/completed/INDEX.md` 涓?`docs/dev/ACTIVE.md`

## 楠岃瘉

- `python -m unittest tests.tooling.run.test_python_unittest_command`
- `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_registry_command`
- `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- `python -m unittest tests.unit.run.test_stage2_helloworld_proof_spec`
- `powershell -ExecutionPolicy Bypass -File .\run.ps1 test registry check-consistency --json`
- `powershell -ExecutionPolicy Bypass -File .\run.ps1 test module --id module/managed-closure/basic --json`

## 褰撳墠鐘舵€?

- 浠诲姟鐘舵€侊細`completed`
- 褰撳墠闃舵锛歚archived`

## 缁撹

- Stage 3 `Managed Minimal Closure` 宸插畬鎴愬綊妗ｃ€?
- 鑻ョ户缁?reboot 涓荤嚎锛屼笅涓€姝ュ簲鍥炲埌鐖?roadmap锛屽喅瀹氭槸鍚︽淳鐢?Stage 4 `Native Bootstrap And Reference Proof`銆?
