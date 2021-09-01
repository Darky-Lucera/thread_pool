#include <cstdio>
#include <chrono>
#include "thread_pool/thread_pool.h"

#if __cplusplus >= 201703L
    using namespace std::chrono_literals;
#else
constexpr std::chrono::milliseconds operator"" ms(unsigned long long value) noexcept {
    return std::chrono::milliseconds(value);
}
#endif

int main() {
    thread_pool         pool;

    auto work = [](const char *name) {
        std::this_thread::sleep_for(500ms);
        printf("Hello %s!\n", name);
    };

    auto task = [](int a, int b) {
        std::this_thread::sleep_for(500ms);
        return a + b;
    };

    auto result = pool.enqueue_task(task, 2, 3);
    pool.enqueue_work(work, "work");

    printf("Empty: %d - Size: %d\n", int(pool.empty()), int(pool.size()));
    printf("Result: %d\n", result.get());
    printf("Empty: %d - Size: %d\n", int(pool.empty()), int(pool.size()));

    return 0;
}
