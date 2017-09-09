#include "OS.h"

#ifdef _WIN32
void OS_Init(){
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    //Shellexecute should have the COM initialized
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
    //Get Shutdown Privlidge
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

int OS_Launch(char* app, char* args){
    SHELLEXECUTEINFO launch;
    BOOL ret;

    if(app == NULL){
        return -1;
    }

    //Populate Launch info
    launch.cbSize = sizeof(launch);
    launch.fMask = SEE_MASK_NOCLOSEPROCESS;
    launch.hwnd = NULL; //TODO: Maybe link the current window handle, only for errors so probably not worth it
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
        return (int)launch.hInstApp;
    }
}

void OS_Shutdown(){
    DWORD temp;
    temp = InitiateShutdown(NULL, NULL, 0, SHUTDOWN_FORCE_OTHERS | SHUTDOWN_FORCE_SELF | SHUTDOWN_POWEROFF, SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
    printf("%d\n", temp);
}

void OS_Restart(){
    DWORD temp;
    temp = InitiateShutdown(NULL, NULL, 0, SHUTDOWN_FORCE_OTHERS | SHUTDOWN_FORCE_SELF | SHUTDOWN_RESTART, SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
    printf("%d\n", temp);
}
#endif