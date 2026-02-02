#include "include/NoteDispenser.h"
#include "include/SlipGenerator.h"

NoteDispenser::NoteDispenser(int noteValue, int quantity)
    : noteValue(noteValue), quantity(quantity) {}

void NoteDispenser::setNext(DispenseChain *next) { this->next = next; }

void NoteDispenser::dispense(int amount) {
    if (amount <= 0) return;
    int canUse = std::min(amount / noteValue, quantity);
    int remaining = amount - canUse * noteValue;
    if (canUse > 0) {
        quantity -= canUse;
        SlipGenerator::print("Dispensing " + std::to_string(canUse) + " note(s) of " + std::to_string(noteValue));
    }
    if (remaining > 0 && next) next->dispense(remaining);
}

int NoteDispenser::getNoteValue() const { return noteValue; }
int NoteDispenser::getRemaining() const { return quantity; }
