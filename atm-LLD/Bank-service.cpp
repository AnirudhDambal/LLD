#include "include/BankService.h"
#include "include/Account.h"
#include "include/Card.h"

#include <stdexcept>

// Simple implementations - ownership is intentionally naive for the demo
BankService::BankService() {}
BankService::~BankService() {
    for (auto &p : cardsByNumber) delete p.second;
    for (auto &p : accountsByNumber) delete p.second;
}

Account* BankService::createAccount(const std::string &accountNumber, double balance) {
    if (accountsByNumber.count(accountNumber)) throw std::runtime_error("Account already exists");
    Account *a = new Account(accountNumber, balance);
    accountsByNumber[accountNumber] = a;
    return a;
}

Card* BankService::createCard(const std::string &cardNumber, const std::string &pin) {
    if (cardsByNumber.count(cardNumber)) throw std::runtime_error("Card already exists");
    Card *c = new Card(cardNumber, pin);
    cardsByNumber[cardNumber] = c;
    return c;
}

void BankService::linkCardToAccount(Card *card, Account *account) {
    if (!card || !account) return;
    cardsByNumber[card->getCardNumber()] = card;
    accountsByNumber[account->getAccountNumber()] = account;
    cardToAccount[card->getCardNumber()] = account;
    account->linkCard(card);
}

bool BankService::authenticate(const std::string &cardNumber, const std::string &pin) const {
    auto it = cardsByNumber.find(cardNumber);
    if (it == cardsByNumber.end()) return false;
    return it->second->getPin() == pin;
}

double BankService::getBalance(const std::string &cardNumber) const {
    auto it = cardToAccount.find(cardNumber);
    if (it == cardToAccount.end()) throw std::runtime_error("Card not linked to account");
    return it->second->getBalance();
}

bool BankService::deposit(const std::string &cardNumber, double amount) {
    auto it = cardToAccount.find(cardNumber);
    if (it == cardToAccount.end()) return false;
    it->second->deposit(amount);
    return true;
}

bool BankService::withdraw(const std::string &cardNumber, double amount) {
    auto it = cardToAccount.find(cardNumber);
    if (it == cardToAccount.end()) return false;
    return it->second->withdraw(amount);
}
