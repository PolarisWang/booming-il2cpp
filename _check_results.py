import json, os, glob

results_dir = "D:/agent/booming-il2cpp/verification/foundation-dll/hotupdate-results"
for jsonfile in sorted(glob.glob(os.path.join(results_dir, "*.json"))):
    fn = os.path.splitext(os.path.basename(jsonfile))[0]
    with open(jsonfile) as f:
        d = json.load(f)
    print(f"{fn}: total={d['totalMethods']} passed={d['passedMethods']} failed={d['failedMethods']} d3Patch={d.get('d3PatchApplied')}")
