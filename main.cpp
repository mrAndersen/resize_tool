#include <vips/vips8>
#include <chrono>
#include <string>
#include <iostream>
#include <filesystem>
#include <thread>
#include <deque>
#include <queue>
#include <mutex>

#define START_TIME auto startTime = std::chrono::high_resolution_clock::now()
#define END_TIME_NS std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startTime).count()
#define END_TIME_MS ((double)(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startTime).count()) / 1000000)
#define END_TIME_S std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count()


int main() {
    VIPS_INIT("");

    auto thumb_w = 400;

    std::mutex p_m;
    std::mutex r_m;

    int max_threads = 10;
    int current_threads = 0;

    int resized = 0;
    int total = 0;

    std::string originalsPath = "../originals";
    std::queue<std::string> paths;

    for (const auto &entry : std::filesystem::directory_iterator(originalsPath)) {
        std::string p = entry.path();
        auto filename = p.substr(p.find_last_of("/\\") + 1);

        paths.push(filename);
    }

    total = paths.size();
    printf("got %d files\n", total);
    auto pstart = std::chrono::high_resolution_clock::now();

    while (resized < total) {
        if (current_threads < max_threads) {
            current_threads++;

            std::thread t([&]() -> void {
                std::string filename;

                if (!paths.empty()) {
                    p_m.lock();
                    filename = paths.front();
                    paths.pop();
                    p_m.unlock();
                } else {
                    r_m.lock();
                    current_threads--;
                    r_m.unlock();
                    return;
                }

                std::string new_i = std::string("../resizes/") + filename;
                std::string old_i = originalsPath + "/" + filename;

                auto image = vips::VImage::new_from_file(old_i.c_str());
                auto scale = (float) 1 / ((float) image.width() / (float) thumb_w);

                START_TIME;
                image.resize(scale).pngsave(const_cast<char *>(new_i.c_str()));

                r_m.lock();
                resized++;
                current_threads--;
                printf("Done %s -- %.2f ms\n", new_i.c_str(), END_TIME_MS);
                r_m.unlock();
            });

            t.detach();
        }
    }

    auto p_end = ((double) (std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - pstart).count()) / 1000000);

    printf("Done in %.2f ms\n", p_end);
}
