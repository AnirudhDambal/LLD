#pragma once

class DispenseChain {
public:
    virtual ~DispenseChain() = default;
    virtual void setNext(DispenseChain *next) = 0;
    virtual void dispense(int amount) = 0;
};
