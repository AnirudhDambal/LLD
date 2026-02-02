#include "include/ATM.h"
#include "include/ATMState.h"
#include "include/Card.h"
#include "include/BankService.h"
#include "include/DispenseChain.h"
#include "include/NoteDispenser.h"
#include "include/SlipGenerator.h"

#include <iostream>
#include <algorithm>

using namespace std;

class NoCardState;
class HasCardState;
class AuthenticatedState;
class OutOfCashState;
std::vector<DispenseChain*> buildDefaultChain();
void destroyChain(std::vector<DispenseChain*> &chain);
void dispenseAmount(std::vector<DispenseChain*> &chain, int amount);

static int computeChainCash(const std::vector<DispenseChain*> &chain) {
    int total = 0;
    for (auto *d : chain) {
        // NoteDispenser exposes getNoteValue/getRemaining
        NoteDispenser *n = dynamic_cast<NoteDispenser*>(d);
        if (n) total += n->getNoteValue() * n->getRemaining();
    }
    return total;
}


class NoCardState : public ATMState {
public:
    void insertCard(ATM &atm, Card *card) override {
        atm.clearCurrentCard();
        atm.setState(atm.getHasCardState());
        atm.insertCard(card); // delegate into new state for processing
    }
    void ejectCard(ATM &atm) override { SlipGenerator::print("No card to eject"); }
    void enterPin(ATM &atm, const std::string &pin) override { SlipGenerator::print("Insert card first"); }
    void requestWithdrawal(ATM &atm, int) override { SlipGenerator::print("Insert card first"); }
    void depositCash(ATM &atm, double) override { SlipGenerator::print("Insert card first"); }
    void checkBalance(ATM &atm) override { SlipGenerator::print("Insert card first"); }
    void refillCash(ATM &atm, int amount) override {
        // maintenance insertion of cash is allowed
        for (auto *d : atm.getDispenseChain()) {
            NoteDispenser *n = dynamic_cast<NoteDispenser*>(d);
            if (n) {
                // naive: put all refill into smallest notes
            }
        }
        SlipGenerator::print("Refilled " + to_string(amount) + " (no-op distribution)");
        if (atm.getAvailableCash() > 0) atm.setState(atm.getNoCardState());
    }
};

class HasCardState : public ATMState {
public:
    void insertCard(ATM &atm, Card *card) override {
        if (atm.getCurrentCard()) {
            SlipGenerator::print("Card already inserted");
            return;
        }
        atm.setCurrentCard(card);
        SlipGenerator::print(std::string("Card inserted: ") + card->getCardNumber());
        atm.setState(atm.getHasCardState());
    }
    void ejectCard(ATM &atm) override {
        SlipGenerator::print("Ejecting card");
        atm.clearCurrentCard();
        atm.setState(atm.getNoCardState());
    }
    void enterPin(ATM &atm, const std::string &pin) override {
        Card *c = atm.getCurrentCard();
        if (!c) { SlipGenerator::print("No card"); return; }
        if (atm.getBankService()->authenticate(c->getCardNumber(), pin)) {
            SlipGenerator::print("PIN correct");
            atm.setState(atm.getAuthenticatedState());
            atm.resetPinAttempts();
        } else {
            SlipGenerator::print("Invalid PIN");
            atm.incrementPinAttempts();
            if (atm.getPinAttempts() >= 3) {
                SlipGenerator::print("Card blocked due to 3 failed attempts");
                atm.ejectCard();
            }
        }
    }
    void requestWithdrawal(ATM &atm, int) override { SlipGenerator::print("Enter PIN first"); }
    void depositCash(ATM &atm, double amount) override {
        Card *c = atm.getCurrentCard();
        if (!c) { SlipGenerator::print("No card"); return; }
        atm.getBankService()->deposit(c->getCardNumber(), amount);
        SlipGenerator::print("Deposit successful");
        atm.ejectCard();
    }
    void checkBalance(ATM &atm) override {
        auto *c = atm.getCurrentCard();
        if (!c) { SlipGenerator::print("No card"); return; }
        double bal = atm.getBankService()->getBalance(c->getCardNumber());
        SlipGenerator::print("Balance: " + to_string(bal));
    }
    void refillCash(ATM &atm, int amount) override {
        // allow maintenance in this state too
        SlipGenerator::print("Refill requested");
        if (atm.getAvailableCash() > 0) atm.setState(atm.getNoCardState());
    }
};

class AuthenticatedState : public ATMState {
public:
    void insertCard(ATM &atm, Card *card) override { SlipGenerator::print("Transaction in progress, eject current card first"); }
    void ejectCard(ATM &atm) override {
        SlipGenerator::print("Ejecting card");
        atm.clearCurrentCard();
        atm.setState(atm.getNoCardState());
    }
    void enterPin(ATM &atm, const std::string &) override { SlipGenerator::print("Already authenticated"); }
    void requestWithdrawal(ATM &atm, int amount) override {
        Card *c = atm.getCurrentCard();
        if (!c) { SlipGenerator::print("No card"); atm.setState(atm.getNoCardState()); return; }
        int availableATM = atm.getAvailableCash();
        if (amount > availableATM) {
            SlipGenerator::print("Insufficient cash in ATM");
            return;
        }
        // ask bank to withdraw
        if (!atm.getBankService()->withdraw(c->getCardNumber(), amount)) {
            SlipGenerator::print("Insufficient funds in account");
            return;
        }
        // dispense
        dispenseAmount(atm.getDispenseChain(), amount);
        if (atm.getAvailableCash() <= 0) {
            SlipGenerator::print("ATM out of cash");
            atm.setState(atm.getOutOfCashState());
        }
        // after transaction eject
        ejectCard(atm);
    }
    void depositCash(ATM &atm, double amount) override {
        SlipGenerator::print("Use deposit envelope or teller - not supported");
    }
    void checkBalance(ATM &atm) override {
        auto *c = atm.getCurrentCard();
        if (!c) return;
        double bal = atm.getBankService()->getBalance(c->getCardNumber());
        SlipGenerator::print("Balance: " + to_string(bal));
    }
    void refillCash(ATM &atm, int) override { SlipGenerator::print("Refill requested"); }
};

class OutOfCashState : public ATMState {
public:
    void insertCard(ATM &atm, Card *card) override { SlipGenerator::print("ATM out of cash"); }
    void ejectCard(ATM &atm) override { SlipGenerator::print("No card to eject"); }
    void enterPin(ATM &atm, const std::string &) override { SlipGenerator::print("ATM out of cash"); }
    void requestWithdrawal(ATM &atm, int) override { SlipGenerator::print("ATM out of cash"); }
    void depositCash(ATM &atm, double) override { SlipGenerator::print("ATM out of cash"); }
    void checkBalance(ATM &atm) override {
        SlipGenerator::print("ATM out of cash - check balance at bank");
        if (atm.getCurrentCard()) {
            double bal = atm.getBankService()->getBalance(atm.getCurrentCard()->getCardNumber());
            SlipGenerator::print("Balance: " + to_string(bal));
        }
    }
    void refillCash(ATM &atm, int amount) override {
        // naive: top up smallest notes
        SlipGenerator::print("Refilling ATM with " + to_string(amount));
        // For demo: put all amount into last dispenser
        auto &chain = atm.getDispenseChain();
        if (!chain.empty()) {
            NoteDispenser *n = dynamic_cast<NoteDispenser*>(chain.back());
            if (n) {
                // add quantity approximately
                int addQty = amount / n->getNoteValue();
                // we can't modify private quantity; in demo we assume ability to increase via hack (not ideal)
            }
        }
        if (atm.getAvailableCash() > 0) atm.setState(atm.getNoCardState());
    }
};

////////////////////////////////////////////////////
// ATM implementation
////////////////////////////////////////////////////

ATM::ATM(BankService *bank, std::vector<DispenseChain*> cashChain)
    : bankService(bank), cashChain(std::move(cashChain)) {
    noCardState = new NoCardState();
    hasCardState = new HasCardState();
    authenticatedState = new AuthenticatedState();
    outOfCashState = new OutOfCashState();

    if (getAvailableCash() <= 0) currentState = outOfCashState;
    else currentState = noCardState;
}

ATM::~ATM() {
    delete noCardState;
    delete hasCardState;
    delete authenticatedState;
    delete outOfCashState;
    // destruct chain items
    destroyChain(cashChain);
}

void ATM::insertCard(Card *card) {
    if (!card) { SlipGenerator::print("Invalid card"); return; }
    if (currentState == noCardState) {
        currentCard = card;
        pinAttempts = 0;
        setState(hasCardState);
        SlipGenerator::print("Card inserted: " + card->getCardNumber());
        return;
    }
    currentState->insertCard(*this, card);
}

void ATM::ejectCard() { currentState->ejectCard(*this); }

void ATM::enterPin(const std::string &pin) { currentState->enterPin(*this, pin); }

void ATM::requestWithdrawal(int amount) { currentState->requestWithdrawal(*this, amount); }

void ATM::depositCash(double amount) { currentState->depositCash(*this, amount); }

void ATM::checkBalance() { currentState->checkBalance(*this); }

void ATM::refillCash(int amount) { currentState->refillCash(*this, amount); }

int ATM::getAvailableCash() const { return computeChainCash(cashChain); }

void ATM::setState(ATMState *s) { currentState = s; }
BankService *ATM::getBankService() const { return bankService; }
Card *ATM::getCurrentCard() const { return currentCard; }
void ATM::setCurrentCard(Card *c) { currentCard = c; resetPinAttempts(); }
void ATM::clearCurrentCard() { currentCard = nullptr; resetPinAttempts(); }
std::vector<DispenseChain*> &ATM::getDispenseChain() { return cashChain; }

ATMState *ATM::getNoCardState() const { return noCardState; }
ATMState *ATM::getHasCardState() const { return hasCardState; }
ATMState *ATM::getAuthenticatedState() const { return authenticatedState; }
ATMState *ATM::getOutOfCashState() const { return outOfCashState; }

void ATM::incrementPinAttempts() { ++pinAttempts; }
void ATM::resetPinAttempts() { pinAttempts = 0; }
int ATM::getPinAttempts() const { return pinAttempts; }

