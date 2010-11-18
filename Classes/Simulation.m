//
//  Simulation.m
//  sniper
//
//  Created by Mark Blackwell on 16/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Simulation.h"


@implementation Simulation

@synthesize numPlayers;

//@synthesize numHands;
- (int *)numHands {
	return numHands;
}

- (int *)numHands_weighted {
	return numHands_weighted;
}

- (int *)numHandCodes {
	return numHandCodes;
}

- (int *)handCodesWeights {
	return handCodesWeights;
}


@synthesize handCodes;

@synthesize numFlopped;
@synthesize numDead;
@synthesize numCards;
//@property (nonatomic) int numSims;
@synthesize hands;
@synthesize flop;
@synthesize dead;

//- (char *)flop {
//	return flop;
//}
//
//- (void)setFlop:(char *) newflop {
//	flop = newflop;
//}

- (void)setFlopString:(NSString *) flopStr {
	//store the flop cards at the end of the hand buffer
	flop = bufMarker;
	numFlopped = flopStr.length / 2;
	
	[handCodes setObject:flopStr forKey:FLOP_STRING_ARCHIVE_KEY];
	
	NSRange rng;
	rng.location = 0;
	rng.length = 2;
	int i;
	for (i = 0; i < numFlopped; i++) {
		
		//HACK: assumes that the buffer has 10 characters available
		[[flopStr substringWithRange:rng] getCString:bufMarker maxLength:10 encoding:NSASCIIStringEncoding];
		bufMarker[2] = '\0';
		bufMarker += PODDS_CHARS_PER_CARD;
		rng.location += 2;
	}
}

- (void)setDeadString:(NSString *) deadStr {
	//store the dead cards at the end of the hand buffer
	dead = bufMarker;
	numDead = deadStr.length / 2;
	
	[handCodes setObject:deadStr forKey:DEAD_STRING_ARCHIVE_KEY];
	
	NSRange rng;
	rng.location = 0;
	rng.length = 2;
	int i;
	for (i = 0; i < numDead; i++) {
		
		//HACK: assumes that the buffer has 10 characters available
		[[deadStr substringWithRange:rng] getCString:bufMarker maxLength:10 encoding:NSASCIIStringEncoding];
		bufMarker[2] = '\0';
		bufMarker += PODDS_CHARS_PER_CARD;
		rng.location += 2;
	}
}


//@synthesize buf;
- (char *)buf {
	return buf;
}

@synthesize bufMarker;

@synthesize numSims;

@synthesize liteTrialCount;

@synthesize isFirstFlopRiverTurnCycle;

@synthesize useCount;

@synthesize isFirstUseOfHandWeighting;

@synthesize version;

//@synthesize results;
- (simulation_result *)results {
	return results;
}

#pragma mark NSCoding
- (void)encodeWithCoder:(NSCoder *) encoder {
	//always encode the handcodes
	[encoder encodeObject:handCodes forKey:kHandCodeKey];
	[encoder encodeInt:liteTrialCount forKey:kLiteTrialCount];
	[encoder encodeBool:isFirstFlopRiverTurnCycle forKey:kIsFirstFlopRiverTurnCycle];
    [encoder encodeInt:useCount forKey:kUseCount];
	[encoder encodeBool:isFirstUseOfHandWeighting forKey:kIsFirstUseOfHandWeighting];
	[encoder encodeObject:version forKey:kVersion];
	
	if (bufMarker == buf || (bufMarker - buf > MAX_ENCODEABLE_BUFSIZE)) {
		//there is nothing to encode...
		[encoder encodeBool:NO forKey:kWereResultsEncoded];
	}
	else {
		[encoder encodeBool:YES forKey:kWereResultsEncoded];
		
		[encoder encodeInt:numPlayers forKey:kNumPlayersKey];
		[encoder encodeArrayOfObjCType:@encode(int) count:PODDS_MAX_PLAYERS + 1 at:numHands];
		[encoder encodeInt:numFlopped forKey:kNumFloppedKey];
		[encoder encodeInt:numDead forKey:kNumDeadKey];
		[encoder encodeInt:numCards forKey:kNumCardsKey];
		[encoder encodeInt:(bufMarker - hands) forKey:kBufMarkerOffset];
		[encoder encodeInt:(flop - hands) forKey:kFlopOffset];
		[encoder encodeInt:(dead - hands) forKey:kDeadOffset];
		
		int numBufChars = bufMarker - hands;
		[encoder encodeArrayOfObjCType:@encode(char) count:numBufChars at:buf];
		
		//create a string for each simulation_result
		
		//result has three ints and the float split equity
		int i;
		char resultBuf[256];
		for (i = 0; i < PODDS_MAX_PLAYERS + 1; i++) {
			sprintf(resultBuf, "%d,%d,%d,%9.2f", results[i].rounds, results[i].won, results[i].split, results[i].splitEquity);
			NSString *key = [NSString stringWithFormat:kResultForPlayerPrefix, i];
			[encoder encodeObject:[NSString stringWithCString:resultBuf encoding:NSASCIIStringEncoding] forKey:key];
		}	
	}
}


- (id)initWithCoder:(NSCoder *)decoder {
	if (self = [super init]) {
		//handcodes are always encoded
		if ([decoder containsValueForKey:kHandCodeKey])
			self.handCodes = [decoder decodeObjectForKey:kHandCodeKey];
		else
			handCodes = [[NSMutableDictionary alloc] init];
		
		if ([decoder containsValueForKey:kLiteTrialCount])
			self.liteTrialCount = [decoder decodeIntForKey:kLiteTrialCount];
		else
			self.liteTrialCount = 0;
		
		if ([decoder containsValueForKey:kIsFirstFlopRiverTurnCycle])
			self.isFirstFlopRiverTurnCycle = [decoder decodeBoolForKey:kIsFirstFlopRiverTurnCycle];
		else
			self.isFirstFlopRiverTurnCycle = YES;
        
        if ([decoder containsValueForKey:kUseCount])
			self.useCount = [decoder decodeIntForKey:kUseCount];
		else
			self.useCount = 0;
        
        if ([decoder containsValueForKey:kIsFirstFlopRiverTurnCycle])
			self.isFirstUseOfHandWeighting = [decoder decodeBoolForKey:kIsFirstUseOfHandWeighting];
		else
			self.isFirstUseOfHandWeighting = YES;
		
		if ([decoder containsValueForKey:kVersion])
			self.version = [decoder decodeObjectForKey:kVersion];
		else
			self.version = UNKNOWN_VERSION;		
		
		self.numSims = NUM_SIMS;
		
		BOOL isResults = NO;
		if ([decoder containsValueForKey:kWereResultsEncoded]) {
			isResults = ([decoder decodeBoolForKey:kWereResultsEncoded] &&
						 [decoder containsValueForKey:kNumPlayersKey] &&
						 [decoder containsValueForKey:kNumFloppedKey] &&
						 [decoder containsValueForKey:kNumDeadKey] &&
						 [decoder containsValueForKey:kNumCardsKey] &&
						 [decoder containsValueForKey:kBufMarkerOffset] &&
						 [decoder containsValueForKey:kFlopOffset] &&
						 [decoder containsValueForKey:kDeadOffset]);
			
			if (isResults) {
				int i;
				for (i = 0; i < PODDS_MAX_PLAYERS + 1; i++) {
					NSString *key = [NSString stringWithFormat:kResultForPlayerPrefix, i];
					isResults = [decoder containsValueForKey:key];
				}
			}
		}
		
		//for ridiculously long buffers the encode/decode process takes too long, so they are invalid
		int bufLen = [decoder decodeIntForKey:kBufMarkerOffset];
		isResults = isResults && (bufLen <= MAX_ENCODEABLE_BUFSIZE);
		
		
		if (isResults) {
			self.numPlayers = [decoder decodeIntForKey:kNumPlayersKey];
			
			//don't know how to ensure this happens...
			[decoder decodeArrayOfObjCType:@encode(int) count:PODDS_MAX_PLAYERS + 1 at:numHands];
			
			self.numFlopped = [decoder decodeIntForKey:kNumFloppedKey];
			self.numDead = [decoder decodeIntForKey:kNumDeadKey];
			self.numCards = [decoder decodeIntForKey:kNumCardsKey];
			if (self.numCards < PODDS_FLOP_CARDS_PER_HAND || self.numCards > PODDS_RIVER_CARDS_PER_HAND)
				self.numCards = PODDS_RIVER_CARDS_PER_HAND;
			
			self.hands = [self buf];
			self.bufMarker = self.hands + [decoder decodeIntForKey:kBufMarkerOffset];
			self.flop = self.hands + [decoder decodeIntForKey:kFlopOffset];
			self.dead = self.hands + [decoder decodeIntForKey:kDeadOffset];
			
			int numBufChars = bufMarker - hands;
			[decoder decodeArrayOfObjCType:@encode(char) count:numBufChars at:buf];
			
			//decode a string for each simulation_result
			//result has three ints
			int i;
			char resultBuf[256];
			for (i = 0; i < PODDS_MAX_PLAYERS + 1; i++) {
				NSString *key = [NSString stringWithFormat:kResultForPlayerPrefix, i];
				NSString *result = [decoder decodeObjectForKey:key];
				[result getCString:resultBuf maxLength:256 encoding:NSASCIIStringEncoding];
				
				if ([version isEqualToString:UNKNOWN_VERSION]) {
					sscanf(resultBuf, "%d,%d,%d", &results[i].rounds, &results[i].won, &results[i].split);
					results[i].splitEquity = 0.5 * results[i].split;
				}
				else {
					sscanf(resultBuf, "%d,%d,%d,%9.2f", &results[i].rounds, &results[i].won, &results[i].split, &results[i].splitEquity);
				}
			}	
		}
		else {
			bufMarker = buf;
			hands = buf;
			flop = NULL;
			dead = NULL;
			numPlayers = 0;
			numFlopped = 0;
			numDead = 0;
			numCards = PODDS_RIVER_CARDS_PER_HAND;
		}
		
		initPecentageHands();
	}
	
	return self;
}

#pragma mark -
#pragma mark NSCopying
- (id)copyWithZone:(NSZone *)zone {
	Simulation *copy = [[[self class] allocWithZone:zone] init];
	copy.numPlayers = self.numPlayers;
	copy.numFlopped = self.numFlopped;
	copy.numDead = self.numDead;
	//copy.numSims = self.numSims;
	//copy.percentDone = self.percentDone;
	copy.numCards = self.numCards;
	copy.version = self.version;
	
	//hands just points to the start of the buffer
	copy.hands = [copy buf];
	
	//offset the bufmarker the same distance from the start
	copy.bufMarker = [copy buf] + (self.bufMarker - self.buf);
	
	//offset the flop the same way (it's part of the buf)
	copy.flop = [copy buf] + (self.flop - [self buf]);
	copy.dead = [copy buf] + (self.dead - [self buf]);
	
	//HACK: do a shallow copy... but have used "retain" in property??
	copy.handCodes = self.handCodes;
	
	int i;
	for (i = 0; i < PODDS_MAX_PLAYERS + 1; i++) {
		[copy numHands][i] = [self numHands][i];
		
		[copy results][i].rounds = [self results][i].rounds;
		[copy results][i].player = [self results][i].player;
		[copy results][i].won = [self results][i].won;
		[copy results][i].split = [self results][i].split;

		//these fields are ignored...
		//char card1, card2;
		//unsigned int winclasses[10], looseclasses[10];
	}
	
	copy.liteTrialCount = self.liteTrialCount;
	copy.isFirstFlopRiverTurnCycle = self.isFirstFlopRiverTurnCycle;
    copy.useCount = self.useCount;
    copy.isFirstUseOfHandWeighting = self.isFirstUseOfHandWeighting;
	
	return copy;
}
#pragma mark -

- (id)init {
    if (self = [super init]) {
        bufMarker = buf;
		hands = buf;
		flop = NULL;
		dead = NULL;
		numPlayers = 0;
		numFlopped = 0;
		numDead = 0;
		numSims = NUM_SIMS;
		numCards = 7;
		liteTrialCount = 0;
		isFirstFlopRiverTurnCycle = YES;
        useCount = 0;
        isFirstUseOfHandWeighting = YES;
		version = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"] description];
		
		handCodes = [[NSMutableDictionary alloc] init];
		
		initPecentageHands();
    }
	
    return self;
}

- (void)reset {
	bufMarker = buf;
	flop = NULL;
	dead = NULL;
	numPlayers = 0;
	numFlopped = 0;
	numDead = 0;
	version = [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"] description];
	
	int p;
	for (p = 0; p < PODDS_MAX_PLAYERS + 1; p++) {
		results[p].rounds = 0;
		results[p].player = 0;
		results[p].won = 0;
		results[p].split = 0;
		results[p].splitEquity = 0.0;
		
		numHandCodes[p] = 0;
		int hc;
		for (hc = 0; hc < PODDS_MAX_HANDS_PER_PLAYER; hc++) {
			numHands_weighted[p][hc] = 0;
			handCodesWeights[p][hc] = 0;
		}
	}	
	
	[handCodes removeAllObjects];
}

/* - (BOOL)addPlayer:(NSString *)handCode at:(NSUInteger)index {
	//take a local copy of the hand code
	NSString *localHandCode = [NSString stringWithString:handCode];
	
	int hcCounts[PODDS_MAX_HANDS_PER_PLAYER];
	int charCount = expandHands_inclSubCounts(localHandCode, bufMarker, hcCounts);
	if (charCount > 0) {
		bufMarker += charCount;
		//this should be a round number...
		numHands[numPlayers] = charCount / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
		
		//we still need to maintain the weighted arrays, mostly for percent hands in the current implementation
		numHands_weighted[numPlayers][0] = numHands[numPlayers];
		
		//this is not strictly true, but given unweighted is requested, it works just fine
		numHandCodes[numPlayers] = 1;
		
		//just weight the first handcode
		handCodesWeights[numPlayers][0] = 1;
		
		numPlayers++;
		
		int p;
		for (p = 0; p < PODDS_MAX_PLAYERS + 1; p++)
			results[p].player = numPlayers;
		
		[handCodes setObject:localHandCode forKey:[NSString stringWithFormat:@"%d", index]];
		
		return YES;
	}
	
	return NO;
}
 */

- (BOOL)addPlayer:(NSString *)handCode at:(NSUInteger)index {
	return [self addPlayer:handCode at:index withWeights:NULL excludedCards:NULL];
}


- (BOOL)addPlayer:(NSString *)handCode at:(NSUInteger)index withWeights:(int[PODDS_MAX_HANDS_PER_PLAYER])hcWeights 
	excludedCards:(NSString *)exclCards {
	//take a local copy of the hand code
	NSString *localHandCode = [NSString stringWithString:handCode];
	
	int hcCounts[PODDS_MAX_HANDS_PER_PLAYER];
	int charCount = expandHands_inclSubCounts(localHandCode, bufMarker, hcCounts, exclCards);
	if (charCount > 0) {
		bufMarker += charCount;
		//this should be a round number...
		numHands[numPlayers] = charCount / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
		
		int i = 0, c = 0;
		while (c < charCount) {
			numHands_weighted[numPlayers][i] = hcCounts[i] / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);
			c += hcCounts[i];
			i++;
		}
		
		numHandCodes[numPlayers] = i;
		
		int hc;
		for (hc = 0; hc < numHandCodes[numPlayers] && hc < PODDS_MAX_HANDS_PER_PLAYER; hc++) {
			handCodesWeights[numPlayers][hc] = (hcWeights != NULL ? hcWeights[hc] : numHands_weighted[numPlayers][hc]);
		}
		
		numPlayers++;
		
		int p;
		for (p = 0; p < PODDS_MAX_PLAYERS + 1; p++)
			results[p].player = numPlayers;
		
		[handCodes setObject:localHandCode forKey:[NSString stringWithFormat:@"%d", index]];
		
		return YES;
	}
	
	return NO;
}

- (void)dealloc {
    [super dealloc];
	
	[handCodes release];
	[percentageHands release];
}




@end

int expandHands(NSString* handCode, char* handBuf) {
	return expandHands_inclSubCounts(handCode, handBuf, NULL, NULL);
}

int expandHands_inclSubCounts(NSString* handCode, char* handBuf, int hcCounts[], NSString* excludedCards) {
	
	char inbuf[4096];
	//first look for a percent hand
	NSRange percentRange = [handCode rangeOfString:@"%"];
	if(percentRange.location == NSNotFound) {
		[handCode getCString:inbuf maxLength:1024 encoding:NSASCIIStringEncoding];
	}
	else {
		//HACK: assumes that handcode is always 1-100%... doesn't check for anything else
		NSString *percent = [handCode substringToIndex:percentRange.location];
		NSString *percentHandCode = percentageHandCode(percent);
		int maxLen = [percentHandCode lengthOfBytesUsingEncoding:NSASCIIStringEncoding] + 1;
		[percentHandCode getCString:inbuf maxLength:maxLen encoding:NSASCIIStringEncoding];
	}
	
	int charCount = getPossibleHands_csvHandCode_inclSubCounts(inbuf, handBuf, hcCounts);
	
	if (![excludedCards isEqualToString:@""]) {
		NSRange rng;
		rng.location = 0;
		rng.length = 2;
		char card[PODDS_CHARS_PER_CARD];
		while (rng.location < excludedCards.length) {
			NSString* exclCard = [excludedCards substringWithRange:rng];
			
			[exclCard getCString:card maxLength:PODDS_CHARS_PER_CARD encoding:NSASCIIStringEncoding];
			int c = 0, hcc = 0, hccIx = 0, posn = 0;
			while (c < charCount) {
				BOOL didRemove = NO;
				if (handBuf[c] == card[0] && handBuf[c+1] == card[1]) {										
					//reduce charCount
					charCount -= PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS;
					
					//can't move the last card combo to this location... so just shuffle forward
					int ic = c - (posn * PODDS_CHARS_PER_CARD);
					while (ic < charCount) {
						handBuf[ic] = handBuf[ic + (PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS)];
						ic++;
					}
					
					//decrement hcCounts
					hcCounts[hccIx] -= PODDS_CHARS_PER_CARD * PODDS_NUM_DEALT_CARDS;
					
					didRemove = YES;
				}
				
				if (didRemove) {
					//move c backwards 3 characters if we are looking second card
					c -= (posn * PODDS_CHARS_PER_CARD);
					hcc -= (posn * PODDS_CHARS_PER_CARD);
					posn = 0;
				}
				else {
					c += PODDS_CHARS_PER_CARD;
					hcc += PODDS_CHARS_PER_CARD;
					posn = (posn == 0 ? 1 : 0);
				}

				
				//the handcode counts will always be a multiple of PODDS_CHARS_PER_CARD
				if (hcc == hcCounts[hccIx]) {
					hccIx++;
					hcc = 0;
				}
			}
			
			rng.location += 2;
		}
		
	}
	
	return charCount;	
}

void initPecentageHands() {
	NSString *appPath = [[NSBundle mainBundle] resourcePath];
	NSString *path = [appPath stringByAppendingPathComponent:PERCENTAGEHANDS_FILENAME];
	
	NSString *contents = [NSString stringWithContentsOfFile:path encoding:NSASCIIStringEncoding error:NULL];
	NSArray *lines = [contents componentsSeparatedByString:@"\n"];
	
	//no need to worry about memory management as this will remain in place while app is running
	percentageHands = [[NSMutableDictionary alloc] initWithCapacity:256];
	
	for (NSString *line in lines) {
		if (line.length > 0) {
			NSArray *parts = [line componentsSeparatedByString:@";"];
			if (parts.count == 4)
				[percentageHands setObject:parts forKey:[parts objectAtIndex:0]];
		}
	}
}

NSString* percentageHandLabel(NSString *percentage) {
	NSString* handLabel = [[percentageHand(percentage) objectAtIndex:2] description];
	if (handLabel == nil)
		return @"error!";
	
	return handLabel;
}

NSString* percentageHandCode(NSString *percentage) {
	//HACK: will crash if >100%
	return [[percentageHand(percentage) objectAtIndex:3] description];
}

NSArray* percentageHand(NSString *percentage) {
	NSString *cleanpc = [percentage stringByTrimmingCharactersInSet:[[NSCharacterSet decimalDigitCharacterSet] invertedSet]];
	int rounded = [cleanpc intValue];
	if (rounded > 100)
		return nil;
	
	NSArray *handData = [percentageHands objectForKey:[NSString stringWithFormat:@"%d", rounded]];
	
	return handData;
}

