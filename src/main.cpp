#include <iostream>
#include "game.h"

int main() {
    srand(time(NULL));
    Chunk::perlin = siv::PerlinNoise(rand());
    Chunk::perlin2 = siv::PerlinNoise(rand());
    Chunk::perlin3 = siv::PerlinNoise(rand());

    Game game(1280, 720);
    game.start();
    return 0;
}
