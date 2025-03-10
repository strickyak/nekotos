#ifndef _g_SCORE_H_
#define _g_SCORE_H_

// public:
extern byte gNumberOfPlayers;
extern byte gThisPlayerNumber;
extern int gPartialScores[g_MAX_PLAYERS];
extern int gTotalScores[g_MAX_PLAYERS];

// private:
void DoPartialScores();  // on the Real.seconds?


#endif // _g_SCORE_H_
