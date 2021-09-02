#include <cstdio>
#include <chrono>
#include "thread_pool/thread_pool.h"

constexpr std::chrono::milliseconds operator"" _ms(unsigned long long value) noexcept {
    return std::chrono::milliseconds(value);
}

int main() {
    thread_pool         pool;

    auto work = [](const char *name) {
        std::this_thread::sleep_for(500_ms);
        printf("Hello %s!\n", name);
    };

    auto task = [](int a, int b) {
        std::this_thread::sleep_for(500_ms);
        return a + b;
    };

    auto result = pool.enqueue_task(task, 2, 3);
    pool.enqueue_work(work, "work");

    printf("Empty: %d - Size: %d\n", int(pool.empty()), int(pool.size()));
    printf("Result: %d\n", result.get());
    printf("Empty: %d - Size: %d\n", int(pool.empty()), int(pool.size()));

    return 0;
}
