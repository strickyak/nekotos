#ifndef _NEKOT1_SCORE_H_
#define _NEKOT1_SCORE_H_

// public:
extern byte gNumberOfPlayers;
extern byte gThisPlayerNumber;
extern int gPartialScores[gMAX_PLAYERS];
extern int gTotalScores[gMAX_PLAYERS];

// private:
void DoPartialScores();  // on the Real.seconds?


#endif // _NEKOT1_SCORE_H_
