#ifndef USERINP_H_
#define USERINP_H_

#include <regex.h>

#include "common.h"

#define HAND_SPLITTER 			","
#define RANGE_SPLITTER 			"-"
#define HAND_SPLITTER_CHAR 		','
#define RANGE_SPLITTER_CHAR		'-'

#define RANGE_NO_UPPER_LIMIT 	'+'
#define SUITED 					'S'
#define OFFSUIT 				'O'
#define RANDOM_SUIT 			'x'
#define RANDOM_RANK 			'X'

//used intra handcode translation, while handcode is upper case
//suspect that the conversion to upper case is to allow for broader
//keyboards than PS currently has, but not sure
#define RANDOM_SYMBOL_UPPER 	'X'

//current keyboard has only lower case x
#define RANDOM_SYMBOL_LOWER 	'x'

#define MAX_RANDOM_HAND_COMBOS	256

//#define USE_MAIN

int errno;
int isRegexInited;
regex_t restrict1, restrict2;

int isPercentHandRegexInited;
regex_t percentHandRestrict;

const char* getError();

int getNumSimsFromUser(); 

//cards are numbered 0-51, with lowest two bits the suit, and next 6 bits the rank
//the carddeck parse method implements this pattern


//returns the number of players (including me)
int getHandsFromUser(char* hands, int numHands[]);

void outputHands(char *handCodes);

int validateHand(const char* handCode);
int isPercentHand(const char* handCode);

int getComboCount(const char* handCode);

//internal methods, each of which will return the number of chars put on output array
//first stage deals with full, CSV string eg. AJs-AKs, JJ+, 
int getPossibleHands_csvHandCode(const char* handCode, char* output);
int getPossibleHands_csvHandCode_inclSubCounts(const char* handCode, char* output, int hcCounts[]);

//second stage deals with individual hand sets eg. AJs-AKs or JJ+
int getPossibleHands_possRangeHandCode(const char* handCode, char* output);

//third stage deals with all individual hands, whether suited or unsuited is specified
//eg. AJs,AQs,AKs,JJ,QQ,KK.AA
int getPossibleHands_looseSingleHandCode(const char* handCode, char* output);

//final stage figures the precise hands AJs,AQs,AKs,JJo,QQo,KKo,AAo
int getPossibleHands_preciseSingleHandCode(const char* handCode, char* output);

//int getPossibleHands_randomHandCode(char* handCode, char* output);

int expandRandoms(const char* existingHandCode, char* expandBuf);
void getRankRange(char firstLetter, char secondLetter, char skipCard, char* rankRange);
int isReverseOrder(char firstLetter, char secondLetter);
void getNoLimitUpperBound(const char* existingHandCode, char* upperBound);
void toUpper(char* str);
//int hasRandomRank(char* handCode);
//int hasRandomSuit(char* handCode);

void test();
void printHands(const char* hands, int numHands);

#endif /*USERINP_H_*/
