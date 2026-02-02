#include "include/BankService.h"
#include "include/Card.h"
#include "include/Account.h"
#include "include/DispenseChain.h"

#include <iostream>
#include <vector>

// helpers from Cash-Dispenser.cpp
std::vector<DispenseChain*> buildDefaultChain();
void destroyChain(std::vector<DispenseChain*> &chain);
void dispenseAmount(std::vector<DispenseChain*> &chain, int amount);

int main() {
    BankService bank;

    // create account and card
    Account *acc = bank.createAccount("ACC123", 500);
    Card *card = bank.createCard("CARD-0001", "1234");
    bank.linkCardToAccount(card, acc);

    std::cout << "Initial balance: " << bank.getBalance(card->getCardNumber()) << "\n";

    // deposit
    bank.deposit(card->getCardNumber(), 250);
    std::cout << "After deposit: " << bank.getBalance(card->getCardNumber()) << "\n";

    // withdraw via bank
    if (bank.withdraw(card->getCardNumber(), 200))
        std::cout << "Withdrawn 200 successfully. Balance: " << bank.getBalance(card->getCardNumber()) << "\n";
    else
        std::cout << "Withdraw of 200 failed\n";

    // cash dispenser: demo note dispensing
    auto chain = buildDefaultChain();
    std::cout << "Dispensing 380:\n";
    dispenseAmount(chain, 380);

    destroyChain(chain);
    return 0;
}
