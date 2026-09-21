# C++ 线程池实现

## 项目介绍
基于 C++11 实现的线程池，支持异步任务提交和返回值获取。

## 特性
- 固定大小线程池
- 支持任意函数和参数（Lambda、函数等）
- 使用 std::future 异步获取任务返回值
- 使用 condition_variable 实现任务队列阻塞唤醒，避免忙等待
- 支持优雅关闭，析构时自动等待所有任务执行完毕

## 编译与运行
g++ -std=c++11 main.cpp -pthread -o main
./main

## 使用示例
ThreadPool pool(3);
auto res = pool.enqueue([](int a, int b) { return a + b; }, 3, 4);
cout << "任务结果：" << res.get() << endl;  // 输出 7
