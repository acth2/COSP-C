#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>

class MinimizedPosInt {
public:
    MinimizedPosInt() {
        sharedInt = std::make_shared<int>(75);
    }

    int getValue() {
        std::lock_guard<std::mutex> lock(mtx);
        return *sharedInt;
    }

    void setValue(int newValue) {
        std::lock_guard<std::mutex> lock(mtx);
        if (newValue <= 0) {
            throw std::invalid_argument("New value must be a positive integer.");
        }
        *sharedInt = newValue;
    }

private:
    std::shared_ptr<int> sharedInt;
    std::mutex mtx;
};
