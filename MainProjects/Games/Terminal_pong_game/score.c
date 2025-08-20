#include "score.h"
#include "types.h"

void initScore(Score* score) {
    score->leftScore = 0;
    score->rightScore = 0;
    score->matchesPlayed = 0;
    score->leftWins = 0;
    score->rightWins = 0;
    score->totalBounces = 0;
    score->longestRally = 0;
    score->currentRally = 0;
    score->bestOf = 1;
    score->gameNumber = 1;
}

void incrementLeftScore(Score* score) {
    score->leftScore++;
    if (score->currentRally > score->longestRally) {
        score->longestRally = score->currentRally;
    }
}

void incrementRightScore(Score* score) {
    score->rightScore++;
    if (score->currentRally > score->longestRally) {
        score->longestRally = score->currentRally;
    }
}

void resetScore(Score* score) {
    score->leftScore = 0;
    score->rightScore = 0;
    score->currentRally = 0;
}

void updateStats(Score* score, int bounces) {
    score->totalBounces += bounces;
}

void newMatch(Score* score) {
    score->matchesPlayed++;

    if (score->leftScore > score->rightScore) {
        score->leftWins++;
    } else {
        score->rightWins++;
    }

    resetScore(score);
    score->gameNumber++;
}

void setBestOf(Score* score, int games) {
    score->bestOf = games;
}

int getLeftScore(Score* score) {
    return score->leftScore;
}

int getRightScore(Score* score) {
    return score->rightScore;
}

int getMatchesPlayed(Score* score) {
    return score->matchesPlayed;
}

int getLeftWins(Score* score) {
    return score->leftWins;
}

int getRightWins(Score* score) {
    return score->rightWins;
}

int getTotalBounces(Score* score) {
    return score->totalBounces;
}

int getLongestRally(Score* score) {
    return score->longestRally;
}

int getCurrentRally(Score* score) {
    return score->currentRally;
}

int isMatchWon(Score* score) {
    int winsNeeded = (score->bestOf + 1) / 2;
    return (score->leftWins >= winsNeeded || score->rightWins >= winsNeeded);
}

int getMatchWinner(Score* score) {
    int winsNeeded = (score->bestOf + 1) / 2;
    if (score->leftWins >= winsNeeded) {
        return PLAYER_LEFT;
    } else if (score->rightWins >= winsNeeded) {
        return PLAYER_RIGHT;
    }

    return 0;
}

void incrementRally(Score* score) {
    score->currentRally++;
    score->totalBounces++;
}

void resetRally(Score* score) {
    score->currentRally = 0;
}