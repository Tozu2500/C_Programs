#ifndef SCORE_H
#define SCORE_H

typedef struct {
    int leftScore;
    int rightScore;
    int matchesPlayed;
    int leftWins;
    int rightWins;
    int totalBounces;
    int longestRally;
    int currentRally;
    int bestOf;
    int gameNumber;
} Score;

void initScore(Score* score);
void incrementLeftScore(Score* score);
void incrementRightScore(Score* score);
void resetScore(Score* score);
void updateStats(Score* score, int bounces);
void newMatch(Score* score);
void setBestOf(Score* score, int games);
int getLeftScore(Score* score);
int getRightScore(Score* score);
int getMatchesPlayed(Score* score);
int getLeftWins(Score* score);
int getRightWins(Score* score);
int getTotalBounces(Score* score);
int getLongestRally(Score* score);
int getCurrentRally(Score* score);
int isMatchWon(Score* score);
int getMatchWinner(Score* score);
void incrementRally(Score* score);
void resetRally(Score* score);

#endif // SCORE_H