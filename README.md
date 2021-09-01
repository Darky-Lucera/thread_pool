# ThreadPool

This is a _C++11_ port based on the fast thread pool from Martin Vorbrodt.

From his great blog posts:
  - [Simple Thread Pool](https://vorbrodt.blog/2019/02/12/simple-thread-pool/)
  - [Advanced Thread Pool](https://vorbrodt.blog/2019/02/27/advanced-thread-pool/)

## How to use

Just drop src/threadpool to your project

## Example

```cpp
thread_pool         pool;

auto work = [](const char *name) {
    printf("Hello %s!\n", name);
};

auto task = [](int a, int b) {
    return a + b;
};

pool.enqueue_work(work, "work");
auto result = pool.enqueue_task(task, 2, 3);
printf("Result: %d\n", result.get());
```

Note that _work_ is 5-10x faster than _task_.
