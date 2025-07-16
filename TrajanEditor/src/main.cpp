#include <chrono>
#include <iostream>
#include <ostream>

#include <Log.hpp>


int main(void) {

    Trajan::Initialize();
    Trajan::CreateWindow(800, 600, "Trajan Editor");

    float dt = 0.0f;
    while(!Trajan::ShouldClose()) {
        auto start = std::chrono::high_resolution_clock::now();

        Trajan::Update(dt);

        auto stop = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start).count();
    }

    Trajan::Shutdown();

    return 0;
}