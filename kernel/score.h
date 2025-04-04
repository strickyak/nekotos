#ifndef _KERNEL_SCORE_H_
#define _KERNEL_SCORE_H_

// public:
extern gbyte gNumberOfPlayers;
extern gbyte gThisPlayerNumber;
extern int gPartialScores[gMAX_PLAYERS];
extern int gTotalScores[gMAX_PLAYERS];

// private:
void SendPartialScores();  // on the Real.seconds.


#endif // _KERNEL_SCORE_H_
