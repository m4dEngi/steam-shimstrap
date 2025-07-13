#include <iostream>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <libgen.h>
#include <fstream>

#define EXPORT extern "C" __attribute__((visibility("default")))

typedef void (*SteamUIMainFn) (int, char**);

typedef enum EUniverse {
    k_EUniverseInvalid,
    k_EUniversePublic,
    k_EUniverseBeta,
    k_EUniverseInternal,
    k_EUniverseDev,
    k_EUniverseMax,
} EUniverse;

typedef enum ELauncherType {
    k_ELauncherTypeDefault,
    k_ELauncherTypePw_dota2,
    k_ELauncherTypeNexon_dota2,
    k_ELauncherTypeSteamcmd,
    k_ELauncherTypePw_csgo,
    k_ELauncherTypeClientui,
    k_ELauncherTypeSteamhdl,
    k_ELauncherTypeSteamchina,
    k_ELauncherTypeSingleapp,
} ELauncherType;

std::string g_SteamBaseDir;
std::string g_SteamLoggingDir;
std::string g_SteamInstallDir;
std::string g_ExecutableDir;
std::string g_CmdLine;
std::string g_UserHome;

std::string GetUserHome() {
    const char* home = getenv("HOME");
    if(home) {
        return std::string(home);
    }
    
    char buf[PATH_MAX];
    return getcwd(buf, sizeof(buf)) ? std::string(buf) : "";
}

std::string GetOwnPath() {
    char buf[PATH_MAX];
    if (readlink("/proc/self/exe", buf, sizeof(buf)) != -1) {
        return std::string(dirname(buf));
    }
    return "";
}

EXPORT uint64_t GetBootstrapperVersion() {
    return 80085;
}

EXPORT ELauncherType GetClientLauncherType() {
    return k_ELauncherTypeDefault;
}

EXPORT ELauncherType GetClientActualLauncherType() {
    return k_ELauncherTypeDefault;
}

EXPORT EUniverse SteamBootstrapper_GetEUniverse() {
    return k_EUniversePublic;
}
    
EXPORT const char* SteamBootstrapper_GetForwardedCommandLine() {
    return g_CmdLine.c_str();
}
    
EXPORT const char* SteamBootstrapper_GetInstallDir() {
    const char* sid = getenv("STEAM_INSTALL_DIR");
    if(sid) {
        return sid;
    }
    return g_SteamInstallDir.c_str();
}
     
EXPORT const char* SteamBootstrapper_GetLoggingDir() {
    const char* sld = getenv("STEAM_LOGGING_DIR");
    if(sld) {
        return sld;
    }
    return g_SteamLoggingDir.c_str();
}
     
EXPORT const char* SteamBootstrapper_GetBaseUserDir() {
    const char* sbud = getenv("STEAM_BASE_USER_DIR");
    if(sbud) {
        return sbud;
    }
    return g_SteamBaseDir.c_str();
}

EXPORT bool CanSetClientBeta() {
    return false;
}
    
EXPORT bool IsCheckingForUpdates() {
    return false;
}

EXPORT bool IsClientUpdateAvailable() {
    return false;
}

EXPORT bool IsClientUpdateOutOfDiskSpace() {
    return false;
}

EXPORT bool IsUpdateSuppressed() {
    return false;
}

EXPORT bool StartCheckingForUpdates(bool bUnk, int iUnk) {
    return 1;
}

EXPORT bool ClientUpdateRunFrame() {
    return true;
}

void GetDirs() {
    // ugly hacky stub
    g_ExecutableDir = GetOwnPath();
    g_UserHome = GetUserHome();
    g_SteamInstallDir = g_UserHome + "/.steam/steam";
    g_SteamBaseDir = GetUserHome() + "/.steam/steam";
    g_SteamLoggingDir = g_SteamBaseDir + "/logs";
}

void WritePid() {
    std::cout << getpid() << std::endl;
    std::ofstream pid_out(g_UserHome + "/.steam/steam.pid", std::ios::out);
    pid_out << getpid() << std::endl;
    pid_out.close();
}

int main(int argc, char* argv[]) {
    GetDirs();
    WritePid();
    
    void* h_steamui = dlopen((g_ExecutableDir + "/steamui.so").c_str(), RTLD_LAZY);
    if(!h_steamui) {
        std::cout << "Err: " << dlerror() << std::endl;
        return 1;
    }
    
    SteamUIMainFn ui_main = (SteamUIMainFn)dlsym(h_steamui, "SteamDllMain");
    ui_main(argc, argv);
    return 0;
}
