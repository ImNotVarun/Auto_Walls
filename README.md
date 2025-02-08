# Wallpaper Changer for Windows

This is a simple Windows application that automatically sets a new wallpaper on startup. It fetches high-quality images from the Pexels API and updates your desktop background. The app runs once at startup and exits without running in the background.

## Features

- Downloads a new random wallpaper from Pexels.
- Automatically sets the wallpaper on Windows startup.
- Stores only the last two wallpapers to save storage.
- Does not run persistently in the background.
- Option to uninstall and remove all files easily.

## Setup & Installation
### 1. Bypass Installation

You can also bypass manual setup by simply installing the .exe file.
Once installed, it will automatically handle startup and wallpaper updates without any extra configuration.

### 1.1 Get Your Pexels API Key (Optional)

You can use the provided API key, but if you want your own:

1. Sign up at Pexels API.
2. Generate an API key.
3. Replace the `api_key` value in `WinMain` inside `wallpaper_changer.cpp`:
   ```cpp
   std::string api_key = "YOUR_PEXELS_API_KEY";
   ```

### 2. Build & Run

#### Windows Setup:

1. Install a C++ compiler (MinGW or MSVC).
2. Ensure you have Windows SDK installed.
3. Open a terminal (CMD or PowerShell) and compile using:
   ```sh
   g++ -o Auto_Wall.exe wall.cpp -lwininet -mwindows
   ```
4. Run `Auto_Wall.exe` to test.
5. The app will automatically start on the next boot.

### 3. Uninstallation

To remove the program:

1. Run `uninstaller.exe` or compile `uninstaller.cpp` using:
   ```sh
   g++ uninstall.cpp -o uninstall.exe -mwindows
   ```
2. The program will:
   - Remove itself from startup.
   - Delete all downloaded wallpapers.
   - Show a confirmation message.

## How It Works

1. On startup, it checks if it's already added to the Windows startup registry.
2. It fetches a random wallpaper from Pexels.
3. Downloads and saves it in `C:\Users\<YourUser>\AppData\Local\WallpaperChanger`.
4. Updates the Windows wallpaper.
5. Exits immediately.

## Notes

- The application only runs when executed or on startup.
- You can manually rerun `WallpaperChanger.exe` to fetch a new wallpaper anytime.
- The uninstaller removes all program traces.

## License

This project is open-source and free to use.

