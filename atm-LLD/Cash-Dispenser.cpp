#include "include/DispenseChain.h"
#include "include/NoteDispenser.h"
#include "include/SlipGenerator.h"
#include <vector>

// Backwards-compatible helpers used by demo
std::vector<DispenseChain*> buildDefaultChain() {
    std::vector<DispenseChain*> chain;
    chain.push_back(new NoteDispenser(100, 10));
    chain.push_back(new NoteDispenser(50, 10));
    chain.push_back(new NoteDispenser(20, 20));

    for (size_t i = 0; i + 1 < chain.size(); ++i) chain[i]->setNext(chain[i + 1]);
    return chain;
}

void destroyChain(std::vector<DispenseChain*> &chain) {
    for (auto *p : chain) delete p;
    chain.clear();
}

void dispenseAmount(std::vector<DispenseChain*> &chain, int amount) {
    if (chain.empty()) return;
    chain[0]->dispense(amount);
}



