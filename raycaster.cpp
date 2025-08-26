#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define PI 3.1415926535

int screenWidth = 120;
int screenHeight = 40;

double playerX = 8.0, playerY = 8.0; // Player position
double playerAngle = 0.0;            // Facing angle
double FOV = PI / 3.0;              // Field of view
double depth = 16.0;                // Render distance
double speed = 0.1;                 // Movement speed

std::string map;
int mapWidth = 16;
int mapHeight = 16;

// Set terminal to raw mode
void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Check if key pressed
bool kbhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting > 0;
}

char getch() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}

int main() {
    // Create a simple map (walls #, empty .)
    map += "################";
    map += "#..............#";
    map += "#.......########";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "###............#";
    map += "##.............#";
    map += "#......##......#";
    map += "#......##......#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    enableRawMode();

    // Main game loop
    while (true) {
        std::vector<char> screen(screenWidth * screenHeight);

        for (int x = 0; x < screenWidth; x++) {
            double rayAngle = (playerAngle - FOV / 2.0) + ((double)x / (double)screenWidth) * FOV;
            double distanceToWall = 0.0;
            bool hitWall = false;

            double eyeX = sin(rayAngle);
            double eyeY = cos(rayAngle);

            while (!hitWall && distanceToWall < depth) {
                distanceToWall += 0.05;
                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight) {
                    hitWall = true;
                    distanceToWall = depth;
                } else {
                    if (map[testY * mapWidth + testX] == '#') {
                        hitWall = true;
                    }
                }
            }

            int ceiling = (double)(screenHeight / 2.0) - screenHeight / ((double)distanceToWall);
            int floor = screenHeight - ceiling;

            char wallShade;
            if (distanceToWall <= depth / 4.0) wallShade = '#';
            else if (distanceToWall < depth / 3.0) wallShade = 'O';
            else if (distanceToWall < depth / 2.0) wallShade = 'o';
            else if (distanceToWall < depth) wallShade = '.';
            else wallShade = ' ';

            for (int y = 0; y < screenHeight; y++) {
                if (y < ceiling) screen[y * screenWidth + x] = ' ';
                else if (y > ceiling && y <= floor) screen[y * screenWidth + x] = wallShade;
                else screen[y * screenWidth + x] = ' ';
            }
        }

        // Clear screen
        std::cout << "\033[H";

        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                std::cout << screen[y * screenWidth + x];
            }
            std::cout << "\n";
        }

        // Controls
        if (kbhit()) {
            char c = getch();
            if (c == 'a') playerAngle -= 0.1;
            if (c == 'd') playerAngle += 0.1;
            if (c == 'w') {
                playerX += sin(playerAngle) * speed;
                playerY += cos(playerAngle) * speed;
            }
            if (c == 's') {
                playerX -= sin(playerAngle) * speed;
                playerY -= cos(playerAngle) * speed;
            }
            if (c == 'q') break; // Quit
        }

        usleep(30000); // 30ms delay for smooth rendering
    }

    return 0;
}
