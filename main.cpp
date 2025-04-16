#include <bits/stdc++.h>
#include "random.h"
using namespace std;

namespace Settings {
    constexpr int bust {21};
    constexpr int dealerStopsAt {17};
}

struct Card {
    enum Rank {
        rank_ace,
        rank_2,
        rank_3,
        rank_4,
        rank_5,
        rank_6,
        rank_7,
        rank_8,
        rank_9,
        rank_10,
        rank_jack,
        rank_queen,
        rank_king,

        max_ranks,
    };

    enum Suit {
        suit_club,
        suit_diamond,
        suit_heart,
        suit_spade,


        max_suits,
    };

    Rank rank;
    Suit suit;

    static constexpr array allRanks{
        rank_ace, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8, rank_9, rank_10, rank_jack, rank_queen,
        rank_king
    };
    static constexpr array allSuits {suit_club, suit_diamond, suit_heart, suit_spade};

    friend ostream& operator<<(ostream& out, const Card& card) {
        static constexpr array ranks {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
        static constexpr array suits {'H', 'C', 'D', 'S'};

        return out<<ranks[static_cast<size_t>(card.rank)]<<suits[static_cast<size_t>(card.suit)];
    }

    int value() const {
        static constexpr array values {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
        return values[rank];
    }
};

class Deck {
    array<Card, 52> m_cards {};
    int m_nextCardIndex {0};

public:
    Deck() {
        int i=0;
        for (auto const suit: Card::allSuits) {
            for (auto const rank: Card::allRanks) {
                m_cards[i++] = Card{rank, suit};
            }
        }
    }

    void shuffle() {
        ranges::shuffle(m_cards.begin(), m_cards.end(),  Random::mt);
        m_nextCardIndex=0;
    }

    Card dealCard() {
        assert(m_nextCardIndex!=52 && "Deck::dealCard ran out of cards");
        return m_cards[m_nextCardIndex++];
    }
};

struct Player {
    int score {0};
    int aceCount {0};

    void addScore(const Card& card) {
        score+=card.value();
        if (card.rank==Card::rank_ace) {
            aceCount++;
        }
    }

    void consumeAces() {
        while (score>Settings::bust && aceCount>0) {
            score-=10;
            --aceCount;
        }
    }
};

bool dealersTurn(Player& dealer, Deck& deck) {
    const Card d_card {deck.dealCard()};
    dealer.score+=d_card.value();
    cout<<"The dealer flips a "<<d_card<<". They now have: "<<dealer.score<<'\n';


    return dealer.score>Settings::bust;
}

bool playersTurn(Player& player, Deck& deck) {
    char c;
    do {
        cout<<"(h) to hit, (s) to stand: ";
        cin>>c;
        if (c!='h' && c!='s') {
            cout<<"Invalid input. Try again\n";
        }
    } while (c!='h' && c!='s');


    if (c=='h') {
        const Card p_card {deck.dealCard()};
        player.addScore(p_card);

        cout<<"You were dealt a "<<p_card<<". You now have: "<<player.score<<'\n';

        if (player.score>Settings::bust && player.aceCount>0) {
            player.consumeAces();
            cout<<"You consumed your aces now your score is: "<<player.score<<"\n";
        }

        if (player.score>Settings::bust) {
            return true;  //player busts
        }
        return false;
    }

    if (c=='s') {
        return true;
    }

    return true;
}

enum class gameResult {
    player_won,
    dealer_won,
    tie,
};

gameResult playBlackJack() {
    Deck deck;
    deck.shuffle();

    const Card d_card {deck.dealCard()};
    Player dealer;
    dealer.addScore(d_card);

    cout<<"The dealer is showing "<<d_card<<" ("<<dealer.score<<")\n";

    const Card p_card_1 {deck.dealCard()};
    const Card p_card_2 {deck.dealCard()};

    Player player;
    player.addScore(p_card_1);
    player.addScore(p_card_2);
    cout<<"You are showing "<<p_card_1<<" "<<p_card_2<<" ("<<player.score<<")\n";

    while (true) {
        if (playersTurn(player, deck)==true) {
            if (player.score>Settings::bust) {
                cout<<"You went bust\n";
                return gameResult::dealer_won;
            }
            if (player.score<=Settings::bust){
                break;
            }
        }
    }

    while (dealer.score<=player.score && dealer.score<Settings::dealerStopsAt) {
        if (dealersTurn(dealer, deck)==true) {
            cout<<"The dealer went bust\n";
            return gameResult::player_won;
        }
    }

    if (player.score>dealer.score) {
        return gameResult::player_won;
    }
    if (player.score<dealer.score) {
        return gameResult::dealer_won;
    }

    return gameResult::tie;
}

int main() {
    switch (playBlackJack()) {
        case gameResult::dealer_won:
            cout<<"You lost\n";
            break;
        case gameResult::player_won:
            cout<<"You won!\n";
            break;
        case gameResult::tie:
            default:
                cout<<"It was a tie\n";
                break;
    }
    return 0;
}
