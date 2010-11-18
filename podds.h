#ifndef PODDS_H_
#define PODDS_H_

#include "carddeck.h"
#include "common.h"
#include "handevaluator.h"
#include "randomgenerator.h"
#include "userinp.h"


//typedef struct simulation_result {
//	unsigned int rounds;
//	unsigned int player; /* Number of opponents (1 for heads-up) */
//	unsigned int won;
//	unsigned int split;
//	char card1, card2;
//	unsigned int winclasses[10], looseclasses[10];
//} simulation_result; 

char handBuf[(PODDS_MAX_PLAYERS + 1) * PODDS_NUM_DEALT_CARDS];
char* playedHands[PODDS_MAX_PLAYERS + 1];

//otherHands has an array entry for each other player, with a set of possible hands for each player
//numOtherHands laid out by player, specifying the number of possible hands that player has
int runSimulation(char* hands, int numPlayers, int numOtherHands[], char* flop, int numFlopped, simulation_result* results); 
int runSimulation_ncards(char* hands, int numPlayers, int numHands[], char* flop, int numFlopped, int nCards, simulation_result* results);
int runSimulation_ncards_dead(char* hands, int numPlayers, int numHands[], char* flop, int numFlopped, 
							  char* dead, int numDead, int nCards, simulation_result* results);


int runSimulation_ncards_dead_weighted(char* hands, int numPlayers, int numHands[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER], 
									   char* flop, int numFlopped, 
									   char* dead, int numDead, 
									   int nCards, int numHandCodes[PODDS_MAX_PLAYERS + 1], 
									   int handCodesWeights[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER], simulation_result* results);


//this method simply brings the functionality back together again
int runSimulation_internals(int numPlayers, char* flop, int numFlopped, 
							char* dead, int numDead, int nCards, simulation_result* results);


//int ctoi(char* card);

//void printResults(simulation_result* result, int numPlayers);

#endif /*PODDS_H_*/
