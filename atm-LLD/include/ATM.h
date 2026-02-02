#pragma once
#include <vector>
#include <memory>
#include <string>

class ATMState;
class Card;
class BankService;
class DispenseChain;

class ATM {
public:
    ATM(BankService *bank, std::vector<DispenseChain*> cashChain);
    ~ATM();

    // user interactions
    void insertCard(Card *card);
    void ejectCard();
    void enterPin(const std::string &pin);
    void requestWithdrawal(int amount);
    void depositCash(double amount);
    void checkBalance();

    // maintenance
    void refillCash(int amount);
    int getAvailableCash() const;

    // internal helpers (made public so states can interact)
    void setState(ATMState *s);
    BankService *getBankService() const;
    Card *getCurrentCard() const;
    void setCurrentCard(Card *c);
    void clearCurrentCard();
    std::vector<DispenseChain*> &getDispenseChain();

    // state getters for concrete state classes
    ATMState *getNoCardState() const;
    ATMState *getHasCardState() const;
    ATMState *getAuthenticatedState() const;
    ATMState *getOutOfCashState() const;

    // pin attempt helpers
    void incrementPinAttempts();
    void resetPinAttempts();
    int getPinAttempts() const;

private:
    ATMState *noCardState{nullptr};
    ATMState *hasCardState{nullptr};
    ATMState *authenticatedState{nullptr};
    ATMState *outOfCashState{nullptr};

    ATMState *currentState{nullptr};

    BankService *bankService{nullptr};
    Card *currentCard{nullptr};
    int pinAttempts{0};

    std::vector<DispenseChain*> cashChain;
};
