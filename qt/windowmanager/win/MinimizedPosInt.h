#ifndef MINIMIZEDPOSINT_H
#define MINIMIZEDPOSINT_H

#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>

class MinimizedPosInt {
public:
    MinimizedPosInt();

    int getValue();

    void setValue(int newValue);

private:
    std::shared_ptr<int> sharedInt;
    std::mutex mtx;
};

#endif // MINIMIZEDPOSINT_H
