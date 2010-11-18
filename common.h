#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>

/** Helper macros */
#define min(A,B) (((A)<(B))?(A):(B))
#ifdef DEBUG
	#define assert_local(A) if (!(A)) { printf("assert_localion failure at %s:%u: %s\n",  __FILE__, __LINE__, #A); exit(-1);}
#else
	#define assert_local(A)
#endif 

/** Card specific constants */
//#define HANDTYPECLASSCOUNT 9
static char CARD[13] = {'2', '3', '4' ,'5', '6', '7', '8', '9', 'T' , 'J', 'Q', 'K', 'A'};
static char COLOR[4] = {'h', 's', 'c', 'd'};
//static char COLOR_UPPER[4] = {'H', 'S', 'C', 'D'};
//static char *HANDTYPE[HANDTYPECLASSCOUNT] = {"HC", "1P", "2P", "3K", "S", "F", "FH", "4K", "SF"};

//(PODDS_MAX_PLAYERS - 1) * PODDS_MAX_HANDS_PER_PLAYER * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD
//must be < 1024 or need to increase buf
#define PODDS_MAX_PLAYERS 				3 //excluding me
#define PODDS_MAX_HANDS_PER_PLAYER 		2048 //1326, but move up to power of 2
#define PODDS_NUM_DEALT_CARDS		 	2
#define PODDS_CHARS_PER_CARD		 	3

#define PODDS_NUM_FLOP_CARDS		 	5
#define PODDS_RIVER_CARDS_PER_HAND		7 //PODDS_NUM_FLOP_CARDS + PODDS_NUM_DEALT_CARDS
#define PODDS_TURN_CARDS_PER_HAND		6
#define PODDS_FLOP_CARDS_PER_HAND		5
#define PODDS_NUM_CARDS 				52
#define PODDS_NUM_HAND_CARDS 			5
#define PODDS_LOWEST_HAND_SCORE 		7462
#define PODDS_NUM_SUITS 				4
#define PODDS_NUM_CARDS_PER_SUIT		13

#define PODDS_MAX_HANDCODES_PER_PLAYER	32  //including % hands (actually is 19)
#define PODDS_MAX_CHARS_PER_HAND		16  //the maximum length an individual hand string can be (something like 'AJs-AKs')
//NB. includes the null terminator


#endif
