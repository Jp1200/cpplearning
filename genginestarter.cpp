#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;
const float FOV = 3.14159 / 3.0; // 60 degrees
const float DEPTH = 16.0f;

struct Player {
    float x = 8.0f;
    float y = 8.0f;
    float angle = 0.0f;
    float moveSpeed = 5.0f;
    float rotSpeed = 3.0f;
};

std::vector<std::string> map = {
    "################",
    "#..............#",
    "#.......########",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "###............#",
    "##.............#",
    "#......##......#",
    "#......##......#",
    "#..............#",
    "#..............#",
    "#..............#",
    "#..............#",
    "################"
};

// Load a texture
SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* renderer) {
    SDL_Surface* surf = IMG_Load(file.c_str());
    if (!surf) {
        std::cout << "Failed to load texture: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;

    SDL_Window* window = SDL_CreateWindow("SDL2 Raycaster",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Player player;

    SDL_Texture* wallTexture = loadTexture("wall.png", renderer);
    SDL_Texture* floorTexture = loadTexture("floor.png", renderer);
    SDL_Texture* ceilingTexture = loadTexture("ceiling.png", renderer);

    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_W]) {
            player.x += sin(player.angle) * player.moveSpeed * deltaTime;
            player.y += cos(player.angle) * player.moveSpeed * deltaTime;
            if (map[int(player.y)][int(player.x)] == '#') {
                player.x -= sin(player.angle) * player.moveSpeed * deltaTime;
                player.y -= cos(player.angle) * player.moveSpeed * deltaTime;
            }
        }
        if (keystates[SDL_SCANCODE_S]) {
            player.x -= sin(player.angle) * player.moveSpeed * deltaTime;
            player.y -= cos(player.angle) * player.moveSpeed * deltaTime;
            if (map[int(player.y)][int(player.x)] == '#') {
                player.x += sin(player.angle) * player.moveSpeed * deltaTime;
                player.y += cos(player.angle) * player.moveSpeed * deltaTime;
            }
        }
        if (keystates[SDL_SCANCODE_A]) player.angle -= player.rotSpeed * deltaTime;
        if (keystates[SDL_SCANCODE_D]) player.angle += player.rotSpeed * deltaTime;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Raycasting
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            float rayAngle = (player.angle - FOV / 2.0f) + ((float)x / SCREEN_WIDTH) * FOV;
            float distanceToWall = 0.0f;
            bool hitWall = false;
            float eyeX = sin(rayAngle);
            float eyeY = cos(rayAngle);

            int texX = 0;

            while (!hitWall && distanceToWall < DEPTH) {
                distanceToWall += 0.05f;
                int testX = int(player.x + eyeX * distanceToWall);
                int testY = int(player.y + eyeY * distanceToWall);

                if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT) {
                    hitWall = true;
                    distanceToWall = DEPTH;
                } else if (map[testY][testX] == '#') {
                    hitWall = true;
                    float hitX = player.x + eyeX * distanceToWall;
                    float hitY = player.y + eyeY * distanceToWall;
                    float wallHit = (hitX - floor(hitX) + hitY - floor(hitY)) / 2.0f;
                    texX = int(wallHit * 64) % 64; // assuming 64px texture
                }
            }

            int ceiling = (SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / distanceToWall;
            int floor = SCREEN_HEIGHT - ceiling;

            // Draw wall slice
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                if (y < ceiling) {
                    // Ceiling texture
                    float sampleY = float(y) / ceiling * 64;
                    SDL_Rect src = { int(sampleY) % 64, 0, 1, 1 };
                    SDL_Rect dst = { x, y, 1, 1 };
                    SDL_RenderCopy(renderer, ceilingTexture, &src, &dst);
                } else if (y >= ceiling && y <= floor) {
                    // Wall
                    float sampleY = float(y - ceiling) / (floor - ceiling) * 64;
                    SDL_Rect src = { texX, int(sampleY) % 64, 1, 1 };
                    SDL_Rect dst = { x, y, 1, 1 };
                    SDL_RenderCopy(renderer, wallTexture, &src, &dst);
                } else {
                    // Floor texture
                    float sampleY = float(SCREEN_HEIGHT - y) / (SCREEN_HEIGHT - floor) * 64;
                    SDL_Rect src = { int(sampleY) % 64, 0, 1, 1 };
                    SDL_Rect dst = { x, y, 1, 1 };
                    SDL_RenderCopy(renderer, floorTexture, &src, &dst);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(wallTexture);
    SDL_DestroyTexture(floorTexture);
    SDL_DestroyTexture(ceilingTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
