/**
  *
  *   Author: Joao Bernardo Almeida Santos <joao.almeida.santos@tecnico.ulisboa.pt>
  *   Number: 84083
  *   MEEC - Instituto Superior Técnico
  *
  *
  *   Graphical Interface provided by Professor João Ascenso
  *
  *   Blackjack Game
  *
  *
  */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define STRING_SIZE 100       // max size for some strings
#define WIDTH_WINDOW 900      // window width
#define HEIGHT_WINDOW 525     // window height
#define MAX_DECK_SIZE 52      // number of max cards in the deck
#define MAX_NUMBER_OF_DECKS 6   // maximum number of decks to play
#define MIN_NUMBER_OF_DECKS 1
#define MAX_CARD_HAND 11      // 11 cards max. that each player can hold
#define CARD_WIDTH 67         // card width
#define CARD_HEIGHT 97        // card height
#define WINDOW_POSX 500       // initial position of the window: x
#define WINDOW_POSY 250       // initial position of the window: y
#define EXTRASPACE 150
#define MARGIN 5
#define MAX_PLAYERS 4         // number of maximum players
#define MIN_INITIAL_MONEY 10  // minimum amount of initial money for each player
#define MIN_BET_VALUE 1 // minimum bet amount
#define MAX_BET_VALUE_PERCENTAGE 0.2 // maximum bet amount (% of initial money)
#define INITIAL_CARDS_AMOUNT 2 // number of cards to be initially dealt
#define ACE_ID 12 // card id of ace
#define ACE_VALUE_MAX 11 // maximum points of ace card
#define ACE_VALUE_MIN 1 // minimum points of ace card
#define MAXIMUM_POINTS 21 // maximum points
#define FIGURE_VALUE 10 // points of figure cards
#define NUMBER_OF_STATS 3 // number of stats
#define STAT_WON 0 // index for won games stats
#define STAT_TIED 1 // index for tied games stats
#define STAT_LOST 2 // index for lost games stats

// declaration of the functions related to graphical issues
void InitEverything(int , int , SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int , int , const char* , TTF_Font *, SDL_Color *, SDL_Renderer * );
int RenderLogo(int , int , SDL_Surface *, SDL_Renderer * );
void RenderTable(int [], int *, SDL_Surface **, SDL_Renderer *, int, int);
void RenderCard(int , int , int , SDL_Surface **, SDL_Renderer * );
void RenderHouseCards(int [], int , SDL_Surface **, SDL_Renderer * );
void RenderPlayerCards(int [][MAX_CARD_HAND], int [], SDL_Surface **, SDL_Renderer * );
void LoadCards(SDL_Surface **);
void UnLoadCards(SDL_Surface **);
void Hit(int *, int, int *, int *, int *, int *);
void Stand(int *, int, int *, int [][MAX_CARD_HAND], int *, int *);
void ReadGameParameters(int *, int *, int *);
int InitializeDeck(int *, int);
void ShuffleDeck(int *, int);
void Swap(int *, int *);
short DealCards(int *, int *, int [][MAX_CARD_HAND], int *, int *, int *, int *, int, int *);
int *NextCard(int *, int *);
void DeterminePoints(int *, int *, int);
short IsAce(int);
int PointsFromCardID(int);
short Bust(int);
int PlayHouse(int *, int *, int *, int, int *);
void UpdateMoneyAndStats(int *, int [][NUMBER_OF_STATS], int bet, int *, int, int *, int);
void FinishTurn(int *, int, int *, int *, int [][NUMBER_OF_STATS], int, int *, int *, int *, int *, int *);
void WriteMoneyAndStatsToFile(int *, int [][NUMBER_OF_STATS]);
short Blackjack(int, int);


// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Joao Almeida Santos";
const char myNumber[] = "84083";
const char * playerNames[] = {"Player 1", "Player 2", "Player 3", "Player 4"};
const char STATISTICS_FILE_NAME[] = "stats.txt"; // statistics file location

/**
* main function: entry point of the program
* only to invoke other functions !
*/
int main( int argc, char* args[] )
{
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Surface *cards[MAX_DECK_SIZE+1], *imgs[2];
  SDL_Event event;
  int delay = 300; // interface refresh delay
  int quit = 0; // quit variable
  int money[MAX_PLAYERS + 1] = { 0 }; // money for each player. last position is house current balance
  int player_cards[MAX_PLAYERS][MAX_CARD_HAND] = {{ 0 }}; // cards of each player
  int player_points[MAX_PLAYERS] = { 0 }; // points of each player
  int player_stats[MAX_PLAYERS][NUMBER_OF_STATS] = {{ 0 }}; // statistics for each player
  int house_cards[MAX_CARD_HAND] = { 0 }; // house hand
  int deck[MAX_NUMBER_OF_DECKS * MAX_DECK_SIZE] = { 0 }; // set of decks
  int pos_house_hand = 0; // number of cards in the house hand
  int pos_player_hand[MAX_PLAYERS] = { 0 }; // array with the number of cards of each player
  int numberOfDecks = 0; // number of decks used in the game
  int initialMoney = 0; // initial money
  int betAmount = 0; // bet amount
  int numberOfCards = 0; // total number of cards in the deck
  int currentCard = 0; // position of deck's top card
  int currentPlayer = -1; // current selected player
  int i = 0; // iterator
  int house_points = 0; // house points
  short turn_ended = 0; // true value if turn has ended

  srand(456); // initializes the random number generator

  /* Prints welcome message */
  printf("**************************\n*                        *\n*  Welcome to BlackJack  *\n*                        *\n**************************\n\n");

  /* Reads parameters */
  ReadGameParameters(&numberOfDecks, &initialMoney, &betAmount);

  /* Gives each player the initial amount of money */
  for (i = 0; i < MAX_PLAYERS; i++) {
      money[i] = initialMoney;
  }

  // initialize graphics
  InitEverything(WIDTH_WINDOW, HEIGHT_WINDOW, imgs, &window, &renderer);
  // loads the cards images
  LoadCards(cards);

  numberOfCards = InitializeDeck(deck, numberOfDecks);

  ShuffleDeck(deck, numberOfCards);

  DealCards(deck, &currentCard, player_cards, pos_player_hand, house_cards, &pos_house_hand, money, betAmount, player_points);

  /* Stands until the current player hasn't a blackjack */
  Stand(&currentPlayer, betAmount, money, player_cards, pos_player_hand, player_points);

  /* Main loop. Window remains open until quit == 1 */
  while( quit == 0 )
  {
    // while there's events to handle
    while( SDL_PollEvent( &event ) )
    {
      if( event.type == SDL_QUIT )
      {
        quit = 1;
      }
      else if ( event.type == SDL_KEYDOWN )
      {
        switch ( event.key.keysym.sym )
        {
          case SDLK_q:
            quit = 1; // Quits
            break;
          case SDLK_s:
             // stand !
             /* Verifies the turn hasn't end */
             if (!turn_ended) {
                 Stand(&currentPlayer, betAmount, money, player_cards, pos_player_hand, player_points);

                 /* If there are no more players after this stand */
                 if (currentPlayer >= MAX_PLAYERS) {
                     /**
                       * Finishes the turn. The house plays and the money is updated.
                       * After this function, it's possible to press the 'n' key
                       * to begin a new turn.
                       */
                     FinishTurn(deck, numberOfCards, &currentCard, money, player_stats, betAmount, house_cards, pos_player_hand, &pos_house_hand, player_points, &house_points);

                     turn_ended = 1;
                 }
             }

             break;
          case SDLK_h:
            // hit !
            /* Verifies the turn hasn't end */
            if (!turn_ended) {
                /* The current player requests a card. */
                Hit(deck, numberOfCards, &currentCard, player_cards[currentPlayer], &pos_player_hand[currentPlayer], &player_points[currentPlayer]);

                /* Verifies bust or full hand. If so, stands. */
                if (Bust(player_points[currentPlayer]) || pos_player_hand[currentPlayer] > MAX_CARD_HAND) {
                    Stand(&currentPlayer, betAmount, money, player_cards, pos_player_hand, player_points);

                    /* If there are no more players after this stand */
                    if (currentPlayer >= MAX_PLAYERS) {
                        FinishTurn(deck, numberOfCards, &currentCard, money, player_stats, betAmount, house_cards, pos_player_hand, &pos_house_hand, player_points, &house_points);

                        turn_ended = 1;
                    }
                }
            }

            break;
          case SDLK_n:
            /* Verifies if the turn has ended. If so, begins a new turn. */
            if (turn_ended) {
                /* Resets player points to 0 */
                for (i = 0; i < MAX_PLAYERS; i++) {
                    player_points[i] = 0;
                }
                /* Resets house points to 0 */
                house_points = 0;

                /* Verfies if there are any dealt cards. If not, quits the game
                 * diplaying a message.
                 */
                if (!DealCards(deck, &currentCard, player_cards, pos_player_hand, house_cards, &pos_house_hand, money, betAmount, player_points)) {
                    printf("No more players! Thank you for playing Blackjack!\n");
                    quit = 1;
                    break;
                }

                /* Selects current player to imaginary so that we can stand to the first
                 * without blackjack */
                currentPlayer = -1;

                /* Stands until the current player hasn't a blackjack */
                Stand(&currentPlayer, betAmount, money, player_cards, pos_player_hand, player_points);

                turn_ended = 0;
            }
            break;
          default:
            break;
        }
      }
    }


    // render game table
    RenderTable(money, player_points, imgs, renderer, currentPlayer, house_points);
    // render house cards
    RenderHouseCards(house_cards, pos_house_hand, cards, renderer);
    // render player cards
    RenderPlayerCards(player_cards, pos_player_hand, cards, renderer);
    // render in the screen all changes above
    SDL_RenderPresent(renderer);
    // add a delay
    SDL_Delay( delay );
  }

  // free memory allocated for images and textures and close everything
  UnLoadCards(cards);
  SDL_FreeSurface(imgs[0]);
  SDL_FreeSurface(imgs[1]);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  /* Writes stats file */
  WriteMoneyAndStatsToFile(money, player_stats);

  return EXIT_SUCCESS;
}

/**
* NextCards: Returns the deck's top card.
* \param deck
* \param currentCard
*/
int *NextCard(int *deck, int *currentCard)
{
    return &deck[++*currentCard];
}

/**
* Hit: Hits a card and determines player points.
* \param deck the deck of cards
* \param numberOfCards number of cards in the deck
* \param currentCard position of the top card of the deck
* \param cards array with the player's cards
* \param pos_hand number of cards of the player
* \param points array with each player's points
*/
void Hit(int *deck, int numberOfCards, int *currentCard, int *cards, int *pos_hand, int *points)
{
    /* If there are no more cards initializes the deck again */
    if (*currentCard >= (numberOfCards - 1)) {
        InitializeDeck(deck, (numberOfCards / MAX_DECK_SIZE));
        ShuffleDeck(deck, numberOfCards);

        *currentCard = 0;
    }

    /* If player has less than max cards */
    if (*pos_hand <= MAX_CARD_HAND) {
        cards[(*pos_hand)++] = *NextCard(deck, currentCard); // Hit!

        DeterminePoints(points, cards, *pos_hand); // Determines points
    }
}

/**
* Stand: Stands until a player with enough money and also without a blackjack or a bust is reached.
* \param currentPlayer
* \param bet bet amount
* \param money array with each player and house's money
* \param player_cards array with each player's cards
* \param pos_player_hand array with the number of cards of each player
* \param player_points array with each player's points
*/
void Stand(int *currentPlayer, int bet, int *money, int player_cards[][MAX_CARD_HAND], int *pos_player_hand, int *player_points)
{
    /* At least once and until current player has money, isn't bust or hasn't a blackjack, stands */
    do {
            /* Stands if there are any more players */
        if (++(*currentPlayer) < MAX_PLAYERS) {
            DeterminePoints(&player_points[*currentPlayer], player_cards[*currentPlayer], pos_player_hand[*currentPlayer]);
        } else {
            break; // Else exits loop -> Turn ended!
        }
    } while (money[*currentPlayer] < bet || player_points[*currentPlayer] >= MAXIMUM_POINTS);
}

/**
* Bust: Checks bust.
* \param player_points
*/
short Bust(int player_points)
{
    return player_points > MAXIMUM_POINTS;
}

/**
* FinishTurn: Finishes current turn. Plays the house and updates money and stats.
* \param deck the deck of cards
* \param currentCard deck top card position
* \param money array with each player and house's money
* \param stats array with stats for each player
* \param betAmount
* \param house_cards array with house cards
* \param pos_house_hand number of house cards
* \param player_points array with each player's points
* \param house_points
*/
void FinishTurn(int *deck, int numberOfCards, int *currentCard, int *money, int stats[][NUMBER_OF_STATS], int betAmount, int *house_cards, int *pos_player_hand, int *pos_house_hand, int *player_points, int *house_points)
{
    *house_points = PlayHouse(house_cards, pos_house_hand, deck, numberOfCards, currentCard); // plays house

    UpdateMoneyAndStats(money, stats, betAmount, player_points, *house_points, pos_player_hand, *pos_house_hand); // updates money
}

/**
* PlayHouse: Plays the house.
* Returns house points.
* \param house_cards array with house cards
* \param pos_house_hand number of house cards
* \param deck the deck of cards
* \param numberOfCards number of cards in the deck
* \param currentCard deck top card position
*/
int PlayHouse(int *house_cards, int *pos_house_hand, int *deck, int numberOfCards, int *currentCard)
{
    int housePoints = 0; // house points
    int numberOfAces = 0; // number of aces in house hand
    int i = 0; // iterator

    *pos_house_hand = 2; // shows the card face down

    DeterminePoints(&housePoints, house_cards, *pos_house_hand);

    /* At least once and while house has less than 16 points
     * hits a card only if house points is less than 16, or is 17
     * and house has one ace
     */
    do {
        numberOfAces = 0;

        /* Determines the number of aces in house hand */
        for (i = 0; i < (*pos_house_hand); i++) {
            if (IsAce(house_cards[i])) {
                numberOfAces += 1;
            }
        }

        /* If house points is less than 16, or is 17 and house has one ace */
        if (housePoints < 16 || (numberOfAces == 1 && housePoints == 17)) {
            Hit(deck, numberOfCards, currentCard, house_cards, pos_house_hand, &housePoints); // House Hits!
        }
    } while (housePoints < 16);

    return housePoints;
}

/**
* InitializeDeck: Initializes an array of card IDs, ordered ascendendly from
* ID 0 to MAX_DECK_SIZE, repeated until numberOfDecks are used. Returns the
* number of cards used.
* \param deck array to be initialized
* \param numberOfDecks number of single decks of cards to be used
*/
int InitializeDeck(int *deck, int numberOfDecks)
{
    int numberOfCards = numberOfDecks * MAX_DECK_SIZE; // total number of cards in the deck
    int i = 0; // iterator

    /* Assigns a card to each position of deck by order */
    for (i = 0; i < numberOfCards; i++) {
        deck[i] = i % MAX_DECK_SIZE;
    }

    return numberOfCards;
}

/**
* ShuffleDeck: Shuffles the deck of cards using the Fisher-Yates algorithm.
* \param deck array to be shuffled
* \param numberOfCards number of cards in the deck
*/
void ShuffleDeck(int *deck, int numberOfCards)
{
    int i = 0; // iterator

    for (i = (numberOfCards - 1); i > 0; i--) {
        int random = rand() % numberOfCards;

        Swap(&deck[i], &deck[random]);
    }
}

/**
* WriteMoneyAndStatsToFile: Writes the statistics file
* \param money array with player and house's money
* \param stats array with stats for each player
*/
void WriteMoneyAndStatsToFile(int *money, int stats[][NUMBER_OF_STATS])
{
    FILE *statsFile; // pointer to be used for stats file access
    int i; // iterator

    /* Opens the statistics file */
    statsFile = fopen(STATISTICS_FILE_NAME, "w");

    /* Prints some information */
    fprintf(statsFile, "-- Statistics -- \n\n");
    fprintf(statsFile, "[Player Name]: [Games Won] - [Games Tied] - [Games Lost] (Money: [Final Money])\n\n");

    /* Writes statstics for each player */
    for (i = 0; i < MAX_PLAYERS; i++) {
        fprintf(statsFile, "%s: %d - %d - %d (Money: %d)\n", playerNames[i], stats[i][STAT_WON], stats[i][STAT_TIED], stats[i][STAT_LOST], money[i]);
    }

    /* Writes the house final balance */
    fprintf(statsFile, "House Money: %d", money[MAX_PLAYERS]);

    /* Closes the statistics file */
    fclose(statsFile);
}

/**
* DealCards: Performs the initial deal. Dealing 2 times 1 card for each player.
* Returns the number of dealt cards.
* \param deck deck of cards
* \param currentCard current card of deck
* \param player_cards array with each player's hand
* \param pos_player_hand array with the number of cards of each player
* \param house_cards array with house cards
* \param pos_house_hand number of house cards
*/
short DealCards(int *deck, int *currentCard, int player_cards[][MAX_CARD_HAND], int *pos_player_hand, int *house_cards, int *pos_house_hand, int *money, int bet, int *points)
{
    int i = 0, j = 0; // iterators
    int cardsDealt = 0; // number of cards dealt

    /* Deals INITIAL_CARDS_AMOUNT times... */
    for (i = 0; i < INITIAL_CARDS_AMOUNT; i++) {
        /* ...a card for each player */
        for (j = 0; j < MAX_PLAYERS; j++) {
            /* Check if the player has enough money. If not skips it. */
            if (money[j] < bet) {
                pos_player_hand[j] = 0; // in this case player has 0 cards
                continue;
            }

            player_cards[j][i] = *NextCard(deck, currentCard); // Deals the card
            pos_player_hand[j] = i+1; // Updates the number of cards
            cardsDealt++;             // and cards dealt.
        }
        /* ...a card for the house */
        house_cards[i] = *NextCard(deck, currentCard);
    }
    /* Determines each player points */
    for (i = 0; i < MAX_PLAYERS; i++) {
        DeterminePoints(&points[i], player_cards[i], pos_player_hand[i]);
    }

    /* Since we want to display only a card face up, set pos_house_hand to 1 */
    *pos_house_hand = 1;

    return cardsDealt;
}

/**
* Swap: Swaps a to b.
* \param a
* \param b
*/
void Swap(int *a, int *b)
{
    int aux = 0;

    auxiliary = *a;

    *a = *b;
    *b = auxiliary;
}

/**
* DeterminePoints: Determines one player's points.
* It can also be used to determine house points.
* \param player_points pointer to store the result
* \param cards array with player's cards
* \param pos_player_hand number of player cards.
*/
void DeterminePoints(int *player_points, int *cards, int pos_player_hand)
{
    int result = 0; // final result
    int numberOfAces = 0; // number of aces in cards array
    int i = 0; // iterator

    /* Sums up points for each card in cards, incrementing numberOfAces when
     * an ace if found.
     */
    for (i = 0; i < pos_player_hand; i++) {
        int cardID = cards[i]; // current card id

        /* Verifies if cardID is an ace */
        if (IsAce(cardID)) {
            numberOfAces++; // if so increments numberOfAces
        }

        /* Sums current card points */
        result += PointsFromCardID(cardID);
    }

    /* If we have a bust, one at a time, the aces value becomes its minimum.
     * So, we have to subtract the maximum - minimum diference for each ace until we don't have a bust.  */
    if (Bust(result)) {
        /* Subtracts ACE_VALUE_MAX - ACE_VALUE_MIN for each ace. */
        for (i = 0; i < numberOfAces; i++) {
            result -= ACE_VALUE_MAX - ACE_VALUE_MIN;

            /* If we no longer have a bust exit loop. */
            if (!Bust(result)) {
                break;
            }
        }
    }

    *player_points = result; // updates player points
}

/**
* IsAce: Determines wheter a card is an Ace.
* \param cardID
*/
short IsAce(int cardID)
{
    return (cardID % 13) == ACE_ID;
}

/**
* PointsFromCardID: Returns the value of a card.
* \param id
*/
int PointsFromCardID(int id)
{
    int cardPosition = id % 13;

    /* If card is a figure */
    if (cardPosition > 8) {
        /* returns ACE_VALUE_MAX if it's an ace or FIGURE_VALUE if it isn't */
        return cardPosition == ACE_ID ? ACE_VALUE_MAX : FIGURE_VALUE;
    }

    /* [ELSE] */
    return cardPosition + 2;
}

/**
* UpdateMoneyAndStats: Updates each player's money and stats, including the house balance.
* \param money array with each player and house's money
* \param stats array with each player's stats
* \param bet bet amount
* \param player_points array with each player's points
* \param pos_player_hand array with the number of cards of each player
* \param pos_house_hand number of house's cards
*/
void UpdateMoneyAndStats(int *money, int stats[][NUMBER_OF_STATS], int bet, int *player_points, int house_points, int *pos_player_hand, int pos_house_hand)
{
    int i = 0; // iterator

    /* For each player, updates it's money and stats */
    for (i = 0; i < MAX_PLAYERS; i++) {
        /* If current player no longer has money skip it */
        if (money[i] < bet) {
            continue;
        }

        /* If the house is bust and the player isn't */
        if (Bust(house_points) && !Bust(player_points[i])) {
            money[i] += bet * 2; // Player wins 2 times bet amount
            money[MAX_PLAYERS] -= bet * 2;
            stats[i][STAT_WON] += 1;

        /* Else, if player is bust or has less points than house then it looses */
        } else if (Bust(player_points[i]) || player_points[i] < house_points) {
            money[i] -= bet;
            money[MAX_PLAYERS] += bet;
            stats[i][STAT_LOST] += 1;

        /* Else, if player has blackjack and house doens't then player wins the game
         * but only half of it's bet
         */
        } else if (Blackjack(pos_player_hand[i], player_points[i]) && !Blackjack(pos_house_hand, house_points)) {
            money[i] += bet * 1.5;
            money[MAX_PLAYERS] -= bet * 1.5;
            stats[i][STAT_WON] += 1;

        /* Else, if player has a greater hand than house's then it wins */
        } else if (player_points[i] > house_points) {
            money[i] += bet;
            money[MAX_PLAYERS] -= bet;
            stats[i][STAT_WON] += 1;

        /* If none of the above verifies it's a tie! */
        } else {
            stats[i][STAT_TIED] += 1;
        }
    }
}

/**
* Blackjack: Returns a true value if given parameters represent a Blackjack.
* \param numberOfCards
* \param points player's points
*/
short Blackjack(int numberOfCards, int points)
{
    return points == MAXIMUM_POINTS && numberOfCards == 2;
}

/**
* ReadGameParameters: Reads the parameters and stores in the given variables.
* \param numberOfDecks
* \param initialMoney
* \param betAmount
*/
void ReadGameParameters(int *numberOfDecks, int *initialMoney, int *betAmount)
{
    short notValid = 1; // used for parameter value check
    char buffer[STRING_SIZE] = { 0 }; // buffer for fgets

    /* While the parameter entered is not valid prompt for it */
    while (notValid) {
        /* Prompts the parameter */
        printf("Please enter the number of decks (%d to %d) you wan't to use in your game: ", MIN_NUMBER_OF_DECKS, MAX_NUMBER_OF_DECKS);
        fgets(buffer, STRING_SIZE, stdin);
        sscanf(buffer, "%d", numberOfDecks);

        notValid = *numberOfDecks < MIN_NUMBER_OF_DECKS || *numberOfDecks > MAX_NUMBER_OF_DECKS;

        /* If not valid show error */
        if (notValid) {
            printf("\n** Sorry, you have to select between %d to %d decks. **\n\n", MIN_NUMBER_OF_DECKS, MAX_NUMBER_OF_DECKS);
        }
    }

    notValid = 1; // resets not valid

    /* While the parameter entered is not valid prompt for it */
    while (notValid) {
        /* Prompts the parameter */
        printf("Please enter the initial amount of money for each player (> %d): ", MIN_INITIAL_MONEY);
        fgets(buffer, STRING_SIZE, stdin);
        sscanf(buffer, "%d", initialMoney);

        notValid = *initialMoney < MIN_INITIAL_MONEY;

        /* If not valid show error */
        if (notValid) {
            printf("\n** Sorry, each player has to have at least 10 EUR. **\n\n");
        }
    }

    notValid = 1; // resets not valid

    /* While the parameter entered is not valid prompt for it */
    while (notValid) {
        /* Determines maximum bet value */
        int betMaximumValue = MAX_BET_VALUE_PERCENTAGE * (*initialMoney);

        /* Prompts the parameter */
        printf("Please enter the bet amount (%d to %d): ", MIN_BET_VALUE, betMaximumValue);
        fgets(buffer, STRING_SIZE, stdin);
        sscanf(buffer, "%d", betAmount);

        notValid = *betAmount < MIN_BET_VALUE || *betAmount > betMaximumValue;

        /* If not valid show error */
        if (notValid) {
            printf("\n** Sorry, bet amount has to be from %d to %d. **\n\n", MIN_BET_VALUE, betMaximumValue);
        }
    }
}

/**
* RenderTable: Draws the table where the game will be played, namely:
* -  some texture for the background
* -  the right part with the IST logo and the student name and number
* -  squares to define the playing positions of each player
* -  names and the available money for each player
* \param _money amount of money of each player
* \param _img surfaces where the table background and IST logo were loaded
* \param _renderer renderer to handle all rendering in a window
* \param currentPlayer
* \param house_points
*/
void RenderTable(int _money[], int *points, SDL_Surface *_img[], SDL_Renderer* _renderer, int currentPlayer, int house_points)
{
  SDL_Color black = { 0, 0, 0 }; // black
  SDL_Color white = { 255, 255, 255 }; // white
  SDL_Color currentPlayerAreaColor = { 255, 0, 0 }; // red
  char name_money_str[STRING_SIZE] = { 0 };
  char points_str[STRING_SIZE] = { 0 };
  char house_points_str[STRING_SIZE] = { 0 };
  TTF_Font *serif = NULL;
  SDL_Texture *table_texture = NULL;
  SDL_Rect tableSrc = {0, 0, 0, 0}, tableDest = {0, 0, 0, 0}, playerRect = {0, 0, 0, 0};
  int separatorPos = (int)(0.95f*WIDTH_WINDOW); // seperates the left from the right part of the window
  int height = 0;
  SDL_Color *playerColor = NULL; // used for storing player color

  // set color of renderer to some color
  SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

  // clear the window
  SDL_RenderClear( _renderer );

  // this opens a font style and sets a size
  serif = TTF_OpenFont("FreeSerif.ttf", 16);
  if(!serif)
  {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  tableDest.x = tableSrc.x = 0;
  tableDest.y = tableSrc.y = 0;
  tableSrc.w = _img[0]->w;
  tableSrc.h = _img[0]->h;

  tableDest.w = separatorPos;
  tableDest.h = HEIGHT_WINDOW;

  table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
  SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

  // render the IST Logo
  height = RenderLogo(separatorPos, 0, _img[1], _renderer);

  // render the student name
  height += RenderText(separatorPos+3*MARGIN, height, myName, serif, &black, _renderer);

  // this renders the student number
  RenderText(separatorPos+3*MARGIN, height, myNumber, serif, &black, _renderer);

  // renders the areas for each player: names and money too !
  for ( int i = 0; i < MAX_PLAYERS; i++)
  {
    /* Determines this player color. If this player is current player player color is red. */
    playerColor = i == currentPlayer ? &currentPlayerAreaColor : &white;

    /* If this is the current player, show red rect */
    if (i == currentPlayer) {
        SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255 );
    } else {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255 );
    }

    playerRect.x = i*(separatorPos/4-5)+10;
    playerRect.y = (int) (0.55f*HEIGHT_WINDOW);
    playerRect.w = separatorPos/4-5;
    playerRect.h = (int) (0.42f*HEIGHT_WINDOW);

    sprintf(name_money_str,"%s -- %d euros", playerNames[i], _money[i]);

    sprintf(points_str, "%d points", points[i]);
    /* If player is bust */
    if (Bust(points[i])) {
        strcat(points_str, " (BUST)"); // if bust concatnate BUST to string
    }

    RenderText(playerRect.x+20, playerRect.y-50, name_money_str, serif, playerColor, _renderer);
    RenderText(playerRect.x+20, playerRect.y-30, points_str, serif, playerColor, _renderer); // renders points

    /* Checks if house_points is not 0. If so, renders, above the first player,
     * the house points
     */
    if (i == 0 && house_points > 0) {
        sprintf(house_points_str, "House points: %d", house_points);
        RenderText(playerRect.x+40, playerRect.y-125, house_points_str, serif, &white, _renderer);
    }

    SDL_RenderDrawRect(_renderer, &playerRect);
  }

  // destroy everything
  SDL_DestroyTexture(table_texture);

  // close font
  TTF_CloseFont(serif);
}

/**
* RenderHouseCards: Renders cards of the house
* \param _house vector with the house cards
* \param _pos_house_hand position of the vector _house with valid card IDs
* \param _cards vector with all loaded card images
* \param _renderer renderer to handle all rendering in a window
*/
void RenderHouseCards(int _house[], int _pos_house_hand, SDL_Surface **_cards, SDL_Renderer* _renderer)
{
  int card = 0, x = 0, y = 0;
  int div = WIDTH_WINDOW/CARD_WIDTH;

  // drawing all house cards
  for (card = 0; card < _pos_house_hand; card++)
  {
    // calculate its position
    x = (div/2-_pos_house_hand/2+card)*CARD_WIDTH + 15;
    y = (int) (0.26f*HEIGHT_WINDOW);
    // render it !
    RenderCard(x, y, _house[card], _cards, _renderer);
  }
  // just one card ?: draw a card face down
  if (_pos_house_hand == 1)
  {
    x = (div/2-_pos_house_hand/2+1)*CARD_WIDTH + 15;
    y = (int) (0.26f*HEIGHT_WINDOW);
    RenderCard(x, y, MAX_DECK_SIZE, _cards, _renderer);
  } else if (_pos_house_hand > 1) {

  }

}

/**
* RenderPlayerCards: Renders the hand, i.e. the cards, for each player
* \param _player_cards 2D array with the player cards, 1st dimension is the player ID
* \param _pos_player_hand array with the positions of the valid card IDs for each player
* \param _cards vector with all loaded card images
* \param _renderer renderer to handle all rendering in a window
*/
void RenderPlayerCards(int _player_cards[][MAX_CARD_HAND], int _pos_player_hand[], SDL_Surface **_cards, SDL_Renderer* _renderer)
{
  int pos = 0, x = 0, y = 0, num_player = 0, card = 0;

  // for every card of every player
  for ( num_player = 0; num_player < MAX_PLAYERS; num_player++)
  {
    for ( card = 0; card < _pos_player_hand[num_player]; card++)
    {
      // draw all cards of the player: calculate its position: only 4 positions are available !
      pos = card % 4;
      x = (int) num_player*((0.95f*WIDTH_WINDOW)/4-5)+(card/4)*12+15;
      y = (int) (0.55f*HEIGHT_WINDOW)+10;
      if ( pos == 1 || pos == 3) x += CARD_WIDTH + 30;
      if ( pos == 2 || pos == 3) y += CARD_HEIGHT+ 10;
      // render it !
      RenderCard(x, y, _player_cards[num_player][card], _cards, _renderer);
    }
  }
}

/**
* RenderCard: Draws one card at a certain position of the window, based on the card code
* \param _x X coordinate of the card position in the window
* \param _y Y coordinate of the card position in the window
* \param _num_card card code that identifies each card
* \param _cards vector with all loaded card images
* \param _renderer renderer to handle all rendering in a window
*/
void RenderCard(int _x, int _y, int _num_card, SDL_Surface **_cards, SDL_Renderer* _renderer)
{
  SDL_Texture *card_text = NULL;
  SDL_Rect boardPos = {0, 0, 0, 0};

  // area that will be occupied by each card
  boardPos.x = _x;
  boardPos.y = _y;
  boardPos.w = CARD_WIDTH;
  boardPos.h = CARD_HEIGHT;

  // render it !
  card_text = SDL_CreateTextureFromSurface(_renderer, _cards[_num_card]);
  SDL_RenderCopy(_renderer, card_text, NULL, &boardPos);
}

/**
* LoadCards: Loads all images of the cards
* \param _cards vector with all loaded card images
*/
void LoadCards(SDL_Surface **_cards)
{
  int i = 0;
  char filename[STRING_SIZE] = { 0 };

  // loads all cards to an array
  for (i = 0 ; i < MAX_DECK_SIZE; i++ )
  {
    // create the filename !
    sprintf(filename, ".//cartas//carta_%02d.png", i+1);
    // loads the image !
    _cards[i] = IMG_Load(filename);
    // check for errors: deleted files ?
    if (_cards[i] == NULL)
    {
      printf("Unable to load image: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }
  // loads the card back
  _cards[i] = IMG_Load(".//cartas//carta_back.jpg");
  if (_cards[i] == NULL)
  {
    printf("Unable to load image: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}


/**
* UnLoadCards: unloads all card images of the memory
* \param _cards vector with all loaded card images
*/
void UnLoadCards(SDL_Surface **_array_of_cards)
{
  // unload all cards of the memory: +1 for the card back
  for (int i = 0 ; i < MAX_DECK_SIZE + 1; i++ )
  {
    SDL_FreeSurface(_array_of_cards[i]);
  }
}

/**
* RenderLogo function: Renders the IST Logo on the window screen
* \param x X coordinate of the Logo
* \param y Y coordinate of the Logo
* \param _logoIST surface with the IST logo image to render
* \param _renderer renderer to handle all rendering in a window
*/
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
  SDL_Texture *text_IST = NULL;
  SDL_Rect boardPos  = {0, 0, 0, 0};

  // space occupied by the logo
  boardPos.x = x;
  boardPos.y = y;
  boardPos.w = _logoIST->w;
  boardPos.h = _logoIST->h;

  // render it
  text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
  SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

  // destroy associated texture !
  SDL_DestroyTexture(text_IST);
  return _logoIST->h;
}

/**
* RenderText function: Renders the IST Logo on the window screen
* \param x X coordinate of the text
* \param y Y coordinate of the text
* \param text string where the text is written
* \param font TTF font used to render the text
* \param _renderer renderer to handle all rendering in a window
*/
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
  SDL_Surface *text_surface = NULL;
  SDL_Texture *text_texture = NULL;
  SDL_Rect solidRect = {0, 0, 0, 0};

  solidRect.x = x;
  solidRect.y = y;
  // create a surface from the string text with a predefined font
  text_surface = TTF_RenderText_Blended(_font,text,*_color);
  if(!text_surface)
  {
    printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  // create texture
  text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
  // obtain size
  SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
  // render it !
  SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);

  SDL_DestroyTexture(text_texture);
  SDL_FreeSurface(text_surface);
  return solidRect.h;
}

/**
* InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
* \param width width in px of the window
* \param height height in px of the window
* \param _img surface to be created with the table background and IST logo
* \param _window represents the window of the application
* \param _renderer renderer to handle all rendering in a window
*/
void InitEverything(int width, int height, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
  InitSDL();
  InitFont();
  *_window = CreateWindow(width, height);
  *_renderer = CreateRenderer(width, height, *_window);

  // load the table texture
  _img[0] = IMG_Load("table_texture.png");
  if (_img[0] == NULL)
  {
    printf("Unable to load image: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // load IST logo
  _img[1] = SDL_LoadBMP("ist_logo.bmp");
  if (_img[1] == NULL)
  {
    printf("Unable to load bitmap: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

}

/**
* InitSDL: Initializes the SDL2 graphic library
*/
void InitSDL()
{
  // init SDL library
  if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
  {
    printf(" Failed to initialize SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

/**
* InitFont: Initializes the SDL2_ttf font library
*/
void InitFont()
{
  // Init font library
  if(TTF_Init()==-1)
  {
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
}

/**
* CreateWindow: Creates a window for the application
* \param width width in px of the window
* \param height height in px of the window
* \return pointer to the window created
*/
SDL_Window* CreateWindow(int width, int height)
{
  SDL_Window *window = NULL;
  // init window
  window = SDL_CreateWindow( "BlackJack", WINDOW_POSX, WINDOW_POSY, width+EXTRASPACE, height, 0 );
  // check for error !
  if ( window == NULL )
  {
    printf("Failed to create window : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  return window;
}

/**
* CreateRenderer: Creates a renderer for the application
* \param width width in px of the window
* \param height height in px of the window
* \param _window represents the window for which the renderer is associated
* \return pointer to the renderer created
*/
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
  SDL_Renderer *renderer = NULL;
  // init renderer
  renderer = SDL_CreateRenderer( _window, -1, 0 );

  if ( renderer == NULL )
  {
    printf("Failed to create renderer : %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // set size of renderer to the same as window
  SDL_RenderSetLogicalSize( renderer, width+EXTRASPACE, height );

  return renderer;
}
