#include "include/BankService.h"
#include "include/Card.h"
#include "include/Account.h"
#include "include/DispenseChain.h"
#include "include/ATM.h"

#include <iostream>
#include <vector>

// helpers from Cash-Dispenser.cpp
std::vector<DispenseChain*> buildDefaultChain();
void destroyChain(std::vector<DispenseChain*> &chain);

int main() {
    BankService bank;
    // Demo data: one account and card
    Account *acc = bank.createAccount("ACC123", 500);
    Card *card = bank.createCard("CARD-0001", "1234");
    bank.linkCardToAccount(card, acc);
    auto chain = buildDefaultChain();
    ATM atm(&bank, chain);

    std::cout << "\n==== Welcome to the ATM ====" << std::endl;
    bool running = true;
    bool cardInserted = false;
    while (running) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Insert Card\n2. Enter PIN\n3. Check Balance\n4. Withdraw Cash\n5. Exit\nChoice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(1000, '\n');
        switch (choice) {
            case 1: {
                if (cardInserted) {
                    std::cout << "A card is already inserted.\n";
                } else {
                    atm.insertCard(card);
                    cardInserted = true;
                }
                break;
            }
            case 2: {
                if (!cardInserted) {
                    std::cout << "Insert card first.\n";
                } else {
                    std::string pin;
                    std::cout << "Enter PIN: ";
                    std::cin >> pin;
                    atm.enterPin(pin);
                }
                break;
            }
            case 3: {
                atm.checkBalance();
                break;
            }
            case 4: {
                int amt;
                std::cout << "Enter amount to withdraw: ";
                std::cin >> amt;
                atm.requestWithdrawal(amt);
                break;
            }
            case 5: {
                std::cout << "Thank you for using the ATM!\n";
                running = false;
                break;
            }
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
