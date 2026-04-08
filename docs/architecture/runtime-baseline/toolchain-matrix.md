# Toolchain Matrix (Runtime Baseline)

## 鐩爣

`Runtime Baseline` 鍙喕缁?host-specific matrix 鎵€闇€鐨?toolchain 鎻忚堪锛屼笉鎶?Android銆乮OS銆丩inux 鐩存帴鎺ㄨ繘鍒板畬鏁村钩鍙拌涔夊疄鐜般€?

## 鐭╅樀

| 鐩爣 | Host | Preset | Toolchain | Gate 寮哄害 | 澶囨敞 |
| --- | --- | --- | --- | --- | --- |
| Windows x64 reference | Windows | `windows-x64-reference` | `build/toolchains/windows-x64-reference.cmake` | 寮?gate | 鍙傝€冩闈富鍩虹嚎锛屾壙杞介珮闃?ABI / bridge contract 缁勫悎楠岃瘉 |
| macOS reference | macOS | `macos-reference` | `build/toolchains/macos-reference.cmake` | 寮?gate 瀛愰泦 | 鍙傝€冩闈㈠悓璇箟瀛愰泦锛屼繚鎸佷笌 Windows 鎺ヨ繎鐨?contract 杈圭晫 |
| Android arm64 | Windows | `android-arm64-smoke` | `build/toolchains/android-arm64.cmake` | startup smoke shell | 鍙姹傜湡瀹?startup smoke 璺緞鍙皟搴︼紝浜х墿杩涘叆 `artifacts/` |
| iOS arm64 | macOS | `ios-arm64-packaging` | `build/toolchains/ios-arm64.cmake` | compile/link/packaging shell | 涓嶈姹傚畬鏁?iOS runtime 璇箟锛屼絾蹇呴』淇濈暀瀹屾暣 packaging 璺緞 |
| Linux x64 | Windows / macOS | `linux-x64-packaging` | `build/toolchains/linux-x64.cmake` | cross-compile / packaging shell | 浣滀负 common gate 琚袱涓?host profile 鍏卞悓璋冨害 |

## Preset 涓?sourceDir 瀵归綈

- `windows-x64-reference` 褰撳墠缁戝畾 `tests/contracts/native/abi`锛岀敤 ABI compile-only smoke 浣滀负 reference desktop 鐨勪綆灞?toolchain 閿氱偣銆?
- `macos-reference` 褰撳墠缁戝畾 `tests/contracts/native/bridge`锛岀敤 bridge compile-only smoke 浣滀负 macOS reference 鐨勪綆灞?toolchain 閿氱偣銆?
- `android-arm64-smoke` 涓?`ios-arm64-packaging` 鐩存帴缁戝畾鍚勮嚜鐨勫钩鍙?harness 澶栧３銆?
- `linux-x64-packaging` 缁戝畾 `tests/platform/linux-packaging`锛岀敤鏉ュ喕缁?Linux cross-compile / packaging shell 鐨勭嫭绔嬪叆鍙ｃ€?

## 鐜鍙橀噺

- Android: 鍙€夎鍙?`ANDROID_NDK_ROOT`
- iOS: 渚濊禆 Xcode / `DEVELOPER_DIR`
- Linux: 鍙€夎鍙?`ROADMAP0_LINUX_SYSROOT`

## 缁撹

鏈枃浠跺喕缁撶殑鏄€滅煩闃靛浣曡璋冨害鈥濓紝涓嶆槸鈥滄瘡涓钩鍙板凡缁忓畬鏁村疄鐜板埌浠€涔堢▼搴︹€濄€俙Runtime Baseline` 鐨勯噸鐐逛粛鐒舵槸 contract銆乻moke 涓?verify gate 鐨勫叆鍙ｄ竴鑷存€с€?

