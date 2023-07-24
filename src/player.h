#ifndef PLAYERFUSE_H
#define PLAYERFUSE_H

// Enum of a type of player
// 0 - Human -> Takes input from stdin
// 1 - Random -> Randomly chooses a card from hand
// 2- AI -> Chooses the card that has the highest probability of winning
typedef enum {
    HUMAN, 
    RANDOM, 
    AI
} PlayerType;

typedef enum {
    PLAYER1,
    PLAYER2
} Player;

typedef struct {
    Player player;
    int score;
    PlayerType type;
    char* name;
    int wins;
    int losses;
} PlayerInfo;

#endif
