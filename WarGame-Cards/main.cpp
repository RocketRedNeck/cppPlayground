// WarGame-Cards
//
// Secondo once asked how long a game of War (the card variety) could last.
// Primo was curious whether it could be modeled with software (he was itching to learn)
//
// The answers were, of course, a) it depends, and b) yes.
//
// While starting Primo with Applesoft BASIC, he eventually branched into Scratch, and C with Arduino.
// He was curious about C++ so I worked it into this example.
//
// Jump down to main(), at the bottom to see what is happening, and look at the other supporting code
// later.
//
// Run it!
//
// The output is verbose, but interesting.
//
// The number of draws to win a game has been as short as a 48 and as large as 2000+ for a single deck.
// If you increase the NUMBER_OF_DECKS constant, below, the game naturally lasts longer
//
// Enjoy.
//
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// Copyright (c) 2013 - RocketRedNeck
//
// RocketRedNeck and MIT License
//
// RocketRedNeck hereby grants license for others to copy and modify this source code for
// whatever purpose other's deem worthy as long as RocketRedNeck is given credit where
// where credit is due and you leave RocketRedNeck out of it for all other nefarious purposes.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ****************************************************************************************************

#include <stdlib.h> // for srand, rand, etc
#include <stdio.h>  // for printf etc.

#include <ctime>    // useful for seeding the random number generator

// Useful macros
#define DIM(x) (sizeof(x)/sizeof(x[0]))
#define LAST(x) (DIM(x)-1)
#define DELETE_POINTER(x) if(x!=NULL){ delete x; x = NULL;}
#define DELETE_POINTER_ARRAY(x) if (x!=NULL){ delete[] x; x=NULL;}

// How many decks to run?
unsigned int NUMBER_OF_DECKS = 1;

// Suit value is reverse alphabetical order such that spade is highest value
// for games that care.
// Value of jokers are determined by the specific game.
enum Suits
{
    SUIT_CLUB,
    SUIT_DIAMOND,
    SUIT_HEART,
    SUIT_SPADE,
    
    SUIT_JOKER
};

const char *SuitNames[] =
{
        "Clubs",
        "Diamonds",
        "Hearts",
        "Spades",
        "Joker"
};

// Rank is ace-high, and while the value may not be important
// in some games, it may have importance in others, so we
// do assign the value starting at 2.
// We define a wild-low and wild-high value to correspond to
// the selection of a wild card in the form of a joker.
enum Ranks
{
    RANK_LOW,
    
    RANK_UNUSED1,
    
    RANK_TWO,
    RANK_THREE,
    RANK_FOUR,
    RANK_FIVE,
    RANK_SIX,
    RANK_SEVEN,
    RANK_EIGHT,
    RANK_NINE,
    RANK_TEN,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE,
    
    RANK_WILD_HIGH
};

const char *RankNames[] =
{
    "LowCard",
    "Unused",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Ten",
    "Jack",
    "Queen",
    "King",
    "Ace",
    "WildCard"
};

// For the moment a card is just plain-old-data (POD)
// A card contains a suit and a rank, but we cannot
// allow the value of a card to change when in possession
// of a player. Normally this should not be a problem
// when considering that the card state is being managed
// within the application because we would control the
// references to the card. But there may be a need in
// some applications to protect the value, which would
// cause the card to be a class with private membership
// and allow the Deck to be a friend class for initialization.

// What we are finding is that a Card may actually need to
// be a node in a linked list to allow the card to be moved
// from Deck to Hand(s) to Discard or back to the Deck in
// any order
class Card
{
private:
    Suits  suit_;
    Ranks  rank_;
    
    Card *pNext_;
    Card *pPrevious_;
    
    static unsigned int masterCardCount_;  // The master count to keep track of how many cards were created
    unsigned int id_;   // An internal ID to keep track of the card creation in the deck
                        // particularly when returning a card to an empty deck.
    
public:
    
    // ---------------------------------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------------------------------
    Card(Suits aSuit, Ranks aRank)
    :pNext_(NULL),
     pPrevious_(NULL)
    {
        id_ = masterCardCount_;
        ++masterCardCount_;
        suit_ = aSuit;
        rank_ = aRank;
    }

    // ---------------------------------------------------------------------------------
    // getMasterCount - returns the total number of cards created
    // ---------------------------------------------------------------------------------
    unsigned int getMasterCount(void)
    {
        return masterCardCount_;
    }
    
    // ---------------------------------------------------------------------------------
    // getId - returns the internal ID of the card
    // ---------------------------------------------------------------------------------
    unsigned int getId(void)
    {
        return id_;
    }

    // ---------------------------------------------------------------------------------
    // getSuit - returns the suit of this card
    // ---------------------------------------------------------------------------------
    Suits getSuit(void)
    {
        return suit_;
    }
    
    // ---------------------------------------------------------------------------------
    // getRank - returns the rank of this card
    // ---------------------------------------------------------------------------------
    Ranks getRank(void)
    {
        return rank_;
    }

    // ---------------------------------------------------------------------------------
    // remove - removes this card from whatever list it is in, joining any adjecent
    // neighbors together. If this card is not added or inserted into another list
    // or is the beginning of another list it can be lost (but can be found again
    // by asking the Deck instance in which the Card was created in the first place)
    // ---------------------------------------------------------------------------------
    Card * remove(void)
    {
        if (this->pPrevious_ != NULL)
        {
            this->pPrevious_->pNext_ = this->pNext_;
        }
        if(this->pNext_ != NULL)
        {
            this->pNext_->pPrevious_ = this->pPrevious_;
        }
        this->pPrevious_ = NULL;
        this->pNext_ = NULL;
        
        return this;
    }

    // ---------------------------------------------------------------------------------
    // getFirst - traverses the list of cards from this point and returns the
    // pointer to the first card
    // ---------------------------------------------------------------------------------
    Card * getFirst(void)
    {
        Card *pThis = this;
        while (pThis->pPrevious_ != NULL)
        {
            pThis = pThis->pPrevious_;
        }
        
        return pThis;
    }

    // ---------------------------------------------------------------------------------
    // getLast - traverses the list of cards from this point and returns the
    // pointer to the last card
    // ---------------------------------------------------------------------------------
    Card * getLast(void)
    {
        Card *pThis = this;
        while (pThis->pNext_ != NULL)
        {
            pThis = pThis->pNext_;
        }
        
        return pThis;
    }

    // ---------------------------------------------------------------------------------
    // getPrevious - returns the pointer to the previous card in this list
    // ---------------------------------------------------------------------------------
    Card * getPrevious(void)
    {
        return this->pPrevious_;
    }

    // ---------------------------------------------------------------------------------
    // getNext - returns the pointer to the next card in this list
    // ---------------------------------------------------------------------------------
    Card * getNext(void)
    {
        return this->pNext_;
    }

    // ---------------------------------------------------------------------------------
    // count - counts the cards in the list
    // ---------------------------------------------------------------------------------
    unsigned int count(void)
    {
        Card *pThis = getFirst();
        unsigned int thisCount = 0;
        while (pThis != NULL)
        {
            ++thisCount;
            pThis = pThis->pNext_;
        }
        
        return thisCount;
    }
    
    // ---------------------------------------------------------------------------------
    // add - add a card to the end of a list of cards
    // NOTE: This function will traverse the list from the current card to
    // ensure the card is added to the end. Efficient enough if the current
    // card is at the end, but in some cases an insert() may be more efficient.
    // ---------------------------------------------------------------------------------
    Card * add(Card *aCard)
    {
        Card *pNext = aCard->pNext_;

        if (this != aCard)
        {
        
            // Traverse this list from this point to the end
            Card *pThis = this;
            while (pThis->pNext_ != NULL)
            {
                pThis = pThis->pNext_;
            }
            
            // Remove the card we are adding from whatever list it belonged
            aCard->remove();
            
            // Add the card to this list
            pThis->pNext_ = aCard;
            aCard->pPrevious_ = pThis;
        }
        else
        {
            pNext = pNext;
        }
        // Return the next card from the list from which we removed a card
        return pNext;
    }
    
    // ---------------------------------------------------------------------------------
    // insertAfter - insert a card into a list of cards after the current card
    // ---------------------------------------------------------------------------------
    Card * insertAfter(Card *aCard)
    {
        if (this != aCard)
        {
            // Remove the card we are inserting from whatver list it belonged
            aCard->remove();
            
            // Insert
            aCard->pNext_ = this->pNext_;
            aCard->pPrevious_ = this;
            this->pNext_ = aCard;
            
            if (aCard->pNext_ != NULL)
            {
                // This card is not at the end of the list
                aCard->pNext_->pPrevious_ = aCard;
            }
        }
        
        return aCard;
    }

    // ---------------------------------------------------------------------------------
    // insertBefore - insert a card into a list of cards before the current card
    // ---------------------------------------------------------------------------------
    Card * insertBefore(Card *aCard)
    {
        if (this != aCard)
        {
            // Remove the card we are inserting from whatver list it belonged
            aCard->remove();
            
            // Insert
            aCard->pPrevious_ = this->pPrevious_;
            aCard->pNext_ = this;
            this->pPrevious_ = aCard;
            
            if (aCard->pPrevious_ != NULL)
            {
                // This card is not at the beginning of the list
                aCard->pPrevious_->pNext_ = aCard;
            }
        }
        
        return aCard;
    }
    
    // ---------------------------------------------------------------------------------
    // showCards - simple display function to see the suit and rank of a card in
    // a specific position
    // ---------------------------------------------------------------------------------
    void showCards()
    {
        // Find the top of the deck by picking any card and traversing the
        // previous pointers until NULL is found
        Card *pThis = this->getFirst();
        
        unsigned int i = 1;
        do
        {
            printf("CARD %3d: %8s of %8s\n", i, RankNames[pThis->getRank()], SuitNames[pThis->getSuit()]);
            
            ++i;
            pThis = pThis->getNext();
            
        } while (pThis != NULL);
        
    }
    
};
unsigned int Card::masterCardCount_ = 0;

// ---------------------------------------------------------------------------------
// A deck might look like nothing more than array of 52 cards (without jokers)
// or 54 cards (jokers), but there are some behaviors (like shuffling and dealing/drawing)
// that are also associated with state (like in deck or not)... this suggests
// something more than POD.
//
// So here we present a class for representing the Deck, which creates the Cards
// necessary for the game. This allows for each referencing to each Card regardless
// of which Hand the Card is in, thus allows for easy gathering of the Cards after
// each game.
// ---------------------------------------------------------------------------------

class Deck
{
public:
    static const int STANDARD_DECK_SIZE = 54;
    enum JokerPresentType
    {
        JPT_NO_JOKERS,
        JPT_KEEP_JOKERS
    };

private:
    static unsigned int deckSize_;
    Card **ppCards_;
    bool *pPresentInDeck_;
    bool jokersInDeck_;
    
    Card *pTopOfDeck_;

protected:
    
public:
    // ---------------------------------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------------------------------
    Deck(unsigned int aNumberOfDecks = 1, JokerPresentType aKeepJokers = JPT_KEEP_JOKERS, Ranks aJokerRank = RANK_WILD_HIGH)
    {

        jokersInDeck_ = (aKeepJokers == JPT_KEEP_JOKERS);
        unsigned int singleDeckSize = STANDARD_DECK_SIZE - ((jokersInDeck_? 0 : 2));
        deckSize_ = aNumberOfDecks * singleDeckSize;
        
        ppCards_ = new Card*[deckSize_];
        pPresentInDeck_ = new bool[deckSize_];
        
        for (unsigned int currentDeck = 0; currentDeck < aNumberOfDecks; ++currentDeck)
        {
            unsigned int currentCard = currentDeck * singleDeckSize;
            
            for (int thisSuit = SUIT_CLUB; thisSuit <= SUIT_SPADE; ++thisSuit)
            {
                for (int thisRank = RANK_TWO; thisRank <= RANK_ACE; ++thisRank)
                {
                    ppCards_[currentCard] = new Card((Suits)thisSuit, (Ranks)thisRank);
                    pPresentInDeck_[currentCard] = true;
                    ++currentCard;
                    
                }
            }
            
            if (jokersInDeck_)
            {
                // Fill the rest of the deck with jokers
                for (; currentCard < ((currentDeck + 1) * singleDeckSize); ++currentCard)
                {
                    ppCards_[currentCard] = new Card(SUIT_JOKER, aJokerRank);
                    pPresentInDeck_[currentCard] = true;
                }
            }
        }
        
        // Starting with the second card, link the cards together
        for (unsigned int currentCard = 1; currentCard < deckSize_; ++currentCard)
        {
            ppCards_[currentCard-1]->add(ppCards_[currentCard]);
        }
        
        pTopOfDeck_ = ppCards_[0];
        
    }

    // ---------------------------------------------------------------------------------
    // Destructor
    // ---------------------------------------------------------------------------------
    ~Deck()
    {
        // Deallocate memory
        for (int i = 0; i < deckSize_; ++i)
        {
            DELETE_POINTER(ppCards_[i]);
        }
        DELETE_POINTER_ARRAY(pPresentInDeck_);
        DELETE_POINTER_ARRAY(ppCards_);
        
    }

    // ---------------------------------------------------------------------------------
    // getDeckSize - returns the number of cards in the deck
    // ---------------------------------------------------------------------------------
    static unsigned int getDeckSize(void)
    {
        return deckSize_;
    }
    
    // ---------------------------------------------------------------------------------
    // showCards - simple display function to see the suit and rank of a card in
    // a specific position
    // ---------------------------------------------------------------------------------
    void showCards()
    {
        if (pTopOfDeck_ != NULL)
        {
            pTopOfDeck_->showCards();
        }
        else
        {
            printf("Deck is Empty\n");
        }
    }
    
    // ---------------------------------------------------------------------------------
    // gather - returns all cards to the deck by attaching them to the same list.
    // Any references (e.g., Hands) should be cleared in response to a Deck::gather().
    // The deck is gathered back into the original order.
    // ---------------------------------------------------------------------------------
    void gather(void)
    {
        // Starting at the top of the array, bring all cards back into the list so they
        // can be shuffled and dealt.
        ppCards_[0]->remove();
        pPresentInDeck_[0] = true;
        for (unsigned int i = 1; i < deckSize_; ++i)
        {
            ppCards_[i-1]->add(ppCards_[i]);
            pPresentInDeck_[i] = true;
        }
        pTopOfDeck_ = ppCards_[0];
    }

    // ---------------------------------------------------------------------------------
    // shuffle - shuffles the deck
    // Caller can specify the number of shuffles if more time shuffling is desired,
    // but the results of a single shuffle should be sufficient.
    // The default shuffle is 100 x deckSize_ random 2-card swaps
    // ---------------------------------------------------------------------------------
    void shuffle(unsigned int aNumberOfShuffles = 1)
    {
        // Seed the random number generator
        srand((unsigned int)(time(NULL) & 0xFFFFFFFF));
                   
        for (unsigned int shuffleCount = 0; shuffleCount < aNumberOfShuffles; ++shuffleCount)
        {
            // Considering the deck size, perform 100x passes
            // through a random selection of 2-card swaps
            for (unsigned int passCount = 0; passCount < 100; ++passCount)
            {
                for (unsigned int cardCount = 0; cardCount < deckSize_; ++cardCount)
                {
                    unsigned int indexA = rand() % deckSize_;
                    unsigned int indexB = rand() % deckSize_;
                    
                    // Put card A before card B
                    ppCards_[indexB]->insertBefore(ppCards_[indexA]);

                }
            }
        }
        
        pTopOfDeck_ = pTopOfDeck_->getFirst();
    }

    // ---------------------------------------------------------------------------------
    // deal - Deal a card from the top of the deck
    // ---------------------------------------------------------------------------------
    Card * deal(void)
    {
        Card *pCard = NULL;
        
        if (pTopOfDeck_ != NULL)
        {
            Card *pNext = pTopOfDeck_->getNext();
            
            pCard = pTopOfDeck_->remove();
        
            pTopOfDeck_ = pNext;
        }
        
        return pCard;
    }
    
    // ---------------------------------------------------------------------------------
    // push - pushes the card back into the deck
    // ---------------------------------------------------------------------------------
    
};
unsigned int Deck::deckSize_ = 0;

// FUTURE WORK ON USING TYPE TO REPRESENT HAND
//// ---------------------------------------------------------------------------------
//// class Hand - a hand references Cards from the Deck, maintaining a count and
//// the first/last pointers internally for more efficient manipulation of the
//// list of cards in the hand.
////
//// A Hand is constructed with knowledge of the largest number of cards it can
//// hold, which can be as large as the Deck for some games
//// ---------------------------------------------------------------------------------
//class Hand
//{
//private:
//    Card **ppCards_;
//    
//    Card *pFirst_;
//    Card *pLast_;
//    
//protected:
//    
//public:
//    // ---------------------------------------------------------------------------------
//    // Constructor
//    // ---------------------------------------------------------------------------------
//    Hand()
//    : pFirst_(NULL),
//      pLast_(NULL)
//    {
//        // Always make the hand big enough to hold all of the Cards in the Deck we
//        // are playing with.
//        unsigned int deckSize = Deck::getDeckSize();
//        if (deckSize != 0)
//        {
//            ppCards_ = new Card*[deckSize];
//        }
//        else
//        {
//            perror("Must create Deck before Hands\n");
//            ppCards_ = NULL;
//        }
//    }
//    
//    // ---------------------------------------------------------------------------------
//    // Destructor
//    // ---------------------------------------------------------------------------------
//    ~Hand()
//    {
//        DELETE_POINTER_ARRAY(ppCards_);
//    }
//    
//    // ---------------------------------------------------------------------------------
//    // add - assigns a card to the hand
//    // ---------------------------------------------------------------------------------
//    Card * add(Card *pCard)
//    {
//        if (pLast_ == NULL)
//        {
//            pFirst_ = pLast_ = pCard;
//        }
//        else
//        {
//            if (pCard != NULL)
//            {
//                pLast_->add(pCard);
//            }
//        }
//        
//        return pCard;
//    }
//    
//};

// ---------------------------------------------------------------------------------
// main - start here
// ---------------------------------------------------------------------------------

int main(int argc, const char * argv[])
{
    Deck *pDeck = new Deck(NUMBER_OF_DECKS, Deck::JPT_NO_JOKERS);   // Remove jokers
    Deck &deck = *pDeck;
    
    deck.showCards();
    
    printf("\n\nShuffling...\n\n");
    deck.shuffle();
    
    deck.showCards();
    
    printf("\n\nDealing...\n\n");

    // Play war by dealing the deck evenly to 2 hands
    // For now the hands are just pointers to lists of cards
    // Later we may actually encapsulate as a Hand as a means
    // to reference the cards from the deck... i.e., we never
    // really want to move the card data, just its reference.
    Card *pHandA = deck.deal();
    Card *pHandB = deck.deal();
    
    // Deal the rest of the cards until the deck is empty
    // May want to create a dealer that can handle multiple hands
    // and input to decide how much to deal.
    for(;;)
    {
        Card *pNext = deck.deal();
        if (pNext != NULL)
        {
            pHandA->add(pNext);
        }
        else
        {
            break;
        }
        
        pNext = deck.deal();
        if(pNext != NULL)
        {
            pHandB->add(pNext);
        }
        else
        {
            break;
        }
    }
    
    printf("\n\nHand A\n");
    pHandA->showCards();
    
    printf("\n\nHand B\n");
    pHandB->showCards();
    
    // Pull the first two cards
    // To prevent needing to traverse the list each time
    // we will retain some information in each pass
    
    Card *pTopA = pHandA->getFirst();
    Card *pTopB = pHandB->getFirst();
    
    Card *pNextA = pTopA->getNext();
    Card *pNextB = pTopB->getNext();
    
    Card *pLastA = pTopA->getLast();
    Card *pLastB = pTopB->getLast();
    
    Card *pDiscardA = pTopA->remove();
    Card *pDiscardB = pTopB->remove();

    Card *pNextDiscardToReturn[2];
    
    pTopA = pNextA;
    pNextA = pNextA->getNext();
    
    pTopB = pNextB;
    pNextB = pNextB->getNext();
    
    unsigned int cycleCount = 0;
    while (((pTopA != NULL)     &&
            (pTopB != NULL))
               ||
           ((pDiscardA != NULL) &&
            (pDiscardB != NULL)))
    {
        ++cycleCount;
        
        unsigned int countA = pTopA==NULL?0:pTopA->count();
        unsigned int countB = pTopB==NULL?0:pTopB->count();
        unsigned int discardCountA = pDiscardA==NULL?0:pDiscardA->count();
        unsigned int discardCountB = pDiscardB==NULL?0:pDiscardB->count();
        
        // Play war
        if (pDiscardA->getRank() > pDiscardB->getRank())
        {
            printf("%4d %4d %2d %2d = %4d ------A WINS------- Cycle %6d\n",
                   countA,
                   countB,
                   discardCountA,
                   discardCountB,
                   countA + countB + discardCountA + discardCountB,
                   cycleCount);
            
            // Put both cards in Hand A and update the discard piles
            // Randomly select which to do first to prevent the cards
            // from sorting themselves into a war-free configuration
            unsigned int aOrB = rand() % 2;
            
            pNextDiscardToReturn[aOrB & 0x1] = pDiscardA;
            pNextDiscardToReturn[~aOrB & 0x1] = pDiscardB;
            
            for (unsigned int i = 0; i < DIM(pNextDiscardToReturn); ++i)
            {
                while (pNextDiscardToReturn[i] != NULL)
                {
                    if (pLastA != NULL)
                    {
                        pNextDiscardToReturn[i] = pLastA->add(pNextDiscardToReturn[i]);
                        pLastA = pLastA->getNext();
                    }
                    else
                    {
                        Card *pNextReturn = pNextDiscardToReturn[i]->getNext();
                        pLastA = pNextDiscardToReturn[i]->remove();
                        pTopA = pLastA;
                        pNextDiscardToReturn[i] = pNextReturn;
                    }
                }
            }
            
            pNextA = pTopA->getNext();
            pDiscardA = NULL;
            pDiscardB = NULL;
            
        }
        else if (pDiscardA->getRank() < pDiscardB->getRank())
        {
            printf("%4d %4d %2d %2d = %4d ------B WINS------- Cycle %6d\n",
                   countA,
                   countB,
                   discardCountA,
                   discardCountB,
                   countA + countB + discardCountA + discardCountB,
                   cycleCount);
            
            // Put both cards in Hand B and update the discard piles
            // Randomly select which to do first to prevent the cards
            // from sorting themselves into a war-free configuration
            unsigned int aOrB = rand() % 2;
            
            pNextDiscardToReturn[aOrB & 0x1] = pDiscardB;
            pNextDiscardToReturn[~aOrB & 0x1] = pDiscardA;
            
            for (unsigned int i = 0; i < DIM(pNextDiscardToReturn); ++i)
            {
                while (pNextDiscardToReturn[i] != NULL)
                {
                    if (pLastB != NULL)
                    {
                        pNextDiscardToReturn[i] = pLastB->add(pNextDiscardToReturn[i]);
                        pLastB = pLastB->getNext();
                    }
                    else
                    {
                        Card *pNextReturn = pNextDiscardToReturn[i]->getNext();
                        pLastB = pNextDiscardToReturn[i]->remove();
                        pTopB = pLastB;
                        pNextDiscardToReturn[i] = pNextReturn;
                    }
                }
            }
            
            pNextB = pTopB->getNext();
            pDiscardA = NULL;
            pDiscardB = NULL;
        }
        else
        {
            printf("%4d %4d %2d %2d = %4d --------WAR-------- Cycle %6d\n",
                   countA,
                   countB,
                   discardCountA,
                   discardCountB,
                   countA + countB + discardCountA + discardCountB,
                   cycleCount);
            
            // Put a new card on top of each discard pile
            if ((pTopA != NULL) &&
                (pDiscardA != NULL))
            {
                pDiscardA = pDiscardA->insertBefore(pTopA->remove());
            }
            
            if ((pTopB != NULL) &&
                (pDiscardA != NULL))
            {
                pDiscardB = pDiscardB->insertBefore(pTopB->remove());
            }
            
            pTopA = pNextA;
            if (pNextA != NULL)
            {
                pNextA = pNextA->getNext();
            }
            else
            {
                pLastA = NULL;
            }
            
            pTopB = pNextB;
            if (pNextB != NULL)
            {
                pNextB = pNextB->getNext();
            }
            else
            {
                pLastB = NULL;
            }
        }
        
        // Pull two more cards if we know we
        // can play another round
        if ((pTopA != NULL) &&
            (pTopB != NULL))
        {
            if (pDiscardA == NULL)
            {
                if (pTopA != NULL)
                {
                    pDiscardA = pTopA->remove();
                }
            }
            else
            {
                if (pTopA != NULL)
                {
                    pDiscardA = pDiscardA->insertBefore(pTopA->remove());
                }
            }
            
            if (pDiscardB == NULL)
            {
                if (pTopB != NULL)
                {
                    pDiscardB = pTopB->remove();
                }
            }
            else
            {
                if (pTopB != NULL)
                {
                    pDiscardB = pDiscardB->insertBefore(pTopB->remove());
                }
            }
            
            pTopA = pNextA;
            if (pNextA != NULL)
            {
               pNextA = pNextA->getNext();
            }
            else
            {
                pLastA = NULL;
            }
            
            pTopB = pNextB;
            if (pNextB != NULL)
            {
               pNextB = pNextB->getNext();
            }
            else
            {
                pLastB = NULL;
            }
        }
    }
    
    
    if (pTopA != NULL)
    {
        printf("A CARDS--------\n");
        pTopA->showCards();
    }
    if (pTopB != NULL)
    {
        printf("B CARDS--------\n");
        pTopB->showCards();
    }
    if (pDiscardA != NULL)
    {
        printf("CARDS on Table from A\n");
        pDiscardA->showCards();
    }
    if (pDiscardB != NULL)
    {
        printf("CARDS on Table from B\n");
        pDiscardB->showCards();
    }
    printf("%s Wins in %d draws!\n", (pTopA == NULL)?"B":"A", cycleCount);
    
//    printf("Gathering Deck...\n");
//    deck.gather();
//    deck.showCards();

    DELETE_POINTER(pDeck);
    
    return 0;
}

