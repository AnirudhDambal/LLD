#pragma once

class ATM;
class Card;

class ATMState {
public:
    virtual ~ATMState() = default;
    virtual void insertCard(ATM &atm, Card *card) = 0;
    virtual void ejectCard(ATM &atm) = 0;
    virtual void enterPin(ATM &atm, const std::string &pin) = 0;
    virtual void requestWithdrawal(ATM &atm, int amount) = 0;
    virtual void depositCash(ATM &atm, double amount) = 0;
    virtual void checkBalance(ATM &atm) = 0;
    virtual void refillCash(ATM &atm, int amount) = 0;
};
