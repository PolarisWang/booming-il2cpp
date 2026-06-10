@echo off
set PYTHONPATH=D:\agent\booming-il2cpp\testing\foundation-dll;D:\agent\booming-il2cpp\testing\foundation-dll\verification
cd /d D:\agent\booming-il2cpp
"C:\Program Files\Python312\python" -m verification.entry_points.cli boxing-unboxing-casts --assembly System.Private.CoreLib --mode strict --native-config check --verbose
