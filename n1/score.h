#ifndef _N1_SCORE_H_
#define _N1_SCORE_H_

// public:
extern byte N1NumberOfPlayers;
extern byte N1ThisPlayerNumber;
extern int N1PartialScores[N1_MAX_PLAYERS];
extern int N1TotalScores[N1_MAX_PLAYERS];

// private:
void DoPartialScores();  // on the Real.seconds?


#endif // _N1_SCORE_H_
