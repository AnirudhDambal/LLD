#include<bits/stdc++.h>
using namespace std;


 enum class OperationType{
    WITHDRAW_CASH,
    DEPOSITE_CASH,
    CHECH_BALANCE 
};
class Card{
    private:
        string cardNumber;
        string pin;
    public :
        Card(string crdNo , string p){
            this->cardNumber = crdNo;
            pin = p;
        }

        void setPin(string pin){
            this->pin = pin;
        }

        string getPin(){
            return pin;
        }

        string getCardNumber(){
            return cardNumber;
        }
};


class Account{
    unordered_map<string , Card *> cards;
    double balance;
    string accountNumber;
    public :
        Account(double balance , string accNo){
            balance = 0;
            accountNumber = accNo;
        }

        string getAccountNumber(){
            return accountNumber;
        }


        void depositeMoney(double amount){
            balance += amount;
        }


        bool withdrawMoney(double amount ){
            if(balance >= amount){
                balance -= amount;
                return true;
            }
            else{
                return false;
            }
        }

        double getBalance(){
            return balance;
        }  
};

class BankService{
    unordered_map<string , Card *> cards;
    unordered_map<string , Account *> accounts;
    unordered_map<Card * , Account *> cardtoAccount;
    public :
        BankService(){
            
        }
        void linkCardToAccount(Card *card , Account *account){
            cards[card->getCardNumber()] = card;
            accounts[account->getAccountNumber()] = account;
            cardtoAccount[card] = account;
        }

        double getBalance(Card * card){
            Account *account = cardtoAccount[card];
            return account->getBalance();
        }

        void depositeMoney(Card *card , double amount){
            Account *account = cardtoAccount[card];
            account->depositeMoney(amount , cardNumber , pin);
        }

        Account * createAccount(string accountNumber , double balance = 0){
            Account *account = new Account(balance , accountNumber);
            return account;
        }

        Card * createCard(string cardNumber , string pin){
            Card *card = new Card(cardNumber , pin);
            return card;
        }

        void withdrawMoney(Card *card , double amount){
            Card *card = cards[cardNumber];
            Account *account = cardtoAccount[card];
            account->withdrawMoney(amount);
        }

        bool authenticate(Card *card , string pin)
        {
            if(card->getPin() == pin)
                return true;
            else
                return false;
        }


};