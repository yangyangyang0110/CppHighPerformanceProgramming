#include "helper/buffer.hpp"
#include "helper/logger.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

/**
 * @brief 模拟go-channel建立一个素数筛选器. 启动3个线程, 分别进行生成自然数, 素数过滤器, 输出到标准输出的工作.
 */

using String = std::string;
using Thread = std::thread;

template <typename T>
using Atomic = std::atomic<T>;

inline bool isPrime(int n) noexcept {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

static inline int getNextNatureNumber() noexcept {
    static int gNatureNumber = 0;
    return gNatureNumber++;
}

class Worker {
public:
    Worker() noexcept
        : naturalNumberQueue_(3)
        , primeNumberQueue_(2) {
        genNatureNumberThread_ = Thread([this]() {
            while (genNatureNumberAtomic_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                auto natureNumber = getNextNatureNumber();
                LOG_DEBUG("> NatureNumber: {}", natureNumber);
                naturalNumberQueue_.wait_and_push(natureNumber);
            }
        });

        getPrimeNumberThread_ = Thread([this]() {
            while (getPrimeNumberAtomic_) {
                auto number = naturalNumberQueue_.wait_and_pop();
                if (isPrime(number)) {
                    String str = "Prime Number: " + std::to_string(number);
                    LOG_DEBUG(">> {}", str);
                    primeNumberQueue_.wait_and_push(std::move(str));
                }
            }
        });
    }

    ~Worker() noexcept {
        Stop();
        if (genNatureNumberThread_.joinable()) {
            genNatureNumberThread_.join();
        }

        if (getPrimeNumberThread_.joinable()) {
            getPrimeNumberThread_.join();
        }
    }

    void Stop() const noexcept {
        genNatureNumberAtomic_.store(false);
        getPrimeNumberAtomic_.store(false);
    }

    void DisplayPrime() noexcept {
        auto primeStr = primeNumberQueue_.wait_and_pop();
        LOG_INFO(">>> Displaying prime: {}", primeStr);
    }

private:
    mutable Atomic<bool> genNatureNumberAtomic_{true}, getPrimeNumberAtomic_{true};
    Thread genNatureNumberThread_, getPrimeNumberThread_;

    ThreadSafeQueue<int> naturalNumberQueue_;
    ThreadSafeQueue<String> primeNumberQueue_;
};

int main() {

    // Initialize logger.
    helper::logger::LoggerParam logParam;
    {
        logParam.logLevel = helper::logger::LogLevel::DEBUG;
        logParam.maxFileSize = logParam.maxRetentionDays = 0;
        logParam.logDir = logParam.logName = "";
    }
    helper::logger::InitializeGlobalLogger(std::move(logParam));

    Worker worker;

    for (int i = 0; i < 10; ++i) {
        worker.DisplayPrime();
    }

    std::cout << " Bye~" << std::endl;

    return 0;
}
