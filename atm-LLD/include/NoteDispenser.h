#pragma once
#include "DispenseChain.h"
#include <algorithm>
#include <iostream>

class NoteDispenser : public DispenseChain {
public:
    NoteDispenser(int noteValue, int quantity);
    void setNext(DispenseChain *next) override;
    void dispense(int amount) override;
    int getNoteValue() const;
    int getRemaining() const;
private:
    DispenseChain *next{nullptr};
    int noteValue{0};
    int quantity{0};
};
