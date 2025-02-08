#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <shlobj.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdlib>  // For rand() and srand()

#pragma comment(lib, "wininet.lib")

// Function to add program to startup (only if not already added)
void AddToStartup() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        char existingPath[MAX_PATH];
        DWORD size = sizeof(existingPath);
        
        if (RegQueryValueExA(hKey, "WallpaperChanger", NULL, NULL, (LPBYTE)existingPath, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return; // Already in startup, no need to add again
        }
        RegCloseKey(hKey);
    }

    // If not found, add the entry
    char executablePath[MAX_PATH];
    GetModuleFileNameA(NULL, executablePath, MAX_PATH);
    
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WallpaperChanger", 0, REG_SZ, (BYTE*)executablePath, strlen(executablePath) + 1);
        RegCloseKey(hKey);
    }
}

// Function to get wallpapers directory
std::string GetWallpapersDir() {
    char appDataPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath);
    std::string wallpaperDir = std::string(appDataPath) + "\\WallpaperChanger";
    std::filesystem::create_directories(wallpaperDir);
    return wallpaperDir;
}

// Function to clean old wallpapers
void CleanOldWallpapers() {
    std::string wallpaperDir = GetWallpapersDir();
    std::vector<std::filesystem::directory_entry> files;
    
    for (const auto& entry : std::filesystem::directory_iterator(wallpaperDir)) {
        if (entry.path().extension() == ".jpg") {
            files.push_back(entry);
        }
    }
    
    // Sort files by last write time
    std::sort(files.begin(), files.end(), 
        [](const auto& a, const auto& b) {
            return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
        });
    
    // Keep only the last 2 wallpapers
    while (files.size() > 2) {
        std::filesystem::remove(files[0]);
        files.erase(files.begin());
    }
}

// Function to download image
bool DownloadImage(const std::string& url, const std::string& filename) {
    HINTERNET hInternet = InternetOpen("WallpaperChanger", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream outFile(filename, std::ios::binary);
    char buffer[1024];
    DWORD bytesRead;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outFile.write(buffer, bytesRead);
    }

    outFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return true;
}

// Function to set wallpaper
void SetWallpaper(const std::string& filename) {
    SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)filename.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

// Function to check if the program has run before
bool IsFirstRun() {
    std::string flagFilePath = GetWallpapersDir() + "\\first_run.flag";
    return !std::filesystem::exists(flagFilePath);
}

// Function to mark the program as having run
void MarkAsRun() {
    std::string flagFilePath = GetWallpapersDir() + "\\first_run.flag";
    std::ofstream flagFile(flagFilePath);
    flagFile.close();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Add to startup only if it's not already in the registry
    AddToStartup();
    
    // Clean old wallpapers
    CleanOldWallpapers();

    // Pexels API endpoint and key
    std::string api_key = "You-Pexels-API";  // Replace with your actual Pexels API key
    int randomPage = std::rand() % 1000 + 1;  // Random page number between 1 and 1000
    std::string url = "https://api.pexels.com/v1/search?query=nature&per_page=1&page=" + std::to_string(randomPage);

    HINTERNET hInternet = InternetOpen("WallpaperChanger", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrl(
            hInternet,
            url.c_str(),
            ("Authorization: " + api_key + "\r\n").c_str(),
            -1L,
            INTERNET_FLAG_RELOAD,
            0
        );

        if (hConnect) {
            char buffer[8192];
            DWORD bytesRead;
            std::string response;

            while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                response.append(buffer, bytesRead);
            }

            InternetCloseHandle(hConnect);

            size_t pos = response.find("\"landscape\":\"");  // Extract 1920x1080 image
            if (pos != std::string::npos) {
                pos += 13;

                size_t endPos = response.find("\"", pos);
                if (endPos != std::string::npos) {
                    std::string image_url = response.substr(pos, endPos - pos);
                    
                    // Generate filename in wallpapers directory
                    std::string wallpaperDir = GetWallpapersDir();
                    std::string filename = wallpaperDir + "\\wallpaper_" + std::to_string(std::time(0)) + ".jpg";

                    if (DownloadImage(image_url, filename)) {
                        // Set the wallpaper
                        SetWallpaper(filename);

                        // Check if it's the first run and show the message
                        if (IsFirstRun()) {
                            MessageBox(NULL, 
        "The app starts with Windows and changes the wallpaper.\n\n"
    "It does not run in the background.\n\n"
    "Run the .exe to set a wallpaper.\n\n"
    "Rerun it to change the wallpaper.",
        "Wallpaper Changer", MB_OK | MB_ICONINFORMATION);
                            MarkAsRun(); // Mark as run after showing the message
                        }
                    }
                }
            }
        }
        InternetCloseHandle(hInternet);
    }

    return 0;
}