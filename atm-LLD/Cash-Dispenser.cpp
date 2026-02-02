#include<bits/stdc++.h>
#include<"helper.cpp">
using namespace std;


class DispenseChain{
    public:
        virtual ~DispenseChain() {}
        virtual void setNextChain(DispenseChain* nextChain) = 0;
        virtual void dispense(int amount) = 0;


};
class NoteDispenser : public DispenseChain{
    protected :
        DispenseChain *nextChain;
        int noteValue;
        int numberOfNotes;

    public :
        NoteDispenser(int value , int numberOfNotes){
            this->noteValue = value;
            this->numberOfNotes = numberOfNotes;
            nextChain = nullptr;
        }

        void setNextChain(DispenseChain* nextChain){
            this->nextChain = nextChain;
        }


        void dispense(int amount){
            int used = min(amount/noteValue , numberOfNotes);
            int remaining = amount - used * noteValue;
            if(used > 0){
                SlipGenerator("Dispensing  " + to_string(used) + "notes of " + to_string(noteValue));
                numberOfNotes -= used;
            }

            if(remaining && nextChain != nullptr){
                nextChain->dispense(remaining);
            }
        }

};



