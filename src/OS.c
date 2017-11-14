#include "OS.h"

/* ------ WINDOWS ------ */
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

void OS_Init(){
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    //Shellexecute should have the COM initialized
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
    //Get Shutdown Priviledge
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

int OS_Launch(const char* app, const char* args){
    SHELLEXECUTEINFO launch;
    BOOL ret;

    if(app == NULL){
        return -1;
    }

    //Populate Launch info
    launch.cbSize = sizeof(launch);
    launch.fMask = SEE_MASK_NOCLOSEPROCESS;
    launch.hwnd = NULL;
    launch.lpVerb = NULL;
    launch.lpFile = app;
    launch.lpParameters = args;
    launch.lpDirectory = NULL;
    launch.nShow = SW_MAXIMIZE;
    launch.hInstApp = NULL;

    ret = ShellExecuteEx(&launch);
    if(ret == TRUE){
        return 0;
    } else {
        return (int)(launch.hInstApp);
    }
}

#define WIN_SHUTDOWN_REASON SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED
#define WIN_SHUTDOWN_FLAGS SHUTDOWN_FORCE_OTHERS | SHUTDOWN_FORCE_SELF

void OS_Shutdown(){
    InitiateShutdown(NULL, NULL, 0,  WIN_SHUTDOWN_FLAGS | SHUTDOWN_POWEROFF, WIN_SHUTDOWN_REASON);
}

void OS_Restart(){
    InitiateShutdown(NULL, NULL, 0, WIN_SHUTDOWN_FLAGS  | SHUTDOWN_RESTART, WIN_SHUTDOWN_REASON);
}

/* ------ UNIX & LINUX ------ */
#elif __linux__ || __APPLE__
#include <unistd.h>
#include <stdlib.h>

void OS_Init(){
    //Unix doesnt need any fancy startup
}

int OS_Launch(const char* app, const char* args){
    pid_t pid;
    
    pid = fork();
    if(pid == 0){ //baby cakes
        setpgid(0, 0);
        execlp(app, app, args, NULL);
        return -1; //BAD THINGS HAPPENED
    }
    else if(pid > 0){ //momma bird
        setpgid(pid, pid);
        return 0;
    }
    else{ //ERROR
        return -1;
    }
    return -1;
}

#if __APPLE__
#include <CoreServices/CoreServices.h>
#include <Carbon/Carbon.h>

//Event handler for shutdown/restart
OSStatus OSX_EventHandle(AEEventID EventToSend){
    AEAddressDesc targetDesc;
    static const ProcessSerialNumber kPSNOfSystemProcess = { 0, kSystemProcess };
    AppleEvent eventReply = {typeNull, NULL};
    AppleEvent appleEventToSend = {typeNull, NULL};

    OSStatus error = noErr;

    error = AECreateDesc(typeProcessSerialNumber, &kPSNOfSystemProcess, sizeof(kPSNOfSystemProcess), &targetDesc);
    if (error != noErr)
        return(error);

    error = AECreateAppleEvent(kCoreEventClass, EventToSend, &targetDesc, kAutoGenerateReturnID, kAnyTransactionID, &appleEventToSend);
    AEDisposeDesc(&targetDesc);
    if (error != noErr)
        return(error);

    error = AESend(&appleEventToSend, &eventReply, kAENoReply, kAENormalPriority, kAEDefaultTimeout, NULL, NULL);
    AEDisposeDesc(&appleEventToSend);
    if (error != noErr)
        return(error);

    AEDisposeDesc(&eventReply);

    return(error);
}

void OS_Shutdown(){
    OSX_EventHandle(kAEShutDown);
}

void OS_Restart(){
    OSX_EventHandle(kAERestart);
}
#else

void OS_Shutdown(){
    system("systemctl halt -i");
}

void OS_Restart(){
    system("systemctl reboot -i");
}
#endif
#endif