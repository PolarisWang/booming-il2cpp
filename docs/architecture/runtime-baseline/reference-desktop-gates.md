# Reference Desktop Gates (Runtime Baseline)

## 瀹氫綅

`Windows x64 + macOS` 鍏卞悓鏋勬垚 `Runtime Baseline` 鐨?reference desktop銆傚畠浠笉鏄绛夊鍒讹紝鑰屾槸鈥淲indows 涓诲熀绾?+ macOS 鍚岃涔夊瓙闆嗏€濄€?

## Windows x64

- 浣滀负楂橀樁 contract 涓诲熀绾?
- 璐熻矗 ABI / bridge compile-only smoke
- 璐熻矗 5 涓?smoke 杈撳叆椤圭洰鐨勪富绾?build
- 璐熻矗 Windows lazy warmup trace compare
- 骞冲彴鑳藉姏鐩稿叧 `icall` 浠呬繚鐣?query / basic-path 绾у埆瑕佹眰

## macOS

- 浣滀负 reference desktop 鐨勫悓璇箟瀛愰泦
- 璐熻矗涓?Windows 鎺ヨ繎鑼冨洿鐨?trace / contract 楠岃瘉
- 璐熻矗 macOS lazy warmup trace compare
- iOS compile/link/packaging gate 鐢?macOS host 璋冨害
- 骞冲彴鑳藉姏鐩稿叧 `icall` 鍚屾牱鍙繚鐣?query / basic-path 鐣欑櫧

## 鍏佽鐣欑櫧

- 涓嶈姹?`Runtime Baseline` 瀹屾垚骞冲彴涓撳睘 `icall` 鐨勫畬鏁村疄鐜?
- 涓嶈姹?Android / iOS 鎻愬墠杩涘叆瀹屾暣绉诲姩骞冲彴 runtime 璇箟
- 鍏佽 reference desktop gate 浠?contract銆乼race銆乻tartup shell銆乸ackaging shell 涓轰富锛岃€屼笉鏄畬鏁翠骇鍝佺骇杩愯鏃?

## 涓?Host Matrix 鐨勫叧绯?

- Windows host: `Windows + Android + Linux`
- macOS host: `macOS + iOS + Linux`
- Linux cross-compile / packaging 姘歌繙灞炰簬 common gate锛岀敱涓や釜 host profile 閮借皟搴?

## 缁撹

reference desktop gate 鐨勭洰鏍囦笉鏄€滄妸鎵€鏈夊钩鍙伴兘璺戦€氣€濓紝鑰屾槸缁欏悗缁?runtime / codegen 婕旇繘鎻愪緵涓€涓ǔ瀹氥€佸彲杩借矗銆佸彲 diff 鐨勬闈?contract 鍩虹嚎銆?

