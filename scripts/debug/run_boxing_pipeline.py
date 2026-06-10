import sys, os
sys.path.insert(0, r'D:\agent\booming-il2cpp\testing\foundation-dll')
sys.path.insert(0, r'D:\agent\booming-il2cpp\testing\foundation-dll\verification')
os.chdir(r'D:\agent\booming-il2cpp')
from verification.entry_points.cli import main
import sys as _sys
_sys.argv = ['', 'boxing-unboxing-casts', '--assembly', 'System.Private.CoreLib', '--mode', 'strict', '--native-config', 'check', '--verbose']
main()
