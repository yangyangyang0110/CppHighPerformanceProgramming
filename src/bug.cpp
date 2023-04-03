/* Proj: CppHighPerformanceProgramming
 * File: bug.cpp
 * Created Date: 2023/2/12
 * Author: yangyangyang
 * Description:
 * -----
 * Last Modified: 2023/2/12 18:38:02
 * -----
 * Copyright (c) 2023  . All rights reserved.
 */

#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <thread>

/**
 * 模拟Sleep
 */

using namespace std::chrono;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
using std::thread;

int randomInt(int min = 0, int max = INT_MAX) {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    std::uniform_int_distribution dist6(min, max);
    while (true) {
        const auto value = dist6(rng);
        if (value > min && value < max) {
            return value;
        }
    }
}

std::string randomString(int size = 6) {
    stringstream ss;
    for (int i = 0; i < size; ++i)
        ss << char('a' + randomInt(0, 25));
    return ss.str();
}

template <typename Rep, typename Period>
void sleep(const std::chrono::duration<Rep, Period>& timeout) {
    std::this_thread::sleep_for(timeout);
}

void test() {
    map<int, std::string> data;

    auto random1000 = [maxValue = 100000]() { return randomInt(0, maxValue); };

    cout << INT_MAX << endl;

    thread([&]() {
        for (int i = 0;; ++i) {
            data[random1000()] = randomString();
            printf("<<< size: %lu >>>\n", data.size());
            // sleep(500ms);
        }
    }).join();
}

int main() {
    test();
    return 0;
}
