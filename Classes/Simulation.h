//
//  Simulation.h
//  sniper
//
//  Created by Mark Blackwell on 16/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "podds.h"

//must be at least (PODDS_MAX_PLAYERS + 1) * PODDS_MAX_HANDS_PER_PLAYER * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD
//this was 1024*8 but that is much larger than required and causes the initFromCoder code to run VERY slowly
//(crashing the device)
#define SIMULATION_BUFSIZE				32768 //must be enough for four players to have xxxx, plus a bit more for for dead/board cards
													  //which is 1326 hands x 2 cards x 3 chars x 4 players = 31824 or round up to 2^15 = 32768
#define kNumPlayersKey					@"numPlayers"
#define kNumFloppedKey					@"numFlopped"
#define kNumDeadKey						@"numDead"
#define kNumCardsKey					@"numCards"
#define kResultForPlayerPrefix			@"resultForPlayer%d"
#define kBufMarkerOffset				@"bufMarkerOffset"
#define kFlopOffset						@"flopOffset"
#define kDeadOffset						@"deadOffset"
#define kHandCodeKey					@"handCodes"
#define kWereResultsEncoded				@"WereResultsEncoded"
#define kLiteTrialCount					@"liteTrialCount"
#define kIsFirstFlopRiverTurnCycle		@"isFirstFlopRiverTurnCycle"
#define kUseCount                       @"useCount"
#define kIsFirstUseOfHandWeighting		@"isFirstUseOfHandWeighting"
#define kVersion						@"version"
#define UNKNOWN_VERSION					@"unknown_version"
//#define kBufKey @"buffer"
#define NUM_SIMS						1000000
#define FLOP_STRING_ARCHIVE_KEY			@"flopStr"
#define DEAD_STRING_ARCHIVE_KEY			@"deadStr"
#define PERCENTAGEHANDS_FILENAME		@"percentHandData.csv"

#define MAX_ENCODEABLE_BUFSIZE			5000  //if we encode/decode too much it takes too long

//typedef struct simulation_result {
//	unsigned int rounds;
//	unsigned int player; /* Number of opponents (1 for heads-up) */
//	unsigned int won;
//	unsigned int split;
//	char card1, card2;
//	unsigned int winclasses[10], looseclasses[10];
//} simulation_result; 

@interface Simulation : NSObject <NSCoding, NSCopying> {
	char buf[SIMULATION_BUFSIZE];
	char *bufMarker;
		

@public
	int numPlayers;
	int numHands[PODDS_MAX_PLAYERS + 1];
	int numFlopped;
	int numDead;
	int numSims;
	NSMutableDictionary *handCodes;

	
	char *hands;
	char *flop;
	char *dead;
	
	int numCards;
	
	//int percentDone;
	
	simulation_result results[PODDS_MAX_PLAYERS + 1];
	
	//HACK: really should be a property of the SniperViewController, but that would
	//require more fiddling with archivers... so put here
	int liteTrialCount;
	BOOL isFirstFlopRiverTurnCycle;
    int useCount;
    BOOL isFirstUseOfHandWeighting;
	
	int numHands_weighted[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER];
	int numHandCodes[PODDS_MAX_PLAYERS + 1];
	int handCodesWeights[PODDS_MAX_PLAYERS + 1][PODDS_MAX_HANDS_PER_PLAYER];
	
	NSString *version;

}

//can't figure out how to use properties on C arrays
//one approach (http://stackoverflow.com/questions/476843/create-an-array-of-integers-property-in-objective-c)
//worked until tried to build release candidate for OS 3.0, when it broke, so have retreated to 
//using manual getters

@property (nonatomic) int numPlayers;
//@property (nonatomic) int *numHands;
@property (nonatomic) int numFlopped;
@property (nonatomic) int numDead;
@property (nonatomic) int numSims;
@property (nonatomic) char *hands;
@property (nonatomic) char *flop;
@property (nonatomic) char *dead;
@property (nonatomic) int numCards;
@property (nonatomic) int liteTrialCount;
@property (nonatomic) BOOL isFirstFlopRiverTurnCycle;
@property (nonatomic) int useCount;
@property (nonatomic) BOOL isFirstUseOfHandWeighting;
@property (nonatomic, retain) NSMutableDictionary *handCodes;
@property (nonatomic, retain) NSString *version;
//- (void)setFlop:(char *) newflop;
- (void)setFlopString:(NSString *) flopStr;
- (void)setDeadString:(NSString *) deadStr;

//@property (nonatomic) simulation_result *results;

//these two are really private to the class, but can't seem to have that
//and still access them in the copyWithZone implementation

//@property (nonatomic) char *buf;
@property (nonatomic) char *bufMarker;

- (char *)buf;
- (simulation_result *)results;
- (int *)numHands;
- (int *)numHands_weighted;
- (int *)numHandCodes;
- (int *)handCodesWeights;

- (id)init;

- (void)reset;

- (BOOL)addPlayer:(NSString *)handCode at:(NSUInteger)index;
- (BOOL)addPlayer:(NSString *)handCode at:(NSUInteger)index withWeights:(int[PODDS_MAX_HANDS_PER_PLAYER])hcWeights 
		excludedCards:(NSString *)exclCards;

@end

NSMutableDictionary *percentageHands;

NSString* percentageHandLabel(NSString *percentage);
NSString* percentageHandCode(NSString *percentage);
NSArray* percentageHand(NSString *percentage);


//returns the number of chars written to the handBuf for the player, or -error code if invalid hand
int expandHands(NSString* handCode, char* handBuf);
int expandHands_inclSubCounts(NSString* handCode, char* handBuf, int hcCounts[], NSString* excludedCards);
void initPecentageHands();

