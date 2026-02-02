#pragma once
#include <string>
#include <unordered_map>

class Account;
class Card;

class BankService {
public:
    BankService();
    ~BankService();

    Account* createAccount(const std::string &accountNumber, double balance = 0.0);
    Card* createCard(const std::string &cardNumber, const std::string &pin);

    void linkCardToAccount(Card *card, Account *account);

    // operations by card number
    bool authenticate(const std::string &cardNumber, const std::string &pin) const;
    double getBalance(const std::string &cardNumber) const;
    bool deposit(const std::string &cardNumber, double amount);
    bool withdraw(const std::string &cardNumber, double amount);

private:
    std::unordered_map<std::string, Card*> cardsByNumber;
    std::unordered_map<std::string, Account*> accountsByNumber;
    std::unordered_map<std::string, Account*> cardToAccount; // map cardNumber -> Account*
};
