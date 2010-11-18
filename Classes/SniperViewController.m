//
//  sniperViewController.m
//  sniper
//
//  Created by Mark Blackwell on 28/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "SniperViewController.h"



@implementation SniperViewController

@synthesize lastSim;

@synthesize helpScreen;

@synthesize activeLabel;

@synthesize stopSign;

@synthesize runDownLastPressedAt;

@synthesize activeComboCount;


// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		NSString *archivePath = [self dataFilePath];
		if (USE_ARCHIVE && [[NSFileManager defaultManager] isReadableFileAtPath:archivePath]) {
			NSLog(@"Recovering last sim...");
			
			@try {
				self.lastSim = [NSKeyedUnarchiver unarchiveObjectWithFile:archivePath];
			} 
			@catch ( NSException *e ) {
				self.lastSim = [[Simulation alloc] init];
			}
			
		}
		else {
			//this has the same lifetime as the app, so no need to release it...
			self.lastSim = [[Simulation alloc] init];
		}
				
		UIApplication *app = [UIApplication sharedApplication];
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(applicationWillTerminate:)
													 name:UIApplicationWillTerminateNotification
												   object:app];		
		
    }
    return self;
}

- (void)viewDidLoad {
	//NSLog(@"Starting...");
	NSLog(@"SniperVC viewDidLoad\n");
	
	[self handLabelSelected:playerOneHand];
	[self.view bringSubviewToFront:altBoardDeadValue];
	
	self.stopSign = [UIImage imageNamed:@"50px-Stop_cross.png"];
	
	playerHands[0] = playerOneHand;
	playerHands[1] = playerTwoHand;
	playerHands[2] = playerThreeHand;
	playerHands[3] = playerFourHand;
	
	//title seems to be nil in OS3.0 builds, which interferes with operation
	int p;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		[playerHands[p] setTitle:@"" forState:UIControlStateNormal];
		playerHands[p].titleLabel.adjustsFontSizeToFitWidth = YES; 
		playerHands[p].titleLabel.minimumFontSize = 10;
	}
	[flop setTitle:@"" forState:UIControlStateNormal];
	[deadCards setTitle:@"" forState:UIControlStateNormal];
	[deadCards retain];
	[deadCards removeFromSuperview];
	
	flopRiverTurnButton.titleLabel.lineBreakMode = UILineBreakModeWordWrap;
	flopRiverTurnButton.titleLabel.numberOfLines = 2;
	flopRiverTurnButton.titleLabel.textAlignment = UITextAlignmentCenter;

	
	playerPercentInfos[0] = playerOnePercentInfo;
	playerPercentInfos[1] = playerTwoPercentInfo;
	playerPercentInfos[2] = playerThreePercentInfo;
	playerPercentInfos[3] = playerFourPercentInfo;	
	
	playerEqs[0] = playerOneEq;
	playerEqs[1] = playerTwoEq;
	playerEqs[2] = playerThreeEq;
	playerEqs[3] = playerFourEq;
	
	//playerOdds[0] = playerOneOdds;
	//playerOdds[1] = playerTwoOdds;
	//playerOdds[2] = playerThreeOdds;
	//playerOdds[3] = playerFourOdds;	
	
	playerWins[0] = playerOneWins;
	playerWins[1] = playerTwoWins;
	playerWins[2] = playerThreeWins;
	playerWins[3] = playerFourWins;	
	
	playerLosses[0] = playerOneLosses;
	playerLosses[1] = playerTwoLosses;
	playerLosses[2] = playerThreeLosses;
	playerLosses[3] = playerFourLosses;	
	
	playerTies[0] = playerOneTies;
	playerTies[1] = playerTwoTies;
	playerTies[2] = playerThreeTies;
	playerTies[3] = playerFourTies;	
	
	NSLog(@"SniperVC viewDidLoad 2\n");
	
	NSString *handCode;
	int i;
	for (i = 0; i < MAX_PLAYERS_ON_SCREEN; i++) {
		if ((handCode = [lastSim.handCodes objectForKey:[NSString stringWithFormat:@"%d", i]]) != nil) {
			[playerHands[i] setTitle:handCode forState:UIControlStateNormal];
			if (handCode.length <= 4) {
				if (isPercentHand([handCode cStringUsingEncoding:NSASCIIStringEncoding])) {
					NSString *percentHandExpl = percentageHandLabel(playerHands[i].currentTitle);
					playerPercentInfos[i].text = percentHandExpl;
				}
			}
		}
	}
	
	if ((handCode = [lastSim.handCodes objectForKey:FLOP_STRING_ARCHIVE_KEY]) != nil)
		[flop setTitle:handCode forState:UIControlStateNormal];
	
	if ((handCode = [lastSim.handCodes objectForKey:DEAD_STRING_ARCHIVE_KEY]) != nil)
		[deadCards setTitle:handCode forState:UIControlStateNormal];
	
	[self updateFlopRiverTurnButton];
	
	isDeadCardsShowing = NO;
	[self updateBoardDeadUI];
	
	[self updateProgress];
	
	NSLog(@"SniperVC viewDidLoad 3\n");
	
	if (!isInited && !isHandEvalInited) {
		[NSThread detachNewThreadSelector:@selector(initWorker) toTarget:self withObject:nil];
        
        if (lastSim.useCount < SNIPER_HANDWEIGHTING_INFO_SHOWS) {
            [SniperViewController displayAlert:@"Sniper v.3 features an advanced feature - Hand Weighting/Edit Combos.  See help page for details on how to activate and use this." entitled:@"Sniper v3.0"];
			lastSim.useCount++;
		}
        
		isInited = YES;
	}
	//this situation occurs when view is unloaded then re-loaded
	else if (!isInited) {
		[self enablePostInit];
		isInited = YES;
	}
	
	NSLog(@"SniperVC viewDidLoad 4\n");
	
	[super viewDidLoad];
}

- (NSString *)dataFilePath {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *docsDirectory = [paths objectAtIndex:0];
	return [docsDirectory stringByAppendingPathComponent:ARCHIVE_NAME];
}

- (NSString *)settingsFilePath {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *docsDirectory = [paths objectAtIndex:0];
	return [docsDirectory stringByAppendingPathComponent:SNIPER_PRIVATE_DATAFILE_NAME];
}

- (void)initWorker {
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	if (!isHandEvalInited) {
		//how do i lock isHandEvalInited?  could use a C semaphore??
		//NSLog(@"Initialising 0...");
						
		//NSLog(@"Initialising 1...");
		//init_deck(&deck);
		
		NSString *appPath = [[NSBundle mainBundle] resourcePath];
		
		//NSLog(@"Initialising 2...");
		
		appPath = [appPath stringByAppendingString:@"/"];
		
		//NSLog(@"Initialising 3...");
		//NSString *appDir = [appPath stringByDeletingPathExtension];
		char buf[1024];
		[appPath getCString:buf maxLength:512 encoding:NSASCIIStringEncoding];
		
		//NSLog(@"Initialising 4...");
		
		handeval_init(buf);
		
		//NSLog(@"Initialising 5...");
		
		init_random ( time(NULL) );
		
		//NSLog(@"Initialising 6...");
		
		//numIters.text = @"Init'd";
		NSLog(@"Finished initialising");
		
		isHandEvalInited = YES;
		
		[self performSelectorOnMainThread:@selector(enablePostInit)
							   withObject:nil
							waitUntilDone:YES];		
		
	}
	
	[pool release];

}

- (void)enablePostInit {
	waitInitLabel.hidden = YES;
	[runButton setTitle:@"=" forState:UIControlStateNormal];
	runButton.enabled = YES;
}

- (IBAction)buttonPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;

	UIButton *btn = (UIButton*)sender;
	
	//NSLog([NSString stringWithFormat:@"entering buttonPressed %@", activeLabel.currentTitle]);
	
	[self appendActiveLabel:btn.currentTitle];
	
	[self validateActiveLabel];
	
	//NSLog([NSString stringWithFormat:@"exiting buttonPressed %@", activeLabel.currentTitle]);
}

- (IBAction)click:(id)sender {
//	SystemSoundID soundID = 0x450;
//	AudioServicesPlaySystemSound(soundID);	
}

- (void)appendActiveLabel:(NSString*)card {
	if (activeLabel != nil) {
		[self removeComboCounts];
		
		//NSLog([NSString stringWithFormat:@"appending %@", card]);
		[activeLabel setTitle:[activeLabel.currentTitle stringByAppendingString:card] forState:UIControlStateNormal];
	}
}

- (IBAction)percentPressed:(id) sender {
	if (activeLabel == flop || activeLabel == deadCards)
		return;
	
	if ([self isActiveLabelPercentHand])
		return;
	
	[self appendActiveLabel:@"%"];
	
	NSString *percentHandExpl = percentageHandLabel(activeLabel.currentTitle);
	playerPercentInfos[activeLabel.tag - 1].text = percentHandExpl;
	
	[self validateActiveLabel];
}

- (BOOL)isActiveLabelPercentHand {
	return isPercentHand([activeLabel.currentTitle cStringUsingEncoding:NSASCIIStringEncoding]);
}

- (IBAction)heartsPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;

	[self appendActiveLabel:@"h"];
	
	[self validateActiveLabel];
}

- (IBAction)diamondsPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;

	[self appendActiveLabel:@"d"];
	
	[self validateActiveLabel];
}

- (IBAction)spadesPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;

	[self appendActiveLabel:@"s"];
	
	[self validateActiveLabel];
}

- (IBAction)clubsPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;
	
	[self appendActiveLabel:@"c"];
	
	[self validateActiveLabel];
}

- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}

- (IBAction)backspacePressed:(id) sender {
	if (activeLabel != nil) {
		[self removeComboCounts];
		
		NSUInteger len = [activeLabel.currentTitle length];
		if (len > 0) {
			if ([self isActiveLabelPercentHand])
				playerPercentInfos[activeLabel.tag - 1].text = @"";
			
			[activeLabel setTitle:[activeLabel.currentTitle substringToIndex:(len - 1)] forState:UIControlStateNormal];
		}
		
		[self validateActiveLabel];
	}
}

- (IBAction)clearPressed:(id) sender {
	if (activeLabel != nil) {
		[self removeComboCounts];
		
		[activeLabel setTitle:@"" forState:UIControlStateNormal];
		
		if (activeLabel != flop && activeLabel != deadCards)
			playerPercentInfos[activeLabel.tag - 1].text = @"";
	}
}

- (IBAction)clearDoublePressed:(id)sender {
	[self removeComboCounts];
	
	int p;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		[playerHands[p] setTitle:@"" forState:UIControlStateNormal];
		playerPercentInfos[p].text = @"";
	}
	
	[flop setTitle:@"" forState:UIControlStateNormal];
	[deadCards setTitle:@"" forState:UIControlStateNormal];
	
	[lastSim reset];
	
	[self updateProgress];
	[self updateBoardDeadUI];
}

- (UIView *)view:(SniperView *)view hitTest:(CGPoint)point
       withEvent:(UIEvent *)event hitView:(UIView *)hitView {
    if (event.type == UIEventTypeTouches)
		lastTouch = point;
	
	return hitView;
}

- (IBAction)handLabelSelected:(id)sender {
		
	if (activeComboCount) {
		activeComboCount.comboCountLabel.textColor = [UIColor blackColor];

		//[activeComboCount release];
		self.activeComboCount = nil;
	}
	
	if (isComboCountsShowing && activeLabel == sender && activeLabel != flop && activeLabel != deadCards) {
		//get the location of the last tap
		CGPoint pt = [self.view convertPoint:lastTouch toView:activeLabel];
		
		//compare it on the x-direction with the set of combo count labels
		for (HandComboCount *hcc in handCodeComboCounts[activeLabel.tag - 1]) {
			assert_local (hcc.hand == activeLabel)
			CGRect lblRect = hcc.comboCountLabel.frame;
			
			if (pt.x > lblRect.origin.x && pt.x < (lblRect.origin.x + lblRect.size.width)) {
				//make it the active combo count
				self.activeComboCount = hcc;
				
				//change the colour to red
				hcc.comboCountLabel.textColor = [UIColor redColor];
                
                if (lastSim.isFirstUseOfHandWeighting) {
                    [SniperViewController displayAlert:@"To adjust % hands up and down, use the new up/down arrows in the keypad" entitled:@"Sniper v3.0"];
                    lastSim.isFirstUseOfHandWeighting = NO;
                }
                
				
				break;
			}
		}
	}
	else if (activeLabel != sender) {
		//remove any existing highlight
		if (activeLabel != nil)
			activeLabel.backgroundColor = nil;			
		
		self.activeLabel = (UIButton*)sender;
		
		//highlight selection white
		activeLabel.backgroundColor = [UIColor whiteColor];
		
	}
}

- (IBAction)randomHandPressed:(id) sender {
	if ([self isActiveLabelPercentHand])
		return;

	if (activeLabel == nil)
		return;
	
	if (activeLabel == flop || activeLabel == deadCards) 
		return;
	
	[self removeComboCounts];
	
	[activeLabel setTitle:@"xxxx" forState:UIControlStateNormal];
}

- (IBAction)downArrowTouchedDown:(id) sender {
	arrowRepeatTimer = [NSTimer scheduledTimerWithTimeInterval:PRESS_AND_HOLD_SECS target:self selector:@selector(doDownArrow) userInfo:nil repeats:YES];
	arrowRepeatTimerIters = 0;
}

- (IBAction)downArrowTouchedUpOutside:(id) sender {
	if (arrowRepeatTimer != nil) {
		[arrowRepeatTimer invalidate];
		arrowRepeatTimer = nil;
    }
	
	arrowRepeatTimerIters = 0;
}

- (IBAction)downArrowTouchedUp:(id) sender {
	if (arrowRepeatTimer != nil) {
		[arrowRepeatTimer invalidate];
		arrowRepeatTimer = nil;
    }
	
	if (arrowRepeatTimerIters == 0)
		[self doDownArrow];
	
	arrowRepeatTimerIters = 0;
}

- (void)doDownArrow {
	if (activeLabel == flop || activeLabel == deadCards)
		return;
	
	if ([self isActiveLabelPercentHand]) {
		[self adjustPercentHand:activeLabel by:-1];
	}
	else if (activeComboCount != nil && activeComboCount.userAdjustedCount > 1) {
		activeComboCount.userAdjustedCount--;
		activeComboCount.comboCountLabel.text = [NSString stringWithFormat:@"(%d)", activeComboCount.userAdjustedCount];
	}
	
	arrowRepeatTimerIters++;
}

- (IBAction)upArrowTouchedDown:(id) sender {
	arrowRepeatTimer = [NSTimer scheduledTimerWithTimeInterval:PRESS_AND_HOLD_SECS target:self selector:@selector(doUpArrow) userInfo:nil repeats:YES];
	arrowRepeatTimerIters = 0;
}

- (IBAction)upArrowTouchedUpOutside:(id) sender {
	if (arrowRepeatTimer != nil) {
		[arrowRepeatTimer invalidate];
		arrowRepeatTimer = nil;
    }
	
	arrowRepeatTimerIters = 0;
}

- (IBAction)upArrowTouchedUp:(id) sender  {
	if (arrowRepeatTimer != nil) {
		[arrowRepeatTimer invalidate];
		arrowRepeatTimer = nil;
    }
	
	if (arrowRepeatTimerIters == 0)
		[self doUpArrow];
	
	arrowRepeatTimerIters = 0;
}

- (void)doUpArrow {
	if (activeLabel == flop || activeLabel == deadCards)
		return;
	
	if ([self isActiveLabelPercentHand]) {
		[self adjustPercentHand:activeLabel by:1];
	}
	else if (activeComboCount != nil && activeComboCount.userAdjustedCount < activeComboCount.rawCount) {
		activeComboCount.userAdjustedCount++;
		activeComboCount.comboCountLabel.text = [NSString stringWithFormat:@"(%d)", activeComboCount.userAdjustedCount];
	}
	
	arrowRepeatTimerIters++;
}

- (void)adjustPercentHand:(UIButton *)playerHand by:(int)increment {
	NSString *cleanpc = [playerHand.currentTitle stringByTrimmingCharactersInSet:[[NSCharacterSet decimalDigitCharacterSet] invertedSet]];
	int rounded = [cleanpc intValue];
	
	rounded += increment;
	
	if (rounded == 0 || rounded > 100)
		return;
	
	[playerHand setTitle:[NSString stringWithFormat:@"%d", rounded] forState:UIControlStateNormal];
	
	//hack... probably shouldn't call UI code, but whatever
	[self percentPressed:playerHand];
	
}

- (IBAction)cycleFlopRiverTurn:(id)sender {
	if (lastSim == nil)
		return;
	
	if (isWorking)
		return;
	
	if (lastSim.numCards != PODDS_RIVER_CARDS_PER_HAND)
		lastSim.numCards++;
	else
		lastSim.numCards = PODDS_FLOP_CARDS_PER_HAND;
	
	[self updateFlopRiverTurnButton];
	
	if (lastSim.isFirstFlopRiverTurnCycle) {
		[SniperViewController displayAlert:@"By default, PokerSniper deals to the river, meaning that hand equity is calculated on all 7 cards (2 + 5).  Deal to flop ends the hand and calculates equity with 3 community cards dealt, whilst Deal to turn ends the hand with 4 community cards dealt.  This message will not be shown again." entitled:@"Deal to FLOP/TURN/RIVER"];
		lastSim.isFirstFlopRiverTurnCycle = NO;
	}
}

- (IBAction)cycleBoardDeadCardsView:(id)sender {
	[UIView beginAnimations:@"CycleBoardDeadCards" context:nil]; {
		[UIView setAnimationDuration:0.5];
		[UIView setAnimationBeginsFromCurrentState:YES];
		
	
		UIButton *incoming, *outgoing;
		if (isDeadCardsShowing) {
			incoming = flop;
			outgoing = deadCards;
		}
		else {
			incoming = deadCards;
			outgoing = flop;
		}
		
		//[UIView setAnimationTransition:UIViewAnimationTransitionCurlUp
		//					   forView:outgoing
		//						 cache:NO];
		
		[outgoing retain];
		[outgoing removeFromSuperview];
		
		[self.view insertSubview:incoming belowSubview:altBoardDeadValue];
		[incoming release];
		
		[self handLabelSelected:incoming];
		
		isDeadCardsShowing = !isDeadCardsShowing;
	
	} [UIView commitAnimations];
	
	[self updateBoardDeadUI];
}

- (void)updateBoardDeadUI {
	
	[boardDeadButton setTitle:(isDeadCardsShowing ? @"Dead" : @"B") forState:UIControlStateNormal];
							  
	if ((isDeadCardsShowing && ![flop.currentTitle isEqualToString:@""]) ||
		(!isDeadCardsShowing && ![deadCards.currentTitle isEqualToString:@""])) {
		[boardDeadButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
		
		altBoardDeadValue.text = [NSString stringWithFormat:@"(%@=%@)", 
								  (isDeadCardsShowing ? @"Board" : @"Dead"),
								  (isDeadCardsShowing ? flop.currentTitle : deadCards.currentTitle)];
	}
	else {
		[boardDeadButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		
		altBoardDeadValue.text = @"";
	}

	
}

- (void)updateFlopRiverTurnButton {
	assert_local (lastSim.numCards >= PODDS_FLOP_CARDS_PER_HAND && lastSim.numCards <= PODDS_RIVER_CARDS_PER_HAND);
	
	switch (lastSim.numCards) {
		case PODDS_FLOP_CARDS_PER_HAND:
			[flopRiverTurnButton setTitle:@"Deal to FLOP" forState:UIControlStateNormal];
			[flopRiverTurnButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
			break;
		case PODDS_TURN_CARDS_PER_HAND:
			[flopRiverTurnButton setTitle:@"Deal to TURN" forState:UIControlStateNormal];
			[flopRiverTurnButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
			break;
		case PODDS_RIVER_CARDS_PER_HAND:
			[flopRiverTurnButton setTitle:@"Deal to RIVER" forState:UIControlStateNormal];
			[flopRiverTurnButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
			break;
	}
}

- (BOOL)validateActiveLabel {
	if (![self validateLabel:activeLabel]) {
		[activeLabel setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
		//NSLog([NSString stringWithFormat:@"Invalid label %@", activeLabel.currentTitle]);
		return NO;
	}
	
	[activeLabel setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
	//NSLog([NSString stringWithFormat:@"Valid label %@", activeLabel.currentTitle]);
	return YES;
}

- (BOOL)validateLabel:(UIButton *) label {
	//flop is separately validated
	if (label == flop || label == deadCards)
		return YES;
	
	char validateBuf[256];
	[label.currentTitle getCString:validateBuf maxLength:128 encoding:NSASCIIStringEncoding];
	
	if (validateHand(validateBuf)) 
		return NO;
	
	return YES;
}

- (IBAction)switchHandEquityPresentationPressed:(id) sender {
	isResultsDisplaySwitched = !isResultsDisplaySwitched;
	
	if (!isWorking)
		[self updateProgress];
}

- (IBAction)showHelp { 
	if (isWorking) {
		[SniperViewController displayAlert:@"Cannot show help while calculating" entitled:@"Error"];
		return;
	}
	
	if (helpScreen == nil) {
		helpScreen = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
		helpScreen.delegate = self;
	
#ifdef USE_TRADITIONAL_LITE
		helpScreen.resourceFileName = @"help_LITE";
#else
		helpScreen.resourceFileName = @"help";
#endif
	
		helpScreen.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
	}
	[self presentModalViewController:helpScreen animated:YES];
	
	//it's retained in the property definition... do i need to release?  doesn't really matter so play safe
}

- (IBAction)runPressedDown:(id) sender {
	self.runDownLastPressedAt = [NSDate date];
}

- (void)displayComboCounts {
	if (isComboCountsShowing) {
		//update existing counts??
		
		return;
	}
	
	if (![self doPreRunChecks])
		return;
	
	//for each player hand
	int p;
	int hcCounts[PODDS_MAX_HANDS_PER_PLAYER];
	char buf[SIMULATION_BUFSIZE];
	NSString* excludedCards = [self fullySpecifiedCardsShowing];
	
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if (playerHands[p].currentTitle == @"")
			continue;
		
		//calculate the combo count except if % or have random cards/suits
		if (isPercentHand([playerHands[p].currentTitle cStringUsingEncoding:NSASCIIStringEncoding]))
			continue;
		
		//note: sum of hcCount = charCount
		//remove this hand from the excluded cards, if it's there		
		NSString* ec = [NSString stringWithString:excludedCards];
		NSArray* hands = [playerHands[p].currentTitle componentsSeparatedByString:@","];
		for (NSString* hand in hands) {
			if (hand.length == 4 && isCard([hand substringToIndex:2]) && isCard([hand substringFromIndex:2])) {
				ec = [ec stringByReplacingOccurrencesOfString:hand withString:@""];
			}
		}
		
		int charCount = expandHands_inclSubCounts(playerHands[p].currentTitle, buf, hcCounts, ec);
		
		if (charCount < 0) {
			//report error
			NSString *errStr = [NSString stringWithCString:getError() encoding:NSASCIIStringEncoding];
			[SniperViewController displayAlert:errStr entitled:[NSString stringWithFormat:@"Error in hand %d", p + 1]];
			
			return;
		}
		
		
		//subtract any hands from the count that conflict with board, dead or other players cards
		//NOTE: will only remove specific cards... general ranges and non-specific cards will be ignored
		//Andrew says that 99% of the time this feature will be used to evaluate a specific hand vs a general hand
		
		//place a label below the hand showing the count	
		
		NSArray *hcs = [playerHands[p].currentTitle componentsSeparatedByString:@HAND_SPLITTER];
		int i = 0, c = 0;
		float hOffset = 0.0;
		//get the font size, allowing for that it shrinks with long hands...
		UIFont *sizingFont = playerHands[p].titleLabel.font;
		CGFloat actualFontSz = 0.0;
		[playerHands[p].titleLabel.text sizeWithFont:sizingFont 
										 minFontSize:playerHands[p].titleLabel.minimumFontSize
									  actualFontSize:&actualFontSz
											forWidth:playerHands[p].titleLabel.frame.size.width
									   lineBreakMode:playerHands[p].titleLabel.lineBreakMode];
		if (sizingFont.pointSize != actualFontSz) {
			sizingFont = [sizingFont fontWithSize:actualFontSz];
		}
		
		handCodeComboCounts[p] = [[NSMutableArray alloc] initWithCapacity:32];
		
		
		
		//********  BUG BUG BUG BUG - WILL EXCLUDE HANDS THAT HAVE A ZERO COUNT DUE TO EXCLUDED CARDS *************
		
		
		
		
		while (c < charCount) {
			NSString *hcStr = [hcs objectAtIndex:i];
			CGSize hcsz = [hcStr sizeWithFont:sizingFont];
			CGRect lblFrame = CGRectMake(hOffset, playerHands[p].frame.size.height - 10.0, hcsz.width, 10.0);
			UILabel *lbl = [[UILabel alloc] initWithFrame:lblFrame];
			
			int hc = hcCounts[i] / (PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD);

			lbl.text = [NSString stringWithFormat:@"(%d)", hc];
			lbl.textAlignment = UITextAlignmentCenter;
			UIFont *ft = [lbl.font fontWithSize:10.0];
			lbl.font = ft;
			lbl.backgroundColor = [UIColor clearColor];
			
			[playerHands[p] addSubview:lbl];
			
			HandComboCount *hcc = [[HandComboCount alloc] init];
			hcc.hand = playerHands[p];
			hcc.handCode = hcStr;
			hcc.comboCountLabel = lbl;
			hcc.rawCount = hc;
			hcc.userAdjustedCount = hc;
			
			[handCodeComboCounts[p] insertObject:hcc atIndex:i];
			
			//add the width of the comma...
			CGSize commaSz = [@"," sizeWithFont:sizingFont];
			hOffset += hcsz.width + commaSz.width;
			c += hcCounts[i];
			i++;
			[lbl release];
			[hcc release];
		}
	}
	
	isComboCountsShowing = YES;
}

- (NSString *)fullySpecifiedCardsShowing {
	NSString* cards = [NSString stringWithString:flop.currentTitle];
	cards = [cards stringByAppendingString:deadCards.currentTitle];
	
	int p;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		NSArray* hands = [playerHands[p].currentTitle componentsSeparatedByString:@","];
		for (NSString* hand in hands) {
			if (hand.length == 4 && isCard([hand substringToIndex:2]) && isCard([hand substringFromIndex:2])) {
				cards = [cards stringByAppendingString:hand];
			}
		}
	}
	
	return cards;

}

- (void)removeComboCounts {
	if (!isComboCountsShowing)
		return;
	
	int p;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if (handCodeComboCounts[p]) {
			for (HandComboCount *hcc in handCodeComboCounts[p])
				[hcc.comboCountLabel removeFromSuperview];
			
			[handCodeComboCounts[p] release];
			handCodeComboCounts[p] = nil;
		}
	}
	
	//activeComboCount just points to one of the set, so s/be no need to release
	self.activeComboCount = nil;
	
	isComboCountsShowing = NO;
}

- (IBAction)runPressed:(id) sender {
	//press and hold of the equals button shows the hand combos
	if (!isWorking && [[NSDate date] timeIntervalSinceDate:self.runDownLastPressedAt] > PRESS_AND_HOLD_SECS) {
		if (isComboCountsShowing)
			[self removeComboCounts];
		else
			[self displayComboCounts];
				
		return;
	}
	
	
	if (isWorking && isWorkerCancelled) {
		//do nothing... wait for the cancellation to take effect
	}
	else if (isWorking) {
		isWorkerCancelled = YES;
		//change image to = sign
		[runButton setTitle:@"=" forState:UIControlStateNormal];
		[runButton setImage:nil forState:UIControlStateNormal];

	}
	else if (isHandEvalInited) {
#ifdef USE_TRADITIONAL_LITE
		[self showNagDialog];
		if (lastSim.liteTrialCount >= SNIPER_LITE_TRIALS) {
			//[SniperViewController displayError:@"You have reached the maximum number of trials." entitled:@"Sorry"];
			return;
		}
				
		lastSim.liteTrialCount++;
		
#endif
        //lastSim.useCount++;
		isWorkerCancelled = NO;
		
		if([self run]) {
			//change image to stop sign
			[runButton setTitle:@"" forState:UIControlStateNormal];
			[runButton setImage:stopSign forState:UIControlStateNormal];
		}
	}
	else {
		//hand eval not inited
		[SniperViewController displayAlert:@"PokerSniper hasn't quite finished initialising yet... please try again in a few moments"
								  entitled:@"Sorry..."];
	}
	
	//NSLog(@"Leaving runPressed");
}

#ifdef USE_TRADITIONAL_LITE

- (void)showNagDialog {
	int trials = lastSim.liteTrialCount;
	if ((trials >= SNIPER_LITE_START_NAG_AT_TRIAL_NUM) && (trials % SNIPER_LITE_TRIALS_BETWEEN_NAGS) == 0) {
		
		NSString *msg;
		if (trials >= SNIPER_LITE_TRIALS)
			msg = @"You have no more free trials... would you like to buy the full version of PokerSniper?";
		else
			msg = [NSString stringWithFormat:@"You have %d free trials remaining... would you like to buy the full version of PokerSniper?", 
				   SNIPER_LITE_TRIALS - trials];
		
		isWaitingForFullVersionDecision = YES;
		UIActionSheet *sheet = [[UIActionSheet alloc] 
								initWithTitle:msg
								delegate:self 
								cancelButtonTitle:nil 
								destructiveButtonTitle:nil 
								otherButtonTitles:@"Yes please!", @"No thanks", nil];
		
		//according to http://stackoverflow.com/questions/876209/uiactionsheet-not-responding-to-touches
		//one solution to the mysteriously unresponsive buttons is to show the action sheet in self.view.window
		[sheet showInView:self.view.window];
		
		//wait until we have an answer before doing the calc
		NSRunLoop *theRL = [NSRunLoop currentRunLoop];
		while (isWaitingForFullVersionDecision && [theRL runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]]);
		
		[sheet release];	
	}
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			//"Yes thanks"... WOOHOO!
			
			//first save down the simulation file... seems not to happen which puts back 50 trials
			[self applicationWillTerminate:nil];
			
			[[UIApplication sharedApplication] openURL:[NSURL URLWithString:kFullVersionUrl]];
			break;
		case 1:
			//"No thanks"... do nothing :(
			break;
			
		default:
			break;
	}
	isWaitingForFullVersionDecision = NO;
}

#endif


- (BOOL)doPreRunChecks {
	//user alert already occurred... just being defensive
	if (!isHandEvalInited)
		return NO;
	
	if ([self checkHandCollision])
		return NO;
	
	int p, blanks = 0; //, validHandIx = 0;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if ([playerHands[p].currentTitle isEqualToString:@""]) {
			blanks++;
			continue;
		}
	}
	
	if (blanks == MAX_PLAYERS_ON_SCREEN) {
		[SniperViewController displayAlert:@"No hands entered!" entitled:@"Whoops!"];
		return NO;
	}
	
	
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if ([playerHands[p].currentTitle isEqualToString:@""])
			continue;
		
		if ([playerHands[p].currentTitle isEqualToString:@"x"] || [playerHands[p].currentTitle isEqualToString:@"xx"]) {
			[playerHands[p] setTitle:@"xxxx" forState:UIControlStateNormal];
			[playerHands[p] setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
			
		}
		
		if (![self validateLabel:playerHands[p]]) {
			[SniperViewController displayAlert:[NSString stringWithFormat:@"Error in hand %d", p + 1] entitled:@"Whoops!"];
			
			return NO;
		}
	}
	
	//parse the flop cards
	NSString *flopStr = flop.currentTitle;
	
	//must be an even number of characters (number plus suit for each card)
	BOOL isErr = ((flopStr.length % 2) != 0);
	NSRange rng;
	rng.location = 0;
	rng.length = 2;
	while (!isErr && (rng.location + rng.length) <= flopStr.length) {
		isErr = !(isCard([flopStr substringWithRange:rng]));
		rng.location += 2;
	}
	
	if (isErr) {
		[SniperViewController displayAlert:@"Cards must each have card and rank" entitled:@"Error in flop"];
		
		return NO;
	}
	
	
	//parse the dead cards
	NSString *deadStr = deadCards.currentTitle;
	
	//must be an even number of characters (number plus suit for each card)
	isErr = ((deadStr.length % 2) != 0);
	rng.location = 0;
	rng.length = 2;
	while (!isErr && (rng.location + rng.length) <= deadStr.length) {
		isErr = !(isCard([deadStr substringWithRange:rng]));
		rng.location += 2;
	}
	
	if (isErr) {
		[SniperViewController displayAlert:@"Cards must each have card and rank" entitled:@"Error in dead cards"];
		
		return NO;
	}
	

	
	if ([self checkFlopVsDealMode])
		return NO;
	
	return YES;
	
}

- (BOOL)run {
	
	if (![self doPreRunChecks])
		return NO;
	
	[self checkEasterEgg];
	
	[lastSim reset];
		
	NSString* excludedCards = [self fullySpecifiedCardsShowing];
	int p;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if ([playerHands[p].currentTitle isEqualToString:@""])
			continue;
		
		//this is also done in preRunChecks... but no harm making sure
		NSLog([NSString stringWithFormat:@"Player %d hand: %@", p, playerHands[p].currentTitle]);
		if ([playerHands[p].currentTitle isEqualToString:@"x"]) {
			[playerHands[p] setTitle:@"xxxx" forState:UIControlStateNormal];
			[playerHands[p] setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
			
		}
		
		BOOL wasSuccess = NO;
		if (handCodeComboCounts[p]) {
			int hcWeights[PODDS_MAX_HANDS_PER_PLAYER];
			int hc;
			for (hc = 0; hc < [handCodeComboCounts[p] count]; hc++) {
				HandComboCount *hcc = [handCodeComboCounts[p] objectAtIndex:hc];
				hcWeights[hc] = hcc.userAdjustedCount;
			}
			
			NSString* ec = [NSString stringWithString:excludedCards];
			NSArray* hands = [playerHands[p].currentTitle componentsSeparatedByString:@","];
			for (NSString* hand in hands) {
				if (hand.length == 4 && isCard([hand substringToIndex:2]) && isCard([hand substringFromIndex:2])) {
					ec = [ec stringByReplacingOccurrencesOfString:hand withString:@""];
				}
			}
			
			wasSuccess = [lastSim addPlayer:playerHands[p].currentTitle at:p withWeights:hcWeights 
							  excludedCards:ec];
			
			assert_local([handCodeComboCounts[p] count] == [lastSim numHandCodes][lastSim.numPlayers - 1])
		}
		else {
			wasSuccess = [lastSim addPlayer:playerHands[p].currentTitle at:p];
		}
		
		if (!wasSuccess) {
			//report error
			NSString *errStr = [NSString stringWithCString:getError() encoding:NSASCIIStringEncoding];
			[SniperViewController displayAlert:errStr entitled:[NSString stringWithFormat:@"Error in hand %d", p + 1]];
			
			return NO;
		}
		

	}

	[lastSim setFlopString:flop.currentTitle];
	[lastSim setDeadString:deadCards.currentTitle];
	
	//run processing in worker thread
	[NSThread detachNewThreadSelector:@selector(runWorker) toTarget:self withObject:nil];
	
	//NSLog(@"Worker started");
	
	return YES;
}

- (void)checkEasterEgg {
	int p; //, validHandIx = 0;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if ([playerHands[p].currentTitle isEqualToString:@"J3s"]) {
		    [SniperViewController displayAlert:@"If you really played J3s, then not even PokerSniper can help you." 
									  entitled:@"Uh oh :)"];
			return;
		}
	}
}

- (BOOL)checkFlopVsDealMode {
	if ((flop.currentTitle.length / 2) + PODDS_NUM_DEALT_CARDS > lastSim.numCards) {
		[SniperViewController displayAlert:[NSString stringWithFormat:@"Too many flop cards specified for '%@'", flopRiverTurnButton.currentTitle] 
								  entitled:@"Error"];
		return YES;
	}
	
	return NO;
}

- (BOOL)checkHandCollision {
	
	//return NO;

	//check for >4 of the same rank	
	//check for exact duplicate cards
	
	//take the first specified hands from each player
	//if a range is specified take only the left end of the range
	//ignore if more than one hand combo exists just take the first combo
	//because this makes hand collisions far less likely (ie. they will only
	//exist if every one of the player hands specifies that same card,
	//which is far more likely to be done as a range)
	NSMutableSet *dealtCards = [[NSMutableSet alloc] initWithCapacity:PODDS_NUM_CARDS];
	
	//90 gives access to all nums and letters...
	int suitsPerRank[90] = { 0 };
	
	//hack: splitting hands on any punctuation characters (not just +-, etc)
	NSCharacterSet *splitters = [NSCharacterSet punctuationCharacterSet];
	int p = 0;
	BOOL isCollision = NO;
	while (!isCollision) {
		NSString *hand;
		//check the dealt hands and the flop
		if (p < MAX_PLAYERS_ON_SCREEN)
			hand = playerHands[p].currentTitle;
		else if (p == MAX_PLAYERS_ON_SCREEN)
			hand = flop.currentTitle;
		else if (p == MAX_PLAYERS_ON_SCREEN + 1)
			hand = deadCards.currentTitle;
		else
			break;
		p++;
		
		if ([hand isEqual:@""])
			continue;
			
		//just take the first "hand" (which could be anything really... KQ, KdKh, K7s etc etc
		//this works because if there are multiple hands to choose from then impossible collisions are much less likely
		NSString *firstHand = [[hand componentsSeparatedByCharactersInSet:splitters] objectAtIndex:0];

		//if the first two characters are a card, then add then to the set of cards, checking for collisions
		NSRange rng;
		rng.location = 0;
		rng.length = 2;
		while (!isCollision && (rng.location + rng.length) <= firstHand.length) {
			NSString* card = [firstHand substringWithRange:rng];
			if (isCard(card)) {
				if ([dealtCards containsObject:card]) {
					isCollision = YES;
					
					[SniperViewController displayAlert:[NSString stringWithFormat:@"'%@' occurs twice (or more!)", card]
											  entitled:@"Card overdealt!"];
					break; //out of while
				}
				else {
					[dealtCards addObject:card];
					
					//and add the rank to the suitsPerRank count
					unichar rank = [card characterAtIndex:0];	
					if (++suitsPerRank[(int)rank] > PODDS_NUM_SUITS) {
						isCollision = YES;
						[SniperViewController 
							displayAlert:[NSString stringWithFormat:@"'%c' occurs %d (or more) times", rank, suitsPerRank[(int)rank]]
							entitled:@"Card overdealt!"];
						break; //out of while
					}
				}
			}
			//if the first two characters are not a card, and they are legal, then they must be either two ranks
			//or have a random rank (or maybe suit for something like xhxh)
			else if (rng.location == 0) {
				int i;
				for (i = 0; i < 2; i++) {
					unichar rank = [card characterAtIndex:i];	
					//if 
					if (rank == RANDOM_ENTRY_CHAR)
						break;
					
					if (++suitsPerRank[(int)rank] > PODDS_NUM_SUITS) {
						isCollision = YES;
						[SniperViewController 
							displayAlert:[NSString stringWithFormat:@"'%c' occurs %d (or more) times", rank, suitsPerRank[(int)rank]]
							entitled:@"Card overdealt!"];
						break; //won't break out of while loop immediately, but will on next iter
					}
				}
				
			}
			
			rng.location += 2;
		}
	}

	
	[dealtCards release];
	
	return isCollision;
}

+ (void)displayAlert:(NSString *)msg entitled:(NSString *)title {
	UIAlertView *alert = [[UIAlertView alloc] 
						  initWithTitle:title
						  message:msg
						  delegate:nil 
						  cancelButtonTitle:@"OK" 
						  otherButtonTitles:nil];
	[alert show];
	[alert release];	
}

- (int)countPlayers {
	int p, count = 0;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if (![playerHands[p].currentTitle isEqualToString:@""])
			count++;
	}
	
	return count;
}

- (void)runWorker {
	//NSLog(@"runWorker started");
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	isWorking = YES;
	
//	clock_t start, end;
//	double elapsed;
//	start = clock();
	
	//NSLog(@"Beginning sim");
#ifdef	DUMP_RESULTS_FILE
	NSMutableData *resultsDump = [NSMutableData dataWithCapacity:1024];
	
#endif		
	
	
	int i;
	for (i = 0; i < lastSim.numSims && !isWorkerCancelled; i++) {
		if (i != 0 && (i % UPDATE_EQ_FREQ) == 0) {
			[self performSelectorOnMainThread:@selector(updateProgress)
								   withObject:nil
								waitUntilDone:YES];
		}

		int failed = 0;
		if (isComboCountsShowing) {
			failed = runSimulation_ncards_dead_weighted(lastSim.hands, lastSim.numPlayers, [lastSim numHands_weighted], 
											   lastSim.flop, lastSim.numFlopped, 
											   lastSim.dead, lastSim.numDead, 
											   lastSim.numCards, [lastSim numHandCodes], [lastSim handCodesWeights],
											   [lastSim results]);
		}
		else {
			failed = runSimulation_ncards_dead(lastSim.hands, lastSim.numPlayers, [lastSim numHands], 
								  lastSim.flop, lastSim.numFlopped, 
								  lastSim.dead, lastSim.numDead, 
								  lastSim.numCards, [lastSim results]);
		}
		
#ifdef	DUMP_RESULTS_FILE
		if (!failed) {
			int p;
			char cardBuf[PODDS_CHARS_PER_CARD];
			NSString *playerHandsStr = nil;
			NSString *playerHandnamesStr = nil;
			NSString *winner = @"Split";
			NSString *splits = nil;
			for (p = 0; p < lastSim.numPlayers; p++) {
				hand_to_str(&([lastSim results][p].card1), 1, cardBuf);
				NSString *hand = [NSString stringWithCString:cardBuf encoding:NSASCIIStringEncoding];
				hand_to_str(&([lastSim results][p].card2), 1, cardBuf);
				hand = [hand stringByAppendingString:[NSString stringWithCString:cardBuf encoding:NSASCIIStringEncoding]];
				
				if (!playerHandsStr)
					playerHandsStr = [NSString stringWithString:hand];
				else
					playerHandsStr = [playerHandsStr stringByAppendingString:[NSString stringWithFormat:@",%@", hand]];
				
				NSString *handName = [NSString stringWithCString:[lastSim results][p].handeval->desc encoding:NSASCIIStringEncoding];
				
				if (!playerHandnamesStr)
					playerHandnamesStr = [NSString stringWithString:handName];
				else
					playerHandnamesStr = [playerHandnamesStr stringByAppendingString:[NSString stringWithFormat:@",%@", handName]];
				
				if ([lastSim results][p].isWinner)
					winner = [NSString stringWithFormat:@"P%d", p];
				else if ([lastSim results][p].isSplltter) {
					if (!splits)
						splits = [NSString stringWithFormat:@"P%d", p];
					else
						splits = [splits stringByAppendingString:[NSString stringWithFormat:@",P%d", p]];
				}
			}
			
			char flopBuf[PODDS_NUM_FLOP_CARDS * PODDS_CHARS_PER_CARD];
			char deadBuf[PODDS_NUM_CARDS * PODDS_CHARS_PER_CARD];

			
			//just use the flop and dead cards provided in the first result... they are all the same
			hand_to_str([lastSim results][0].flopCards, PODDS_NUM_FLOP_CARDS, flopBuf);
			NSString *flopStr = [NSString stringWithCString:flopBuf encoding:NSASCIIStringEncoding];
			
			hand_to_str([lastSim results][0].deadCards, PODDS_NUM_FLOP_CARDS, deadBuf);
			NSString *deadStr = [NSString stringWithCString:deadBuf encoding:NSASCIIStringEncoding];
			
			NSString *result = [NSString stringWithFormat:@"%@,%@,%@,%@,%@,%@\r\n", playerHandsStr, flopStr, deadStr, playerHandnamesStr, winner, splits];
			const char *utfResult = [result UTF8String];
			[resultsDump appendBytes:utfResult length:strlen(utfResult)];
		}
#endif
	}
	
	//in practice the cancellation check always seems to occur while the screen updating is taking place
	//so we don't miss out on any simulations by not displaying the final state... and also means that
	//an even number of sims will be displayed, rather than one more
	
//	[self performSelectorOnMainThread:@selector(updateProgress)
//						   withObject:nil
//						waitUntilDone:YES];
	
//	end = clock();
//	elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	
#ifdef	DUMP_RESULTS_FILE
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    if (!documentsDirectory) {
        NSLog(@"Documents directory not found!");
    }
	else {
		NSString *appFile = [documentsDirectory stringByAppendingPathComponent:DUMP_RESULTS_FILENAME];
		[resultsDump writeToFile:appFile atomically:YES];
	}
	
#endif
	
	isWorking = NO;
	
    [pool release];
}

- (void)updateProgress {
	int p, activeIx = 0;
	for (p = 0; p < MAX_PLAYERS_ON_SCREEN; p++) {
		if (![playerHands[p].currentTitle isEqualToString:@""]) {
			
			//hand collisions may mean no rounds are played... hopefully stopped before this point
			if ([lastSim results][activeIx].rounds == 0) {
				continue;
			}
			
			double equity = ([lastSim results][activeIx].splitEquity + [lastSim results][activeIx].won) / [lastSim results][activeIx].rounds;
			//convert equity to pot odds
			double odds = (1 - equity) / equity;
			
			if (!isResultsDisplaySwitched) {
				[playerEqs[p] setText:[NSString stringWithFormat:@"%1.1f%%", (equity * 100)]];			
				//[playerOdds[p] setText:[NSString stringWithFormat:@"%1.1f\nto 1", odds]];
			}
			else {
				[playerEqs[p] setText:[NSString stringWithFormat:@"%1.1f : 1", odds]];			
				//[playerOdds[p] setText:[NSString stringWithFormat:@"%1.0f%%", (equity * 100)]];
			}
			
			[playerEqs[p] setTextColor:[UIColor blackColor]];
			//[playerOdds[p] setTextColor:[UIColor blackColor]];	
			
			int lost = [lastSim results][activeIx].rounds - ([lastSim results][activeIx].won + [lastSim results][activeIx].split);
			[playerWins[p] setText:[NSString stringWithFormat:@"W: %1.0f%%", 
									(float)[lastSim results][activeIx].won / [lastSim results][activeIx].rounds * 100]];
			[playerLosses[p] setText:[NSString stringWithFormat:@"L: %1.0f%%", 
									  (float)lost / [lastSim results][activeIx].rounds * 100]];
			[playerTies[p] setText:[NSString stringWithFormat:@"T: %1.0f%%", 
									(float)[lastSim results][activeIx].split / [lastSim results][activeIx].rounds * 100]];
			
			[playerWins[p] setTextColor:[UIColor blackColor]];
			[playerLosses[p] setTextColor:[UIColor blackColor]];
			[playerTies[p] setTextColor:[UIColor blackColor]];
			
			activeIx++;
		}
		else {
			if (!isResultsDisplaySwitched) {
				[playerEqs[p] setText:@"HE%"];
				//[playerOdds[p] setText:@"X.X\nto 1"];		
			}
			else {
				[playerEqs[p] setText:@"X.X : 1"];
				//[playerOdds[p] setText:@"HE%"];
			}
			
			[playerEqs[p] setTextColor:[UIColor grayColor]];
			//[playerOdds[p] setTextColor:[UIColor grayColor]];	
			
			
			[playerWins[p] setText:@""];//"W:"];
			[playerLosses[p] setText:@""];//"L:"];
			[playerTies[p] setText:@""];//T:"];
			
			[playerWins[p] setTextColor:[UIColor grayColor]];
			[playerLosses[p] setTextColor:[UIColor grayColor]];
			[playerTies[p] setTextColor:[UIColor grayColor]];
		}
	}
	
	[numIters setText:[NSString stringWithFormat:@"%d", [lastSim results][0].rounds]];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
	//save down the hands, flop and any result
	
	NSLog(@"Terminating...");
	
	NSString *archivePath = [self dataFilePath];
	NSString *archivePath_TMP = [archivePath stringByAppendingString:@".tmp"];
	BOOL wasSuccess = [NSKeyedArchiver archiveRootObject:lastSim toFile:archivePath_TMP];
	NSFileManager *dfltMgr = [NSFileManager defaultManager];
	if (wasSuccess) {
		//check that a file exists at the path first, because on the first run it won't, so no need to remove
		BOOL shouldMove = (![dfltMgr fileExistsAtPath:archivePath]) ||
						  ([dfltMgr fileExistsAtPath:archivePath] && [dfltMgr removeItemAtPath:archivePath error:NULL]);
		if (shouldMove) {
			[dfltMgr moveItemAtPath:archivePath_TMP
							 toPath:archivePath
							  error:NULL];
		}
	}
	
	NSLog([NSString stringWithFormat:@"Finished archiving... %d", wasSuccess]);

}


/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)viewDidUnload {
	NSLog(@"SniperVC viewDidUnload\n");
	
	NSLog([NSString stringWithFormat:@"lastSim: %@", lastSim]);
	
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
	
	//this is (also?) necessary because activeLabel is checked against nil in handLabelSelected upon viewDid(RE)Load
	self.activeLabel = nil;
	
	//need to un-inited the application so that the "initializing" label is removed on viewDid(RE)Load
	isInited = NO;
}

- (void)didReceiveMemoryWarning {
	NSLog(@"SniperVC received mem warning\n");
	
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)dealloc {
    [super dealloc];
}

@end
					
BOOL isCard(NSString* card) {
	BOOL isErr = (card.length != 2);
	int i;
	for (i = 0; (!isErr) && i < card.length; i++) {
		if (i % 2 == 0) {
			unichar number = [card characterAtIndex:i];
			isErr = (number != '2' &&
					 number != '3' &&
					 number != '4' &&
					 number != '5' &&
					 number != '6' &&
					 number != '7' &&
					 number != '8' &&
					 number != '9' &&
					 number != 'T' &&
					 number != 'J' &&
					 number != 'Q' &&
					 number != 'K' &&
					 number != 'A');
		}
		else {
			unichar suit = [card characterAtIndex:i];
			isErr = (suit != 'h' &&
					 suit != 'd' &&
					 suit != 'c' &&
					 suit != 's');
			
		}
	}
	
	return !isErr;
}
					
//BOOL isPercentHand(UIButton* btn) {
//	if (btn == nil)
//		return NO;
//	
//	NSUInteger len = [btn.currentTitle length];
//	return (len > 0 && [btn.titleLabel.text characterAtIndex:(len - 1)] == '%');	
//}
					

//int expandHands(NSString* handCode, char* handBuf) {
//	char inbuf[1024];
//	
//	[handCode getCString:inbuf maxLength:1024 encoding:NSASCIIStringEncoding];
//	int charCount = getPossibleHands_csvHandCode(inbuf, handBuf);
//	
//	return charCount;
//}
