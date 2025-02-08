#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <filesystem>

#pragma comment(lib, "shell32.lib")

// Function to remove the startup registry entry
void RemoveFromStartup() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, "WallpaperChanger"); // Delete registry key
        RegCloseKey(hKey);
    }
}

// Function to get the wallpaper directory path
std::string GetWallpapersDir() {
    char appDataPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath);
    return std::string(appDataPath) + "\\WallpaperChanger";
}

// Function to delete the wallpaper directory
void DeleteWallpaperFolder() {
    std::string wallpaperDir = GetWallpapersDir();
    if (std::filesystem::exists(wallpaperDir)) {
        std::filesystem::remove_all(wallpaperDir);
    }
}

int main() {
    // 1. Remove from startup
    RemoveFromStartup();
    
    // 2. Delete wallpaper directory
    DeleteWallpaperFolder();
    
    // 3. Notify user
    MessageBoxA(NULL, "Auto_wallpaper has been successfully uninstalled.", "Uninstaller", MB_OK | MB_ICONINFORMATION);
    
    return 0;
}
