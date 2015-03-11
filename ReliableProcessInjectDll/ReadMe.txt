Exe Inject ReliableProcessInjectDll.dll to Services.exe.
InjectReliableProcessInject.dll  control services.exe Inject Target.dll to TargetProcess.

OpenPipe _TARGETNAME.
Recv 256 chars.(processname)
Recv 1024 chars (dll path.)
create remote thread to process.