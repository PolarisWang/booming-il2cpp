---
task_id: 20260417-04-aot-skill-flow-and-token-optimization-plan
title: AOT Skill Flow And Token Optimization Plan
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 23:22:10 +08:00
updated_at: 2026-04-17 23:54:00 +08:00
latest_stop_point: authority/docs/skill optimization is complete; validation passed and the task has been archived to completed
current_dir: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan
parent_task_id:
source_task_id: 20260417-01-test-process-and-skill-governance
source_relation: follow-up-on-aot-skill-flow-review-and-token-budget-audit
design_doc: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md
plan_doc: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/plan-v1-01.md
total_tasks: 6
current_task: all-tasks-complete
active: false
---

## 鍏抽敭鏂囨。

- `docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md`
- `docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/plan-v1-01.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`
- `wiki/02-Skill浣撶郴/04-璐ㄩ噺淇濋殰/project-test-governance.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/STATUS.md`

## 褰撳墠缁撹

- authority 鍒嗗伐宸插喕缁擄細
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md` 鎷ユ湁 execution spine / layering / contract 杈圭晫
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md` 鎷ユ湁 formal verification / projection / archive 杈圭晫
  - `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md` 鎷ユ湁 capability intake銆乷wner/proof/benchmark/hotupdate obligation 涓?formal verification 椤哄簭
  - `wiki/06-娴嬭瘯楠岃瘉/INDEX.md` 鎷ユ湁姝ｅ紡楠岃瘉鍏ュ彛銆佸璞″鑸笌 completion 鍓嶅璞′紭鍏堢骇
- `writing-plans` 宸叉妸 `capabilityFamily`銆乣capabilityItem`銆乣ownerSubjectId`銆乣proofRequired`銆乣benchmarkRequired`銆乣hotupdateImpact`銆乣formalVerificationObjects`銆乣requiredGates` 绾冲叆鏄惧紡 intake contract銆?
- `executing-plans`銆乣subagent-driven-development` 涓?`verification-before-completion` 宸叉妸 obligation-driven formal verification gate 鎺ュ叆 `completed` 鍒ゅ畾銆?
- 楂橀 skill 鐑矾寰勬€讳綋宸蹭粠 `18380` 瀛楃闄嶅埌 `17088` 瀛楃锛屽噣鍑忓皯 `1292` 瀛楃锛岀害 `7.0%`銆?

## 鏈€杩戞憳瑕?

- 2026-04-17 23:54:00 +08:00: 宸插畬鎴?authority銆乻kill銆亀iki 闀滃儚涓庣储寮曟敼閫狅紱鏂囨湰绾ч獙璇併€佸叧閿瘝閾捐矾鏍￠獙涓?3 缁?scenario-based 瀹℃煡閫氳繃锛屼换鍔¤繘鍏?completed 鏀跺彛銆?
- 2026-04-17 23:41:13 +08:00: 鐢ㄦ埛瑕佹眰鎸?`20260417-04` 璁″垝鐩存帴鎵ц鍒板簳锛涗换鍔″凡浠?`planning` 鍒囧埌 `executing`锛屽綋鍓嶆壒娆″厛琛ユ墽琛屾€佸苟鍐荤粨 authority / obligation intake 鍩虹嚎銆?
- 2026-04-17 23:22:10 +08:00: 鍩轰簬 AOT skill 娴佺▼瀹℃煡锛屽喕缁?follow-up 璁″垝锛岀洰鏍囨槸鎶?capability/owner/proof/benchmark obligation銆乫ormal verification gate 涓?token slimming 涓夋潯绾挎敹鎴愪竴濂楀彲鎵ц鏀归€犮€?

## 涓嬩竴姝?

- next_action: 鏈换鍔″凡瀹屾垚锛涘悗缁闇€ git 鏀跺彛锛岃繘鍏?`dev:finishing-a-development-branch`銆?
- owner: codex
- trigger: completed 褰掓。鍚庣殑鏈湴鎻愪氦涓庡垎鏀敹鍙?

## 椋庨櫓 / 闃诲

### risks

- 鏈瀹屾垚鍒ゆ柇鍩轰簬 skill / wiki / architecture 鐨勬枃鏈骇楠岃瘉涓?scenario 瀹℃煡锛屼笉鍖呭惈浠讳綍鏂扮殑 IL2CPP 杩愯鏃朵唬鐮佸洖褰掋€?
- 褰撳墠浠撳簱浠嶅瓨鍦ㄧ敤鎴蜂晶骞惰鑴忔敼鍔紱鑻ョ户缁仛 git 鏀跺彛锛屽繀椤婚伩鍏嶆妸鏃犲叧鏀瑰姩涓€骞舵彁浜ゃ€?

### blockers

- 褰撳墠鏃犵‖闃诲銆?
- 璇存槑: 鏈换鍔℃槸瑙勫垯涓庢枃妗ｆ不鐞嗘敼閫狅紝涓嶄緷璧栧綋鍓?AOT 浠ｇ爜涓荤嚎鏀跺彛瀹屾垚锛涗絾浠撳簱瀛樺湪鐢ㄦ埛渚у苟琛岃剰鏀瑰姩锛屽疄鏂芥椂蹇呴』閬垮厤瑕嗙洊鏃犲叧鏂囦欢銆?

## 楠岃瘉

- 鏂囨湰涓€鑷存€э細
  - `git diff --check -- .codex/skills wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 wiki docs/dev`
  - 缁撴灉锛氶€氳繃锛涙棤 whitespace error锛屼粎鏈?Git 琛屽熬杞崲璀﹀憡
- 鍏抽敭璇嶉摼璺細
  - 宸茬‘璁?`ownerSubjectId`銆乣proofRequired`銆乣benchmarkRequired`銆乣hotupdateImpact`銆乣formalVerificationObjects`銆乣requiredGates` 璐€氬埌楂橀 skill 涓?authority 鏂囨。
  - 宸茬‘璁?`collector -> registry -> workspace`銆乣managed proof`銆乣native proof`銆乣hotupdate proof`銆乣canonicalCommand`銆乣required-before-completion` 鐨勫叧閿摼璺瓨鍦?
- token 棰勭畻澶嶇畻锛?
  - 鐑矾寰勭粍鍚堬細`using-booming + project-test-governance + writing-plans + executing-plans + verification-before-completion + test-driven-development`
  - 鏀归€犲墠锛歚18380` 瀛楃
  - 鏀归€犲悗锛歚17088` 瀛楃
- scenario-based 瀹℃煡锛?
  - 鍦烘櫙 1锛氭柊澧?AOT feature锛岄渶 owner/proof/benchmark/hotupdate obligation
  - 鍦烘櫙 2锛歜enchmark 寮傚父鎺掓煡锛岄渶 routing 鍒?debugging + governance + TDD
  - 鍦烘櫙 3锛歠ormal verification object 缂哄け鏃跺皾璇曞綊妗?`completed`
  - 缁撴灉锛氫笁缁勫満鏅潎鑳戒粠褰撳墠 skill 涓?authority 鏂囨。寰楀埌鏄庣‘ routing / gate / stop 鏉′欢

## wiki

- 鏈疆 wiki 鏇存柊宸插畬鎴?
- 鑼冨洿: `wiki/06-娴嬭瘯楠岃瘉/`銆乣wiki/02-Skill浣撶郴/` 涓?`skill-registry`
- 鍘熷洜: 褰撳墠鎵ц鍐呭灞炰簬闀挎湡鏈夋晥鐨?authority / skill 瑙勫垯娌夋穩
