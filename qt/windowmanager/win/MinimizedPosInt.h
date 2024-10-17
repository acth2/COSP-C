#ifndef MINIMIZEDPOSINT_H
#define MINIMIZEDPOSINT_H

#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>

class MinimizedPosInt {
public:
    static MinimizedPosInt& getInstance() {
        static MinimizedPosInt instance;
        return instance;
    }

    int getValue();
    void setValue(int newValue);

private:
    MinimizedPosInt() : sharedInt(std::make_shared<int>(50)) {}
    std::shared_ptr<int> sharedInt;
    std::mutex mtx;

    MinimizedPosInt(const MinimizedPosInt&) = delete;
    void operator=(const MinimizedPosInt&) = delete;
};

#endif // MINIMIZEDPOSINT_H
