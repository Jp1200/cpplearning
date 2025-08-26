#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int nScreenHeight = 40;
int nScreenWidth = 240;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;
float fSpeed = 5.0f;
float fFOV = 3.14159f / 4.0f;
float fDepth = 16.0f;

// Set terminal to raw mode (no enter key required)
void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Restore terminal settings
void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Check if key is pressed (non-blocking)
int kbhit() {
    int oldf = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    int ch = getchar();
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Get key (non-blocking)
char getch_nonblock() {
    if (kbhit()) {
        return getchar();
    }
    return 0;
}

// Clear screen with ANSI escape codes
void clearScreen() {
    cout << "\033[2J\033[H";
}

int main() {
    enableRawMode();
    atexit(disableRawMode);

    vector<char> screen(nScreenWidth * nScreenHeight);
    wstring map;

    map += L"################";
    map += L"#...........#..#";
    map += L"#...........#..#";
    map += L"#..........##..#";
    map += L"#..............#";
    map += L"#...#..........#";
    map += L"#..............#";
    map += L"#..........#...#";
    map += L"#..............#";
    map += L"#####..........#";
    map += L"#..............#";
    map += L"#.........##...#";
    map += L"#.#........#...#";
    map += L"#...##.........#";
    map += L"#....#.........#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (true) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        // Handle controls
        char key = getch_nonblock();
        if (key == 'a') fPlayerA -= (0.9f) * fElapsedTime;
        if (key == 'd') fPlayerA += (0.9f) * fElapsedTime;

        if (key == 'w') {
            fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
            }
        }

        if (key == 's') {
            fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
            }
        }

        if (key == 'q') break; // quit

        // Render screen
        for (int x = 0; x < nScreenWidth; x++) {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0.0f;
            bool bHitWall = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth) {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                } else if (map[nTestY * nMapWidth + nTestX] == '#') {
                    bHitWall = true;
                }
            }

            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            char nShade = ' ';
            if (fDistanceToWall <= fDepth / 4.0f) nShade = '#';
            else if (fDistanceToWall < fDepth / 3.0f) nShade = 'O';
            else if (fDistanceToWall < fDepth / 2.0f) nShade = '*';
            else if (fDistanceToWall < fDepth) nShade = '.';
            else nShade = ' ';

            for (int y = 0; y < nScreenHeight; y++) {
                if (y < nCeiling) screen[y * nScreenWidth + x] = ' ';
                else if (y > nCeiling && y <= nFloor) screen[y * nScreenWidth + x] = nShade;
                else {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25) nShade = '#';
                    else if (b < 0.5) nShade = 'x';
                    else if (b < 0.75) nShade = '.';
                    else if (b < 0.9) nShade = '-';
                    else nShade = ' ';
                    screen[y * nScreenWidth + x] = nShade;
                }
            }
        }

        clearScreen();
        for (int i = 0; i < nScreenHeight; i++) {
            for (int j = 0; j < nScreenWidth; j++) {
                cout << screen[i * nScreenWidth + j];
            }
            cout << "\n";
        }

        cout << "X=" << fPlayerX << " Y=" << fPlayerY << " A=" << fPlayerA
             << " FPS=" << 1.0f / fElapsedTime << " (press Q to quit)\n";

        cout.flush();
        usleep(16000); // ~60 FPS
    }

    return 0;
}
