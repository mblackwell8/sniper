#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "podds.h"


/*
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 27.67      0.70     0.70  1000000     0.00     0.00  runSimulation
 26.88      1.38     0.68  6000000     0.00     0.00  parse_card
 15.81      1.78     0.40  4909179     0.00     0.00  remove_card
 13.04      2.11     0.33  1000000     0.00     0.00  init_deck
  9.49      2.35     0.24  1226790     0.00     0.00  calculate_equivalence_class
  3.16      2.43     0.08  6953829     0.00     0.00  draw_card
  2.37      2.49     0.06  2044650     0.00     0.00  get_random_card
  0.79      2.51     0.02  2044650     0.00     0.00  draw_random_card
  0.40      2.52     0.01        1    10.00    10.00  load_dag
  0.40      2.53     0.01                             main
  0.00      2.53     0.00       13     0.00     0.00  getPossibleHands_preciseSingleHandCode
  0.00      2.53     0.00        8     0.00     0.00  getPossibleHands_looseSingleHandCode
  0.00      2.53     0.00        3     0.00     0.00  getPossibleHands_csvHandCode
  0.00      2.53     0.00        3     0.00     0.00  getPossibleHands_possRangeHandCode
  0.00      2.53     0.00        3     0.00     0.00  toUpper
  0.00      2.53     0.00        2     0.00     0.00  getRankRange
  0.00      2.53     0.00        1     0.00     0.00  getHandsFromUser
  0.00      2.53     0.00        1     0.00     0.00  getNoLimitUpperBound
  0.00      2.53     0.00        1     0.00    10.00  handeval_init
  0.00      2.53     0.00        1     0.00     0.00  init_random
  0.00      2.53     0.00        1     0.00     0.00  load_equivalenceclasses
  0.00      2.53     0.00        1     0.00     0.00  printResults
  
  then when i stopped init_deck on each simulation, this happened...
  probably because the deck gets very shuffled 
  
    %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 44.07      1.45     1.45  4908872     0.00     0.00  remove_card
 22.80      2.20     0.75  1000000     0.00     0.00  runSimulation
 18.09      2.79     0.59  6000000     0.00     0.00  parse_card
  8.51      3.08     0.28  1227171     0.00     0.00  calculate_equivalence_class
  2.74      3.17     0.09  6954157     0.00     0.00  draw_card
  2.43      3.25     0.08  2045285     0.00     0.00  get_random_card
  0.46      3.26     0.01  2045285     0.00     0.00  draw_random_card
  0.30      3.27     0.01        1    10.00    10.00  init_deck
  0.30      3.28     0.01        1    10.00    10.00  load_dag
  0.30      3.29     0.01                             hand_to_str
  0.00      3.29     0.00  1000000     0.00     0.00  reset_deck
  0.00      3.29     0.00       13     0.00     0.00  getPossibleHands_preciseSingleHandCode
  0.00      3.29     0.00        8     0.00     0.00  getPossibleHands_looseSingleHandCode
  0.00      3.29     0.00        3     0.00     0.00  getPossibleHands_csvHandCode
  0.00      3.29     0.00        3     0.00     0.00  getPossibleHands_possRangeHandCode
  0.00      3.29     0.00        3     0.00     0.00  toUpper
  0.00      3.29     0.00        2     0.00     0.00  getRankRange
  0.00      3.29     0.00        1     0.00     0.00  getHandsFromUser
  0.00      3.29     0.00        1     0.00     0.00  getNoLimitUpperBound
  0.00      3.29     0.00        1     0.00    10.00  handeval_init
  0.00      3.29     0.00        1     0.00     0.00  init_random
  0.00      3.29     0.00        1     0.00     0.00  load_equivalenceclasses
  0.00      3.29     0.00        1     0.00     0.00  printResults
  
  //then when i ditched the deck altogether and just used my simple isTaken array...
  //(i also made a slight improvement to parse_card by breaking found loops
  
    %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 46.94      0.69     0.69  1000000     0.00     0.00  runSimulation
 28.57      1.11     0.42  6000000     0.00     0.00  parse_card
 21.43      1.43     0.32  1225995     0.00     0.00  calculate_equivalence_class
  2.04      1.46     0.03        1    30.00    30.00  load_dag
  0.68      1.47     0.01                             main
  0.34      1.47     0.01                             hand_to_str
  0.00      1.47     0.00       13     0.00     0.00  getPossibleHands_preciseSingleHandCode
  0.00      1.47     0.00        8     0.00     0.00  getPossibleHands_looseSingleHandCode
  0.00      1.47     0.00        3     0.00     0.00  getPossibleHands_csvHandCode
  0.00      1.47     0.00        3     0.00     0.00  getPossibleHands_possRangeHandCode
  0.00      1.47     0.00        3     0.00     0.00  toUpper
  0.00      1.47     0.00        2     0.00     0.00  getRankRange
  0.00      1.47     0.00        1     0.00     0.00  getHandsFromUser
  0.00      1.47     0.00        1     0.00     0.00  getNoLimitUpperBound
  0.00      1.47     0.00        1     0.00    30.00  handeval_init
  0.00      1.47     0.00        1     0.00     0.00  init_random
  0.00      1.47     0.00        1     0.00     0.00  load_equivalenceclasses
  0.00      1.47     0.00        1     0.00     0.00  printResults
  
  
*/


//deck_struct deck;

/*
int main(int argc, char* argv[]) {
	
//	test();
	
  //lay out some memory
  //must be at least (PODDS_MAX_PLAYERS + 1) * PODDS_MAX_HANDS_PER_PLAYER * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD
  char buf[1024 * 8];
  int numHands[PODDS_MAX_PLAYERS + 1];

  // including me
  int numPlayers = getHandsFromUser(buf, numHands);
  char* hands = buf;
  
  //test player has five hands of two cards each (Ad9d+)
  //char* testPlayer1 = "Ad\09d\0Ad\0Td\0Ad\0Jd\0Ad\0Qd\0Ad\0Kd\0";
  //otherHands = testPlayer1;
  //int numPlayers = 1;
  //numOtherHands[0] = 5;

  //int numSims = getNumSimsFromUser(); 
  int numSims = 1000000;

  int i;
  simulation_result results[PODDS_MAX_PLAYERS + 1];
  
  int p;
  for (p = 0; p < numPlayers; p++) {
  	results[p].rounds = 0;
  	results[p].player = numPlayers;
  	results[p].won = 0;
  	results[p].split = 0;
  }	
  
  clock_t start, end;
  double elapsed;
  start = clock();
  
  //init_deck(&deck);
  handeval_init();
  init_random ( time(NULL) );
  
  for (i = 0; i < numSims; i++) {
    runSimulation(hands, numPlayers, numHands, results);
  }
  
  end = clock();
  elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  
  printf("Simulation took %5.3f seconds for %d iters:\n", elapsed, numSims);
  printResults(results, numPlayers);
  
  int offset = 0;
  for (p = 0; p < numPlayers; p++) {
    printf("Player %d hands: ", p);
    printHands(hands + offset, numHands[p]);
    offset += numHands[p] * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD;
  }
  
  return 0;

}
*/

int runSimulation(char* hands, int numPlayers, int numHands[], char* flop, int numFlopped, simulation_result* results) {
	int nCards = 7;
	return runSimulation_ncards(hands, numPlayers, numHands, flop, numFlopped, nCards, results);
}

int runSimulation_ncards(char* hands, int numPlayers, int numHands[], char* flop, int numFlopped, int nCards, simulation_result* results) {
	int nDead = 0;
	return runSimulation_ncards_dead(hands, numPlayers, numHands, flop, numFlopped, NULL, nDead, nCards, results);
}

//hands: ptr to the start of a set of characters.  each "hand" is two card plus null = 6 chars (card, suit, null)
//numPlayers: the number of players
//numHands: array numPlayers long that has the number of possible hands for each player
//flop: ptr to start of flop cards.  each card = 3 chars
int runSimulation_ncards_dead(char* hands, int numPlayers, int numHands[], char* flop, int numFlopped, 
							  char* dead, int numDead, int nCards, simulation_result* results) {
	
	assert_local (nCards >= PODDS_FLOP_CARDS_PER_HAND && nCards <= PODDS_RIVER_CARDS_PER_HAND && nCards >= PODDS_NUM_DEALT_CARDS)
	assert_local (nCards >= PODDS_NUM_DEALT_CARDS + numFlopped)
	assert_local (numFlopped <= PODDS_NUM_CARDS);
	assert_local (numDead <= PODDS_NUM_CARDS);
	assert_local (numDead == 0 || dead != NULL);
	
	//for some bazaar f**king reason, which has taken many hours to debug, if these vars are at the method level
	//the release mode optimisation ends up overwriting the first four bites of handBuf, just prior to assigning
	//the seven card hand... but putting them into the global namespace fixes the problem
	
	//char handBuf[(PODDS_MAX_PLAYERS + 1) * PODDS_NUM_DEALT_CARDS];
	//char* playedHands[PODDS_MAX_PLAYERS + 1];
	
	//choose a random hand from each of the hands
	//and store the cards as single chars, bitmasked etc per handeval method
	int p, offset = 0;
	for (p = 0; p < numPlayers; p++) {
		
		//unnecessary optimisation?? prevent an unnecessary call to rand
		int h = (numHands[p] == 1 ? 0 : (rand() % numHands[p]));
		//printf("player %d hand number %d\n", p, h);
		char* hand = (hands + ((offset + h) * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD));
		
		//hack: will not work if PODDS_NUM_DEALT_CARDS != 2 or PODDS_CHARS_PER_CARD != 3
		
		//deal with random ranks or suits
//		char randHand[PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD];
//		memcpy(randHand, hand, PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
//		if (hand[0] == RANDOM_RANK)
//			hand[0] = CARD[rand() % PODDS_NUM_CARDS_PER_SUIT];
//		if (hand[1] == RANDOM_SUIT)
//			hand[1] = COLOR[rand() % PODDS_NUM_SUITS];
//		//ignore the null terminator at posn 2
//		if (hand[3] == RANDOM_RANK)
//			hand[3] = CARD[rand() % PODDS_NUM_CARDS_PER_SUIT];
//		if (hand[4] == RANDOM_SUIT)
//			hand[4] = COLOR[rand() % PODDS_NUM_SUITS];
		
		//if (p == 0) printf("%s%s\n", hand, hand + 3);
		
		handBuf[p * PODDS_NUM_DEALT_CARDS] = parse_card(hand);
		//printf("first card %s = %d\n", hand, handBuf[p * PODDS_NUM_DEALT_CARDS]);
		handBuf[p * PODDS_NUM_DEALT_CARDS + 1] = parse_card(hand + PODDS_CHARS_PER_CARD);
		//printf("second card %s = %d\n", hand + PODDS_CHARS_PER_CARD, handBuf[p * PODDS_NUM_DEALT_CARDS + 1]);
		
		//put the random hand back the way it was
//		memcpy(hand, randHand, PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
		
		playedHands[p] = (handBuf + (p * PODDS_NUM_DEALT_CARDS));
		
		offset += numHands[p];
	}
	
	//reset_deck(&deck);
	
	return runSimulation_internals(numPlayers, flop, numFlopped, dead, numDead, nCards, results);
}


int runSimulation_ncards_dead_weighted(char* hands, int numPlayers, int numHands[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER], 
									   char* flop, int numFlopped, 
									   char* dead, int numDead, 
									   int nCards, int numHandCodes[PODDS_MAX_PLAYERS + 1], 
									   int handCodesWeights[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER], simulation_result* results) {
	
	assert_local (nCards >= PODDS_FLOP_CARDS_PER_HAND && nCards <= PODDS_RIVER_CARDS_PER_HAND && nCards >= PODDS_NUM_DEALT_CARDS);
	assert_local (nCards >= PODDS_NUM_DEALT_CARDS + numFlopped);
	
	//for some bazaar f**king reason, which has taken many hours to debug, if these vars are at the method level
	//the release mode optimisation ends up overwriting the first four bites of handBuf, just prior to assigning
	//the seven card hand... but putting them into the global namespace fixes the problem
	
	//char handBuf[(PODDS_MAX_PLAYERS + 1) * PODDS_NUM_DEALT_CARDS];
	//char* playedHands[PODDS_MAX_PLAYERS + 1];
	
	//choose a random hand from each of the hands
	//and store the cards as single chars, bitmasked etc per handeval method
	int p, offset = 0, hc = 0;
	for (p = 0; p < numPlayers; p++) {
		int totalWeights = 0;
		for (hc = 0; hc < numHandCodes[p]; hc++)
			totalWeights += handCodesWeights[p][hc];
		int randHandCode = (totalWeights == 1 ? 0 : (rand() % totalWeights));
		hc = 0;
		while (hc < numHandCodes[p] && randHandCode > (handCodesWeights[p][hc] - 1)) {
			offset += numHands[p][hc];
			randHandCode -= handCodesWeights[p][hc];
			hc++;
		}
		
		//unnecessary optimisation?? prevent an unnecessary call to rand
		int h = (numHands[p][hc] == 1 ? 0 : (rand() % numHands[p][hc]));
		//printf("player %d hand number %d\n", p, h);
		char* hand = (hands + ((offset + h) * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD));
		
		//hack: will not work if PODDS_NUM_DEALT_CARDS != 2 or PODDS_CHARS_PER_CARD != 3
		
//		//deal with random ranks or suits
//		char randHand[PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD];
//		memcpy(randHand, hand, PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
//		if (hand[0] == RANDOM_RANK)
//			hand[0] = CARD[rand() % PODDS_NUM_CARDS_PER_SUIT];
//		if (hand[1] == RANDOM_SUIT)
//			hand[1] = COLOR[rand() % PODDS_NUM_SUITS];
//		//ignore the null terminator at posn 2
//		if (hand[3] == RANDOM_RANK)
//			hand[3] = CARD[rand() % PODDS_NUM_CARDS_PER_SUIT];
//		if (hand[4] == RANDOM_SUIT)
//			hand[4] = COLOR[rand() % PODDS_NUM_SUITS];
		
		handBuf[p * PODDS_NUM_DEALT_CARDS] = parse_card(hand);
		//printf("first card %s = %d\n", hand, handBuf[p * PODDS_NUM_DEALT_CARDS]);
		handBuf[p * PODDS_NUM_DEALT_CARDS + 1] = parse_card(hand + PODDS_CHARS_PER_CARD);
		//printf("second card %s = %d\n", hand + PODDS_CHARS_PER_CARD, handBuf[p * PODDS_NUM_DEALT_CARDS + 1]);
		
		//put the random hand back the way it was
//		memcpy(hand, randHand, PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
		
		playedHands[p] = (handBuf + (p * PODDS_NUM_DEALT_CARDS));
		//printf("Hand %d: %d %d\n", p, playedHands[p][0], playedHands[p][1]);
		while (hc < numHandCodes[p])
			offset += numHands[p][hc++];
	}
	
	return runSimulation_internals(numPlayers, flop, numFlopped, dead, numDead, nCards, results);
	
}

int runSimulation_internals(int numPlayers, char* flop, int numFlopped, 
							char* dead, int numDead, int nCards, simulation_result* results) {
	//reset_deck(&deck);
	int f, d, i, c, p;
	
	unsigned char isTaken[PODDS_NUM_CARDS];
	for (i = 0; i < PODDS_NUM_CARDS; i++) isTaken[i] = 0;
	
	//deal any dead cards from the deck
	for (d = 0; d < numDead; d++) {
		char* dCard = dead + (d * PODDS_CHARS_PER_CARD);
		char dead = parse_card(dCard);
		if (isTaken[(int)dead])
			//can't run this simulation
			return 1;
		
		//remove_card(&deck, *card);
		isTaken[(int)dead] = 1;  
	}
	
	//deal these cards from the deck
	//abort if the deck can't support the hand (ie. card collision)
	//start on a random player so that we don't advantage early player card selection
	int firstPlayerIx = rand() % numPlayers;
	for (p = 0; p < numPlayers; p++) {
		for (c = 0; c < PODDS_NUM_DEALT_CARDS; c++) {
			char* card = playedHands[firstPlayerIx] + c ;
			if (isTaken[(int)*card])
				//can't run this simulation
				return 1;
			
			//remove_card(&deck, *card);
			isTaken[(int)*card] = 1;  
		}
		if (++firstPlayerIx == numPlayers)
			firstPlayerIx = 0;
	}
	
	//for (p = 0; p < numPlayers; p++)
	//printf("2: Hand %d: %d %d\n", p, playedHands[p][0], playedHands[p][1]);
	
	//deal a further 5 flop cards
	char flopCards[PODDS_NUM_FLOP_CARDS];
	
	//deterministic flop cards first...
	for (f = 0; f < numFlopped && f < (nCards - PODDS_NUM_DEALT_CARDS); f++) {
		char* fCard = flop + (f * PODDS_CHARS_PER_CARD);
		flopCards[f] = parse_card(fCard);
		if (isTaken[(int)flopCards[f]])
			//can't run this simulation
			return 1;
		
		//remove_card(&deck, *card);
		isTaken[(int)flopCards[f]] = 1;  
	}
	
	//for (p = 0; p < numPlayers; p++)
	//printf("3: Hand %d: %d %d\n", p, playedHands[p][0], playedHands[p][1]);
	
	//... then random flop cards, as required
	for (; f < (nCards - PODDS_NUM_DEALT_CARDS); f++) {
		//flopCards[f] = draw_random_card(&deck);
		int ic = -1;
		while (ic < 0 || isTaken[ic] == 1) {
			ic = (rand() % PODDS_NUM_CARDS);
		}
		isTaken[ic] = 1;
		flopCards[f] = (char)ic;
	}
	
    
	//handeval_eq_class* handeval[PODDS_MAX_PLAYERS + 1];
	char sevenCards[PODDS_NUM_DEALT_CARDS + PODDS_NUM_FLOP_CARDS];
	int winnerIx = -1;
	unsigned int currentWinningScore = PODDS_LOWEST_HAND_SCORE; //
	int ties[PODDS_MAX_PLAYERS + 1];
	int t = 0;
	while (t++ < PODDS_MAX_PLAYERS + 1) ties[t] = 0;
	
	for (p = 0; p < numPlayers; p++) {
		
		//printf("Hand %d: %d %d\n", p, playedHands[p][0], playedHands[p][1]);
		
		for (c = 0; c < PODDS_NUM_DEALT_CARDS; c++) sevenCards[c] = playedHands[p][c];
		for (c = 0; c < (nCards - PODDS_NUM_DEALT_CARDS); c++) sevenCards[c + PODDS_NUM_DEALT_CARDS] = flopCards[c];
		//  	printf("Deal is %s, ", hand_to_str(playedHands[p], 2));
		//  	printf("Flop is %s, ", hand_to_str(flopCards, 5));
		//  	printf("Hand is %s, ", hand_to_str(sevenCards, 7));
		results[p].handeval = calculate_equivalence_class_ncards(sevenCards, nCards);
		//  	if (handeval[p] == NULL) {
		//  	  printf("which bombed!\n");
		//  	}
		//  	
		//  	printf("which evals to %s\n", handeval[p]->desc);
		
		if (results[p].handeval->id < currentWinningScore) {
			winnerIx = p;
			currentWinningScore = results[p].handeval->id;
			
			//reset all previous ties to zero - they are beaten
			t = 0;
			while (t < p) ties[t++] = 0;
			
			//this hand must at least tie at this stage
			ties[p] = 1;
		}
		else if (results[p].handeval->id == currentWinningScore) {
			//set this player as tie-ing
			ties[p] = 1;
			
			//no winner
			winnerIx = -1;
		}
	}
	
	for (p = 0; p < numPlayers; p++) {
		results[p].isWinner = 0;
		results[p].isSplitter = 0;
	}
	
	//find winner/ties, and all players results
	if (winnerIx != -1) {
		results[winnerIx].won++;
		results[winnerIx].isWinner = 1;
	}
	else {
		int numSplitters = 0;
		for (p = 0; p < numPlayers; p++) {
			if (ties[p] == 1) {
				results[p].split++;
				results[p].isSplitter = 1;
				numSplitters++;
			}
		}
		for (p = 0; p < numPlayers; p++) {
			if (results[p].isSplitter)
				results[p].splitEquity += 1.0 / numSplitters;
		}
	}
	
	//update all player records
	for (p = 0; p < numPlayers; p++) {
		results[p].rounds++;
		results[p].card1 = playedHands[p][0];
		results[p].card2 = playedHands[p][1];
		memcpy(results[p].flopCards, flopCards, PODDS_NUM_FLOP_CARDS);
	}	
	
	return 0;	
}


//int ctoi(char* card) {
//	
//}

  //  typedef struct simulation_result {
//	unsigned int rounds;
//	unsigned int player; /* Number of opponents (1 for heads-up) */
//	unsigned int won;
//	unsigned int split;
//	char card1, card2;
//	unsigned int winclasses[10], looseclasses[10];
//  } simulation_result; 

void printResults(simulation_result* results, int numPlayers) {
	int i;
	for (i = 0; i < numPlayers; i++) {
		double equity = ((0.5 * results[i].split) + results[i].won) / results[i].rounds;
		printf("Player %d: Wins %d, Splits %d, Rounds %d, Equity: %5.3f\n", 
			i, results[i].won, results[i].split, results[i].rounds, equity);
	}
	
}

