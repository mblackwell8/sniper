#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "userinp.h"

int getNumSimsFromUser() {
	int numSims = 0;
	printf("Enter num simulations: ");
	scanf("%d", &numSims);
	
	return numSims;
}

int getHandsFromUser(char* hands, int numHands[]) {
	char hand[PODDS_MAX_HANDS_PER_PLAYER * PODDS_MAX_CHARS_PER_HAND];
	int playerIx = 0;
	while (playerIx < PODDS_MAX_PLAYERS) {
		printf("Enter Player %d hand (or 'run' to do the sim): ", playerIx + 1);
		scanf("%s", hand);
		if (strcmp(hand, "run") == 0) {
			if (playerIx < 1)
				printf("You have only entered %d players... need at least 2!", playerIx + 1);
			else
				break;
		}
		else {	
			int charCount = getPossibleHands_csvHandCode(hand, hands);
			if (charCount > 0) {
				hands += charCount;
				//this should be a round number...
				numHands[playerIx++] = charCount / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
			}
			else {
				printf("Error code received: %d", charCount);
			}
		}
	}
	
  return playerIx;

}

void toUpper(char* str) {
	for(;*str;str++) 
		*str=toupper(*str);
}

void test() {
	char handsToTest[] = "TT,TT-QQ,AKo,QAs,AJs-AKs,AKs-AJs,QT-QA,JJ+,TA-QA,QQ-QQ,KKo-KKs,AJo-AJs,AJo-AQs,JT+,AQo+,AQs+,JTs+,AdKs,AhJd-AhKd,JdJs+";
	
	outputHands(handsToTest);
}

#ifdef USE_MAIN

int main(int argc, char* argv[]) {
	int i;
	char outbuf[1024*8];
	for (i = 1; i < argc; i++) {
		int charCount = getPossibleHands_csvHandCode(argv[i], outbuf);
    	printHands(outbuf, (charCount / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD)));

	}
	
	return 0;
}

#endif

void outputHands(char *handCodes) {
	
	toUpper(handCodes);
    //char delim[] =  { HAND_SPLITTER };
    char* hands[PODDS_MAX_HANDS_PER_PLAYER];
    int i = 0;
    hands[i] = strtok(handCodes, HAND_SPLITTER);      //get first hand
    while (hands[i] != NULL) {
    	//the last one will be null
    	hands[++i] = strtok(NULL, HAND_SPLITTER);
    }
    
    i = 0;
    char outbuf[1024*20];
    char copybuf[10];
    while (hands[i] != NULL) {
    	strcpy(copybuf, hands[i++]); 
    	
    	printf("'%s' gives: ", copybuf);
    	int charCount = getPossibleHands_possRangeHandCode(copybuf, outbuf);
    	printHands(outbuf, (charCount / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD)));
    }
}



void printHands(const char* hands, int numHands) {
	int i;
	const char* posn = hands;
	for (i = 0; i < numHands; i++) {
		if (hands == NULL) {
			printf(" NULL HAND!");
			return;
		} else {
			printf("%s%s%s", posn, posn + PODDS_CHARS_PER_CARD, ((i + 1) != numHands ? "," : ""));
			posn += PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD;
		}
	}
	printf("\n");
}

const char* getError() {
	switch (errno) {
		case -1:
			return "Multiple ranges specified";
		case -2:
			return "Suited pair doesn't compute";
		case -3:
			return "First code implies pair, but second doesn't";
		case -4:
			return "Cannot handle ranges where suited/offsuit or specific suits specified on only one side";
		case -5:
			return "Cannot understand the hand as typed";
		case -6:
			return "Hand codes must be 2, 3 or 4 letters long, and range lower and upper ends the same length";
		case -7:
			return "Range hands cannot include random cards";
		case -8:
			return "Illegal use of random card";
		default:
			return "Unknown error";
	}
	
	assert_local(0);
	
	return "Unknown error";
}

int validateHand(const char* handCode) {
	
	if (handCode == NULL)
		return 0;
	
	if (!isRegexInited) {
		//assume that this will work
		isRegexInited = 1;
		
		//"^(([2-9TJQKAx][hdscx]){2}([+]|(\-([2-9TJQKA][hdsc]){2}))?)$|^([2-9TJQKAx]{2}[so]?([+]|(\-[2-9TJQKA]{2}[so]?))?)$";
		//note: RegexBuddy says illegal to escape the dash... but must've reinserted it in prior regex (above) for some (good?) reason
		const char* pattern1 = "^(([2-9TJQKAx][hdscx]){2}([+]|(-([2-9TJQKA][hdsc]){2}))?)$|^([2-9TJQKAx]{2}[so]?([+]|(-[2-9TJQKA]{2}[so]?))?)$|^([0-9]|[1-9][0-9]|100)%$";
		
		int chk = regcomp(&restrict1, pattern1, REG_EXTENDED | REG_NOSUB);
		assert_local(chk == 0);
	}
	
	//printf("Validating %s... ", handCode);
	
	int error = 0;

	//BUG: if strtok is called while the load_dag code is running we get an EXC_BAD_ACCESS
	//because that code uses strtok also...
	//HACK: soln is to prevent the use of strtok unless the hand includes a hand splitter, which
	//is unlikely to occur in the first ~10 seconds, which is how long the load_dag code takes on the device
	//ALTERNATIVE?? use strsep??
	
//	if (strchr(handCode, (int)HAND_SPLITTER) != NULL) {
//		printf("using strtok... ");
//		char cpybuf[PODDS_MAX_HANDS_PER_PLAYER * PODDS_MAX_CHARS_PER_HAND];
//		strcpy(cpybuf, handCode);  
//		
//		printf("strtok... ");
//		char* hand = strtok(cpybuf, HAND_SPLITTER);      //get first hand
//		while (hand != NULL) {
//			printf("regex... ");
//			error = regexec(&restrict1, hand, 0, NULL, 0);
//			
//			if (error != 0) 
//				break;
//			
//			//the last one will be null
//			hand = strtok(NULL, HAND_SPLITTER);
//		}
//	}
//	else {
//		printf("w/o strtok... ");
//		error = regexec(&restrict1, handCode, 0, NULL, 0);
//	}
	
	
	//this was PODDS_MAX_HANDS_PER_PLAYER * PODDS_MAX_CHARS_PER_HAND
	//which technically it should be, but started getting EXC_BAD_ACCESS due to excessive memory
	//(only on the iPhone itself)
	//use... almost certainly always just 20 chars long or less
	char buf[256];
	strcpy(buf, handCode);  
	char* cpyHandCode = buf;

	
	//printf("strsep... ");
	char* hand = NULL;
	while ((hand = strsep(&cpyHandCode, HAND_SPLITTER)) != NULL) {
		//printf("regex... ");
		error = regexec(&restrict1, hand, 0, NULL, 0);
		
		if (error != 0) 
			break;
	}	

	//printf("Error = %d\n", error);
	
	return error;
}

int isPercentHand(const char* handCode) {
	if (handCode == NULL)
		return 0;
	
	if (!isPercentHandRegexInited) {
		//assume that this will work
		isPercentHandRegexInited = 1;
		
		const char* pattern = "^([0-9]|[1-9][0-9]|100)%$";
		
		int chk = regcomp(&percentHandRestrict, pattern, REG_EXTENDED | REG_NOSUB);
		assert_local(chk == 0);
	}
	
	int match = regexec(&percentHandRestrict, handCode, 0, NULL, 0);
		
	return (match == 0);
}

/*
int validateHand(char* handCode) {
	if (!isRegexInited) {
		//matches the form RsRs-RsRs
		//const char* pattern1 = "^[+]$";
		const char* pattern1 = "^(([2-9TJQKAx][hdscx]){2}([+]|(\-([2-9TJQKA][hdsc]){2}))?,?)*$|^([2-9TJQKAx]{2}[so]?([+]|(\-[2-9TJQKA]{2}[so]?))?,?)*$";
		
		//const char* pattern1 = "^(([2-9TJQKA][hdsc]){2}(\-([2-9TJQKA][hdsc]){2})?,?)*$|^([2-9TJQKA]{2}[so]?(\-[2-9TJQKA]{2}[so]?)?,?)*$";
		
		int chk = 0;
		chk = regcomp(&restrict1, pattern1, REG_EXTENDED | REG_NOSUB);
		assert_local(chk == 0);
		
		//chk = regcomp(&restrict2, pattern2, REG_EXTENDED | REG_NOSUB);
		//assert_local(chk == 0);
		
		isRegexInited = 1;
	}
	
	return (regexec(&restrict1, handCode, 0, NULL, 0));
	
}
*/

int getPossibleHands_csvHandCode(const char* handCode, char* output) {
	return getPossibleHands_csvHandCode_inclSubCounts(handCode, output, NULL);
}

//first stage deals with full, CSV string eg. AJs-AKs, JJ+, 
//handcode is complete from user... first step is to tokenise
//the CSVs, then hand each individual handcode to internalGetPossibleHands
//returns the number of hands
int getPossibleHands_csvHandCode_inclSubCounts(const char* handCode, char* output, int hcCounts[]) {	
    errno = 0;
	
	//Make writable copy
    char cp[PODDS_MAX_HANDCODES_PER_PLAYER * PODDS_MAX_CHARS_PER_HAND];
    strcpy(cp, handCode);  
	
	//char cp[] = "TT,TT-QQ,AKo";
    //toUpper(cp);
	
    //char delim[] =  { HAND_SPLITTER };
    char* hands[PODDS_MAX_HANDCODES_PER_PLAYER];
    int i = 0;
    hands[i] = strtok(cp, HAND_SPLITTER);      //get first hand
    while (hands[i] != NULL) {
    	//the last one will be null
    	hands[++i] = strtok(NULL, HAND_SPLITTER);
    }
    
    i = 0;
	
    //must be at least 1326 hands * 2 cards * 3 chars per card
	//which is 7956, so use 2^13 = 8192
	char copybuf[8192];
    int charCount = 0;
    while (hands[i] != NULL) {
    	
		int retVal = expandRandoms(hands[i], copybuf); 
		if (retVal < 0) {
			errno = retVal;
			return retVal;
		}
		
		toUpper(copybuf);
		
		//for non random hands this will just be a single hand
		char* expandedHands[MAX_RANDOM_HAND_COMBOS];
		int cc = 0, j = 0;
		expandedHands[j] = strtok(copybuf, HAND_SPLITTER);
		while (expandedHands[j] != NULL) {
			expandedHands[++j] = strtok(NULL, HAND_SPLITTER);
		}
		
		j = 0;
		char expandedHand[PODDS_MAX_CHARS_PER_HAND];
		while (expandedHands[j] != NULL) {
			strcpy(expandedHand, expandedHands[j]);
			int result = getPossibleHands_possRangeHandCode(expandedHand, output);
			if (result < 0) {
				errno = result;
				return result;
			}
			
			cc += result;
			output += result;
			j++;
		}
		
		//no bounds checking here...
		if (hcCounts != NULL)
			hcCounts[i] = cc;
		
    	charCount += cc;
		i++;
    }
	
//	printf("handcode = %s\n", handCode);
//	printf("combo count = %d\n", charCount / (PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS));
//	printHands(output - charCount, (int)(charCount / (PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS)));
    
	return charCount;
}

int expandRandoms(const char* hc, char* expandBuf) {
	//function should never return any more than MAX_RANDOM_HAND_COMBOS combos
	
	if (!strchr(hc, (int)RANDOM_SYMBOL_LOWER)) {
		strcpy(expandBuf, hc);
		return strlen(hc);
	}
		
	if (strchr(hc, (int)RANGE_NO_UPPER_LIMIT) || strchr(hc, (int)RANGE_SPLITTER_CHAR))
		return -7; //"Range hands cannot include random cards"
	
	//xxxx expands to 22-AA,23-2A,34-3A,45-4A,56-5A,67-6A,78-7A,89-8A,9T-9A,TJ-TA,JQ-JA,QK-QA,KA (i think!)
	if (strcmp(hc, "xxxx") == 0) {
		strcpy(expandBuf, "22-AA,23-2A,34-3A,45-4A,56-5A,67-6A,78-7A,89-8A,9T-9A,TJ-TA,JQ-JA,QK-QA,KA");
	}	
	//xxs expands to 23s-2As,34s-3As,45s-4As,56s-5As,67s-6As,78s-7As,89s-8As,9Ts-9As,TJs-TAs,JQs-JAs,QKs-QAs,KAs
	else if (strcmp(hc, "xxs") == 0) {
		strcpy(expandBuf,"23s-2As,34s-3As,45s-4As,56s-5As,67s-6As,78s-7As,89s-8As,9Ts-9As,TJs-TAs,JQs-JAs,QKs-QAs,KAs");
	}
	//xxo expands to
	else if (strcmp(hc, "xxo") == 0) {
		strcpy(expandBuf,"22-AA,23o-2Ao,34o-3Ao,45o-4Ao,56o-5Ao,67o-6Ao,78o-7Ao,89o-8Ao,9To-9Ao,TJo-TAo,JQo-JAo,QKo-QAo,KAo");
	}
	//Rx and xR expands to R2-RA
	//Rxs and xRs expand to R2s-RAs
	//Rxo and xRo expand to R2o-RAo
	else if (strlen(hc) == 2 || strlen(hc) == 3) {
		char constCard = (hc[0] == RANDOM_SYMBOL_LOWER ? hc[1] : hc[0]);

		expandBuf[0] = constCard;
		expandBuf[1] = '2';
		expandBuf[2] = RANGE_SPLITTER_CHAR;
		expandBuf[3] = constCard;
		expandBuf[4] = 'A';
		
		if (strlen(hc) == 3) {
			expandBuf[5] = hc[2]; //s or o
			expandBuf[6] = '\0';
		}
		else {
			expandBuf [5] = '\0';
		}

	}
	else if (strlen(hc) == 4) {
		//Rxxx and xxRx expands to R2-RA (ie. same as Rx)
		if ((hc[1] == RANDOM_SYMBOL_LOWER && hc[2] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER) ||
			(hc[0] == RANDOM_SYMBOL_LOWER && hc[1] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER)) {
			char constCard = (hc[0] == RANDOM_SYMBOL_LOWER ? hc[2] : hc[0]);
			
			expandBuf[0] = constCard;
			expandBuf[1] = '2';
			expandBuf[2] = RANGE_SPLITTER_CHAR;
			expandBuf[3] = constCard;
			expandBuf[4] = 'A';
			expandBuf[5] = '\0';
		}
		//xsxx and xxxs expand to xsxh,xsxc,xsxs,xsxd (call recursively)
		else if ((hc[0] == RANDOM_SYMBOL_LOWER && hc[2] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER) ||
				 (hc[0] == RANDOM_SYMBOL_LOWER && hc[1] == RANDOM_SYMBOL_LOWER && hc[2] == RANDOM_SYMBOL_LOWER)) {
			int varSuitPosn = (hc[1] == RANDOM_SYMBOL_LOWER ? 1 : 3);
			int i, posn = 0;
			char hcBuf[4];
			for (i = 0; i < PODDS_NUM_SUITS; i++) {
				strcpy(hcBuf, hc);
				hcBuf[varSuitPosn] = COLOR[i];
				
				//recursive call
				posn = expandRandoms(hcBuf, expandBuf);
				expandBuf[posn] =  (i != (PODDS_NUM_SUITS - 1) ? HAND_SPLITTER_CHAR : '\0');
				expandBuf += (posn + 1);
			}
			
		}
		//Rsxx and xxRs expand to Rs2h-RsAh,Rs2c-RsAc,Rs2d-RsAd,Rs2s-RsAs
		else if ((hc[2] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER) ||
				 (hc[0] == RANDOM_SYMBOL_LOWER && hc[1] == RANDOM_SYMBOL_LOWER)) {
			int varSuitPosn = (hc[1] == RANDOM_SYMBOL_LOWER ? 1 : 3);

			int i, posn = 0;
			char hcBuf[4];
			for (i = 0; i < PODDS_NUM_SUITS; i++) {
				strcpy(hcBuf, hc);
				hcBuf[varSuitPosn] = COLOR[i];
				
				//recursive call
				posn = expandRandoms(hcBuf, expandBuf);
				expandBuf[posn] =  (i != (PODDS_NUM_SUITS - 1) ? HAND_SPLITTER_CHAR : '\0');
				expandBuf += (posn + 1);
			}
		}
		//RxRx expands to RR
		else if (hc[1] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER) {
			expandBuf[0] = hc[0];
			expandBuf[1] = hc[2];
			expandBuf[2] = '\0';
		}
		//xsxs expands to 2s2s-AsAs,2s3s-2sAs,3s4s-3sAs,4s5s-4sAs,5s6s-5sAs,6s7s-6sAs,7s8s-7sAs,8s9s-8sAs,9sTs-9sAs,TsJs-TsAs,JsQs-JsAs,QsKs-QsAs,KsAs
		else if (hc[0] == RANDOM_SYMBOL_LOWER && hc[2] == RANDOM_SYMBOL_LOWER) {			
			if (hc[1] != hc[3]) {
				strcpy(expandBuf, "2s2z-AsAz,\
								   2s3z-2sAz,3s4z-3sAz,4s5z-4sAz,5s6z-5sAz,6s7z-6sAz,7s8z-7sAz,8s9z-8sAz,9sTz-9sAz,TsJz-TsAz,JsQz-JsAz,QsKz-QsAz,KsAz,\
								   3s2z,4s2z-4s3z,5s2z-5s4z,6s2z-6s5z,7s2z-7s6z,8s2z-8s7z,9s2z-9s8z,Ts2z-Ts9z,Js2z-JsTz,Qs2z-QsJz,Ks2z-KsQz,As2z-AsKz");
			}
			else {
				strcpy(expandBuf, "2s3s-2sAs,3s4s-3sAs,4s5s-4sAs,5s6s-5sAs,6s7s-6sAs,7s8s-7sAs,8s9s-8sAs,9sTs-9sAs,TsJs-TsAs,JsQs-JsAs,QsKs-QsAs,KsAs");
			}
			
			
			char *p;
			for (p = expandBuf; *p; p++) {
				if (*p == 's') *p = hc[1];
				else if (*p == 'z') *p = hc[3];
			}
		}
		//xsRx and Rxxs expand to Rh2s-RhAs,Rd2s-RdAs,Rs2suit-RsAsuit,Rc2s-RcAs
		else if ((hc[1] == RANDOM_SYMBOL_LOWER && hc[2] == RANDOM_SYMBOL_LOWER) ||
				 (hc[0] == RANDOM_SYMBOL_LOWER && hc[3] == RANDOM_SYMBOL_LOWER)) {
			int varSuitPosn = (hc[1] == RANDOM_SYMBOL_LOWER ? 1 : 3);
			
			int i, posn = 0;
			char hcBuf[4];
			for (i = 0; i < PODDS_NUM_SUITS; i++) {
				strcpy(hcBuf, hc);
				hcBuf[varSuitPosn] = COLOR[i];
				
				//recursive call
				posn = expandRandoms(hcBuf, expandBuf);
				expandBuf[posn] =  (i != (PODDS_NUM_SUITS - 1) ? HAND_SPLITTER_CHAR : '\0');
				expandBuf += (posn + 1);
			}
		}
		//RsRx and RxRs expand to RsRh,RsRc,RsRd,RsRs
		else if (hc[3] == RANDOM_SYMBOL_LOWER || hc[1] == RANDOM_SYMBOL_LOWER) {
			char constSuit = (hc[3] == RANDOM_SYMBOL_LOWER ? hc[1] : hc[3]);
			int i, posn = 0;
			for (i = 0; i < PODDS_NUM_SUITS; i++) {
				if (hc[0] != hc[2] || COLOR[i] != constSuit) {
					expandBuf[posn + 0] = hc[0];
					expandBuf[posn + 1] = constSuit;
					expandBuf[posn + 2] = hc[2];
					expandBuf[posn + 3] = COLOR[i];
					expandBuf[posn + 4] = (i != (PODDS_NUM_SUITS - 1) ? HAND_SPLITTER_CHAR : '\0');
					
					posn += 5;
				}
			}
			
		}
		//Rsxs and xsRs expand to Rs2s-RsAs
		else if (hc[0] == RANDOM_SYMBOL_LOWER || hc[2] == RANDOM_SYMBOL_LOWER) {
			char constRank = (hc[0] == RANDOM_SYMBOL_LOWER ? hc[2] : hc[0]);
			char constSuit1 = (hc[0] == RANDOM_SYMBOL_LOWER ? hc[3] : hc[1]);
			char constSuit2 = (hc[0] == RANDOM_SYMBOL_LOWER ? hc[1] : hc[3]);
			
			expandBuf[0] = constRank;
			expandBuf[1] = constSuit1;
			expandBuf[2] = '2';
			expandBuf[3] = constSuit2;
			expandBuf[4] = RANGE_SPLITTER_CHAR;
			expandBuf[5] = constRank;
			expandBuf[6] = constSuit1;
			expandBuf[7] = 'A';
			expandBuf[8] = constSuit2;
			expandBuf[9] = '\0';
		}
		else {
			return -8;
		}
	}
	//anything else is an error
	else {
		return -8;
	}
	
	return strlen(expandBuf);

}

//second stage deals with individual hand sets eg. AJs-AKs or JJ+
int getPossibleHands_possRangeHandCode(const char* handCode, char* output) {
	// first split any ranges...	
	errno = 0;
	
	char copyHandCode[PODDS_MAX_CHARS_PER_HAND];
	strcpy(copyHandCode, handCode);
	char* lowerBound = strtok(copyHandCode, RANGE_SPLITTER);
	char* upperBound = strtok(NULL, RANGE_SPLITTER);
	
	char* remainder = strtok(NULL, RANGE_SPLITTER);
	if (remainder != NULL) {
		//"Multiple ranges specified"
		return -1;
	}
	
	int lbLen = strlen(lowerBound);
	int ubLen = (upperBound == NULL ? 0 : strlen(upperBound));

	char buf[5];
	//do we have an upper bound?
	if (upperBound == NULL) {
		if (lowerBound[lbLen - 1] == RANGE_NO_UPPER_LIMIT) {
			//check for randoms first, because getNoUpperLimitBound may crash on a random card
			if (strchr(lowerBound, (int)RANDOM_SYMBOL_UPPER))
				return -7;
			
			// now we need to deal with no upper limit ranges...
			getNoLimitUpperBound(copyHandCode, buf);
			upperBound = buf;
			ubLen = strlen(upperBound);
			
			//remove the ending plus
			lowerBound[lbLen - 1] = '\0';
			lbLen--;
		} else {
			// this is the simple case of two cards, potentially with
			// offsuit/suited
			return getPossibleHands_looseSingleHandCode(copyHandCode, output);
		}
	}
	
	//ranges can't include randoms...
	if (strchr(lowerBound, (int)RANDOM_SYMBOL_UPPER) || 
		strchr(upperBound, (int)RANDOM_SYMBOL_UPPER))
		return -7;

	// so now we have a range to deal with
	int charShiftToSecondRank = 0;
	if ((lbLen == 2 && ubLen == 2) || (lbLen == 3 && ubLen == 3)) {
		charShiftToSecondRank = 0;
	}
	//if the length is 4 then the suits must match (ie. can't do suit ranges)
	else if (lbLen == 4 && ubLen == 4 && lowerBound[1] == upperBound[1] && lowerBound[3] == upperBound[3]) {
		charShiftToSecondRank = 1;
	}
	else {
		//printf("Hand codes must be 2, 3 or 4 chars long, and range lower and upper ends the same length");
		return -6;
	}
	
	assert_local(lbLen == ubLen);
	
	int charCount = 0;

	//is it a pair range? like 22-66 or 2d2c-6d6c
	if (lowerBound[0] == lowerBound[1 + charShiftToSecondRank]) {
		if (upperBound[0] != upperBound[1 + charShiftToSecondRank]) {
			//printf("First code implies pair, but second doesn't: %s", copyHandCode);
			return -3;
		}
		
		//invalid to have suited pair
		if ((lbLen == 4 && lowerBound[1] == lowerBound[3]) ||
			(ubLen == 4 && upperBound[1] == upperBound[3])) {
			return -2;
		}

		//this will go out of scope so can't use elsewhere
		//must include space for the null terminator
		char rangebuf[PODDS_NUM_CARDS_PER_SUIT + 1];
		
		//it's a pair range, so no need to skip any cards in the range
		char skipCard = '\0';
		getRankRange(lowerBound[0],	upperBound[0], skipCard, rangebuf);
		char handbuf[5];
		int i, rangeLen = strlen(rangebuf);
		for (i = 0; i < rangeLen; i++) {
			handbuf[0] = rangebuf[i];
			handbuf[1] = lowerBound[1];
			handbuf[1 + charShiftToSecondRank] = rangebuf[i]; 	//posn 1 or 2, so will replace lowerBound[1] in two letter case
			handbuf[2 + charShiftToSecondRank] = (lbLen > 2 ? lowerBound[3] : '\0'); 	//posn 2 or 3...
			handbuf[3 + charShiftToSecondRank] = '\0';  			//posn 3 or 4

			charCount += getPossibleHands_looseSingleHandCode(handbuf, output + charCount);
		}
	} 
	//or is it a range like A2-A5 or A2s-A5s or Ah2h-Ah5h (or the other way around, like 2A-5A)
	else if (lowerBound[0] == upperBound[0] || lowerBound[1 + charShiftToSecondRank] == upperBound[1 + charShiftToSecondRank]) {
		if (strlen(lowerBound) != strlen(upperBound)) {
			//printf("Cannot handle ranges where suited/offsuit or specific suits specified on only one side: %s", copyHandCode);
			return -4;
		}

		// non pair range, on either the first or second letter
		char lowerCard, upperCard, constCard;
		if (lowerBound[0] == upperBound[0]) {
			// this is like AJ-AK
			// TODO: does it matter if second letters are same too?
			lowerCard = lowerBound[1 + charShiftToSecondRank];
			upperCard = upperBound[1 + charShiftToSecondRank];
			constCard = lowerBound[0];
		} else {
			assert_local (lowerBound[1 + charShiftToSecondRank] == upperBound[1 + charShiftToSecondRank]);
			// this is like JA-KA (ie. a bit wierd)
			lowerCard = lowerBound[0];
			upperCard = upperBound[0];
			constCard = lowerBound[1 + charShiftToSecondRank];
		}
			
		//lbLen == ubLen... 
		//if we have a 3 letter handcode then it must be suited or offsuit
		//if it is suited, then we want to skip the const card, so that
		//K9s+ (by now, translated to K9s-KAs) doesn't include a KKs in the range
		char skipCard = '\0';
		if (lbLen == 3 && lowerBound[2] == SUITED) {
			assert_local(ubLen == 3 && upperBound[2] == SUITED);
			skipCard = constCard;
		}
		//if we have a 4 letter handcode and the suits are the same
		//then we want to skip over the const card so that
		//Kh9h+ (by now, translated to Kh9h-KhAh) doesn't include KhKh
		if (lbLen == 4 && lowerBound[1] == lowerBound[3]) {
			//both of these assertions are error checked above
			assert_local(ubLen == 4 && upperBound[1] == upperBound[3]);
			assert_local(lowerBound[1] == upperBound[1] && lowerBound[3] == upperBound[3]);
			
			skipCard = constCard;
		}
		
		char rangebuf[PODDS_NUM_CARDS_PER_SUIT];
		getRankRange(lowerCard,	upperCard, skipCard, rangebuf);
		char handbuf[5];
		int i, rangeLen = strlen(rangebuf);
		for (i = 0; i < rangeLen; i++) {
			handbuf[0] = constCard;
			
			//this will be replaced in cases where charShift = 0
			handbuf[1] = lowerBound[1];
			handbuf[1 + charShiftToSecondRank] = rangebuf[i];
			
			// add offsuit etc if required, looking only at the first code initially
			
			//change in v3: was
			//if (lbLen > 2 && ubLen > 2 && (lowerBound[lbLen - 1] == SUITED || lowerBound[lbLen - 1] == OFFSUIT)) { 
			//but this falls foul of 4 letter codes with spades (which are S == SUITED also)
			//eg AH2S
			if (lbLen == 3 && ubLen == 3 && (lowerBound[lbLen - 1] == SUITED || lowerBound[lbLen - 1] == OFFSUIT)) { 
				assert_local (charShiftToSecondRank == 0);
				if (lowerBound[lbLen - 1] == upperBound[ubLen - 1]) {
					//if the offsuit/suit specifier is the same, then include it
					handbuf[2] = lowerBound[2];
					handbuf[3] = '\0';
				}
				else {
					//if its not the same, then we want all combinations, so just
					//leave it off and we will get it due to the default behaviour
					//of getPossibleHands_looseSingleHandCode
					handbuf[2] = '\0';
				}
			}
			else {
				//we're either dealing with a simple two letters or four letters
				handbuf[2 + charShiftToSecondRank] = (lbLen > 2 ? lowerBound[3] : '\0'); 	//posn 2 or 3
				handbuf[3 + charShiftToSecondRank] = '\0';  			//posn 3 or 4
			}
			
			charCount += getPossibleHands_looseSingleHandCode(handbuf, output + charCount);
		}
	} 
	else {
		//printf("Cannot parse: %s", copyHandCode);
		return -5;
	}
	
	//printHands(output, (int)(charCount / (PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS)));


	return charCount;

}

void getNoLimitUpperBound(const char* existingHandCode, char* upperBound) {
	//first check for JJ+ style (which can't be suited, but may be offsuit if the user is being really silly)
	int len = strlen(existingHandCode);
	assert_local(len <= 5); //max is RsRs+
	if ((len == 3 && existingHandCode[0] == existingHandCode[1]) ||
		(len == 4 && existingHandCode[2] == OFFSUIT && existingHandCode[0] == existingHandCode[1])) {
		// it's a pair, so tops out at AA
		upperBound[0] = 'A';
		upperBound[1] = 'A';
		upperBound[2] = '\0';
		// this ignores any offsuit or suited spec... no probs because is pair
	}
	//then look for something like 2c2h+
	else if (len == 5 && existingHandCode[0] == existingHandCode[3]) {
		upperBound[0] = 'A';
		upperBound[1] = existingHandCode[1];
		upperBound[2] = 'A';
		upperBound[3] = existingHandCode[3];
		upperBound[4] = '\0';
	}
	else {
		//could be something simple like J2+, or J2s+ or something more complex like Jh2h+
		
		char firstRank = existingHandCode[0];
		char secondRank = (len == 3 || len == 4 ? existingHandCode[1] : existingHandCode[2]);
		
		//PokerStove behaviour is that KJ+ = KJ-KQ, and that 2J+ = J2+ = J2-JT (ie. if the second card is higher than the first, 
		//then swap them over and increment the lower card)
		//the ranging code handles 2J-TJ, which looks wierd but still works
		//NB "reversed" means J2, ie second card less than first 
		int reversed = isReverseOrder(firstRank, secondRank);
		char constRank = (reversed ? firstRank : secondRank);
		int cardIx = 0;
		while (cardIx < PODDS_NUM_CARDS_PER_SUIT && CARD[cardIx] != constRank) cardIx++;
		assert_local (cardIx != PODDS_NUM_CARDS_PER_SUIT);
		char rangeHighRank = CARD[(cardIx - 1)];
		
		
		upperBound[0] = (reversed ? constRank : rangeHighRank);
		if (len == 3) {
			upperBound[1] = (reversed ? rangeHighRank : constRank);
			upperBound[2] = '\0';
		}
		else if (len == 4) {
			assert_local (existingHandCode[3] == RANGE_NO_UPPER_LIMIT)
			upperBound[1] = (reversed ? rangeHighRank : constRank);
			// add the offsuit/suited specifier to hands like AQo+
			upperBound[2] = existingHandCode[2];
			upperBound[3] = '\0';
			//NB. special case of say AJs+ will return AJs-AAs, where the upper end of the range is
			//nonsense, or similarly JTs+ will return JTs-JAs and the interim JJs is nonsense
			//rather than try to deal with it here (and the second problem really can't be), just let
			//the final hand construction algs do it for me, but not adding the hand 
		}
		else {
			assert_local(len == 5);
			upperBound[1] = existingHandCode[1];
			upperBound[2] = (reversed ? rangeHighRank : constRank);
			upperBound[3] = existingHandCode[3];
			upperBound[4] = '\0';
		}
	}
}

void getRankRange(char firstLetter,	char secondLetter, char skipCard, char* rankRange) {

	// iterate over the key set of LETTER_CODES
	// but first deal with reverse order ranges
	char temp;
	if (isReverseOrder(firstLetter, secondLetter)) {
		temp = firstLetter;
		firstLetter = secondLetter;
		secondLetter = temp;
	}
	
	int i = 0, j = 0;
	//walk the card array until we reach the first letter...
	while (i < PODDS_NUM_CARDS_PER_SUIT && CARD[i] != firstLetter) i++;
	
	while (i < PODDS_NUM_CARDS_PER_SUIT && CARD[i-1] != secondLetter) {
		if (skipCard != CARD[i])
			rankRange[j++] = CARD[i];
		i++;
	}
	//rankRange[j] = (skipCard != secondLetter ? secondLetter : '\0');
	rankRange[j] = '\0';
}

int isReverseOrder(char firstLetter, char secondLetter) {
	int i = 0;
	int isReverseOrder = 0;
	while (i < PODDS_NUM_CARDS_PER_SUIT && CARD[i] != firstLetter) {
		if (CARD[i] == secondLetter) isReverseOrder = 1;
		i++;
	}
	
	assert_local (i != PODDS_NUM_CARDS_PER_SUIT);
	
	return isReverseOrder;
}

/* 
 
int hasRandomRank(char* handCode) {
	//works for a 2 or 4 char handCode
	int len = 0;
	assert_local((len = strlen(handCode)) == 2 || len == 4);
	char *posn = strchr(handCode, (int)RANDOM_RANK);
	
	//random rank will either be the first char or the third
	if (posn != NULL && posn - handCode == 0 || posn - handCode == 2)
		return 1;
	
	return 0;
}

int hasRandomSuit(char* handCode) {
	//works for a 2 or 4 char handCode
	int len = 0;
	assert_local((len = strlen(handCode)) == 2 || len == 4);
	char *posn = strchr(handCode, (int)RANDOM_SUIT);
	
	//random rank will either be the second char or the fourth
	if (posn != NULL && posn - handCode == 1 || posn - handCode == 3)
		return 1;
	
	return 0;
} 
 
*/

//third stage deals with all individual hands, whether suited or unsuited is specified
//eg. AJs,AQs,AKs,JJ,QQ,KK,AA,KsJd,AsAd
int getPossibleHands_looseSingleHandCode(const char* handCode, char* output) {

	int charCount = 0;

	if (strlen(handCode) == 2) {
		if (handCode[0] == handCode[1] && handCode[0] != RANDOM_RANK) {
			// pair, so always offsuit
			char handbuf[4] = { handCode[0], handCode[1], OFFSUIT, '\0' };
			
			charCount = getPossibleHands_preciseSingleHandCode(handbuf, output);
		} else {
			//get offsuit and suited options
			char handbuf[4] = { handCode[0], handCode[1], OFFSUIT, '\0' };
			charCount = getPossibleHands_preciseSingleHandCode(handbuf, output);

			if (charCount > 0) {
				handbuf[2] = SUITED;
				charCount += getPossibleHands_preciseSingleHandCode(handbuf, output + charCount);
			}
		}
	} 
	else if ((strlen(handCode) == 3 || strlen(handCode) == 4)){
		//just evaluate it the way it is...
		charCount = getPossibleHands_preciseSingleHandCode(handCode, output);
	}
	
	//else fail silently... we do not want to return 
	//an error code from here because this method quite often runs in 
	//a loop (for ranges etc), and the error message for one part of a
	//range makes little sense to the user anyway, so we'll just
	//ignore this hand

	return charCount;
}

//final stage figures the precise hands AJs,AQs,AKs,JJo,QQo,KKo,AAo
int getPossibleHands_preciseSingleHandCode(const char* handCode, char* output) {
	
	//this method must received either RsRs or RRo or RRs. if the first, accepts x as a random suit
	int handCodeLen = strlen(handCode);
	assert_local (handCodeLen == 3 || handCodeLen == 4);
	int charCount = 0;
	
	//eg. KQs
	if (handCodeLen == 3 && handCode[2] == SUITED) {
		//add all of the (4) suited two card combos
	
		// invalid to have the same card, suited
		if (handCode[0] == handCode[1] && handCode[0] != RANDOM_RANK)
			return -2;
		
		//note that the above will cause problems when called from a range loop
		//so need to make sure that they NEVER call an invalid hand like AhAh or AAs
	
		int i, j;
		for (i = 0; i < PODDS_NUM_SUITS; i++) {
			for (j = 0; j < PODDS_NUM_DEALT_CARDS; j++) {
				output[charCount]  = handCode[j];
				output[charCount + 1]  = COLOR[i];
				output[charCount + 2]  = '\0';
				charCount += PODDS_CHARS_PER_CARD;
			}
		}
	} 
	//eg KQo
	else if (handCodeLen == 3 && handCode[2] == OFFSUIT) {
		//add all of the (12) unsuited two card combos
		// will generate random next suit
		int i, i2, j;
		for (i = 0; i < PODDS_NUM_SUITS; i++) {
			// if it's a pair we need to skip the repeated combos (eg 2k2s == 2s2k)
			i2 = (handCode[0] == handCode[1] && handCode[0] != RANDOM_RANK) ? i : 0;
			for (; i2 < PODDS_NUM_SUITS; i2++) {
				if (i != i2) {
					for (j = 0; j < PODDS_NUM_DEALT_CARDS; j++) {
						output[charCount]  = handCode[j];
						
						//HACK: will not work if PODDS_NUM_DEALT_CARDS != 2
						output[charCount + 1]  = COLOR[(j == 0 ? i : i2)];
						output[charCount + 2]  = '\0';
						charCount += PODDS_CHARS_PER_CARD;
					}
				}
			}
		}
	}
	else if (handCodeLen == 4) {
		//this is a precise hand already...
		output[charCount] = handCode[0];
		output[charCount + 1] = tolower(handCode[1]);
		output[charCount + 2]  = '\0';
		output[charCount + 3] = handCode[2];
		output[charCount + 4] = tolower(handCode[3]);
		output[charCount + 5]  = '\0';
		
		charCount += 6;
	}
	else {
		return -6;
	}

	return charCount;
}

/*

int getPossibleHands_randomHandCode(char* handcode, char* output) {
	//handcode must be "Rs\0Rs\0" with a random somewhere
	char* start = output;
	int charCount = 0;
	
	for (c = 0; c < PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD; c++) {
		
		//by now the capital R, lower case s has been restored to the hand
		if (output[c] == RANDOM_SUIT) {
			<#statements#>
		}
		else if (output[c] == RANDOM_RANK) {
		}
	}
	
}
*/


