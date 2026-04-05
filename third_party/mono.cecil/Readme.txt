Current revision:

https://github.com/Unity-Technologies/cecil, branch = unity-master, commit = 55142e20f3972f9d83f2c18644a5d4093ad2f526

To update the build on Windows, perform the following steps
1) Build il2cpp.sln
2) From the il2cpp solution directory run :

	>dotnet build\tools\net6.0\Unity.IL2CPP.NetCoreBuilder.dll --build-cecil --cecil-source-dir=<path to your cecil checkout>

3) Manually update the commit revision above to be the revision of your checkout.  (The updater tool will attempt to perform this step automatically)
