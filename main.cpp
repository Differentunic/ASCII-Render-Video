#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cwchar>
#include <windows.h>

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "libraries/stb_image.h"
}

// Loads as RGBA even if file is only RGB
bool load_image(std::vector<unsigned char>& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
    if (data != nullptr)
    {
        image = std::vector<unsigned char>(data, data + x * y * 4);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

void consoleSetup() {
    // Console setup for square characters
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 6;
    cfi.dwFontSize.Y = 6;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    std::wcscpy(cfi.FaceName, L"Consolas");
    // Setup for making console fullscreen
    system("mode con COLS=700");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
    ShowWindow(GetConsoleWindow(),SW_MAXIMIZE);
    SendMessage(GetConsoleWindow(),WM_SYSKEYDOWN,VK_RETURN,0x20000000);
}

int main() {
    // ASCII characters for 10 levels of shading
    std::string SHADE[] = {" ", ".", ":", "-", "=", "+", "*", "#", "%", "@"};
    // Image path
    std::string filepath = "frames_bad_apple\\";
    // File extension
    std::string fileextention = ".png";

    consoleSetup();
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    int width, height;
    const size_t RGBA = 4;
    // Division to image resolution
    const int resolution_fivision = 6;
    // String to hold each row of characters
    std::string line;
    // String that holds each frame
    std::string final;
    // Values of total frames and the frame rate to be played back at
    int total_frames = 6572;
    int framerate = 30;

    for (int f = 1; f < total_frames; f+=1) {
        // Start of frame time
        auto start = std::chrono::high_resolution_clock::now();
        final.clear();
        std::vector<unsigned char> image;
        bool success = load_image(image, filepath + std::to_string(f) + fileextention, width, height);
        if (!success) {
            std::cout << "Error loading image\n";
            return 1;
        }
        for (int h = 0; h < height; h += resolution_fivision) {
            for (int w = 0; w < width; w += resolution_fivision) {
                // Gets values for each pixel
                int value = RGBA * (h * width + w);
                int r = static_cast<int>(image[value + 0]);
                int g = static_cast<int>(image[value + 1]);
                int b = static_cast<int>(image[value + 2]);
                int a = static_cast<int>(image[value + 3]);
                int c = (a > 0 ? (r + g + b) / 3 / 25.51 : 0);

                line = line + SHADE[c];
            }
            final = final + line + '\n';
            line.clear();
        }
        // End of frame time
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        // Make sure frame rate is consistent
        Sleep(duration.count() >= 0 ? (duration.count() >= (1000/framerate) ? 0 : ((1000/framerate) - duration.count())) : (1000/framerate));
        // Prints final ASCII image after for loops have completed
        std::cout << final << std::flush;
        // Resets cursor position to overwrite previous frame
        SetConsoleCursorPosition(console, { 0,0});
    }
    std::cout << '\n' << "Press a ENTER to continue...";
    std::cin.ignore();
    return 0;
}
