//
//  sniperViewController.h
//  sniper
//
//  Created by Mark Blackwell on 28/03/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
//#import <AudioToolbox/AudioToolbox.h>

#import "podds.h"
//#import "Fraction.h"
#import "Simulation.h"
#import "FlipsideViewController.h"
#import "HandComboCount.h"
#import "SniperView.h"

#define UPDATE_EQ_FREQ						100

//NB: in userinp.h RANDOM_RANK is 'X' and RANDOM_SUIT is 'x', but this
//is only true after hands have been parsed... at entry it is just 'x'
#define RANDOM_ENTRY_CHAR					'x'
#define ARCHIVE_NAME						@"sniper.archive"
#define MAX_PLAYERS_ON_SCREEN				4
#define CLR_ALL_INTERVAL_SECS				2.0
#define PRESS_AND_HOLD_SECS					0.3

#define SNIPER_LITE_TRIALS					25
#define SNIPER_LITE_START_NAG_AT_TRIAL_NUM	15
#define SNIPER_LITE_TRIALS_BETWEEN_NAGS		1
#define SNIPER_PRIVATE_DATAFILE_NAME		@"sniper.priv"
#define SNIPER_HANDWEIGHTING_INFO_SHOWS     3

//#define DUMP_RESULTS_FILE
//#define DUMP_RESULTS_FILENAME @"resultsDump.csv"
#define USE_ARCHIVE YES
//#define USE_TRADITIONAL_LITE
#define kFullVersionUrl						@"http://phobos.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=337703748&mt=8"
											/*@"http://itunes.apple.com/au/app/pokersniper/id337703748?mt=8"*/



//typedef struct {
//	int numPlayers;
//	int numHands[PODDS_MAX_PLAYERS + 1];
//	int numFlopped;
//	int numSims;
//	
//	//must be at least (PODDS_MAX_PLAYERS + 1) * PODDS_MAX_HANDS_PER_PLAYER * PODDS_NUM_DEALT_CARDS * PODDS_CHARS_PER_CARD
//	char buf[1024 * 8];
//	char *hands;
//	char *flop;
//	
//	int percentDone;
//	
//	simulation_result results[PODDS_MAX_PLAYERS + 1];
//} simulation;

@interface SniperViewController : UIViewController <UIActionSheetDelegate, FlipsideViewControllerDelegate, SniperViewDelegate> {
	IBOutlet UIButton *playerOneHand;
	IBOutlet UIButton *playerTwoHand;
	IBOutlet UIButton *playerThreeHand;
	IBOutlet UIButton *playerFourHand;
		
	UIButton *playerHands[4];
	
	IBOutlet UIButton *flop;
	
	IBOutlet UIButton *deadCards;
	
	UIButton *activeLabel;
															
	IBOutlet UILabel *playerOnePercentInfo;
	IBOutlet UILabel *playerTwoPercentInfo;
	IBOutlet UILabel *playerThreePercentInfo;
	IBOutlet UILabel *playerFourPercentInfo;
	
	UILabel *playerPercentInfos[4];
	
	IBOutlet UILabel *playerOneEq;
	IBOutlet UILabel *playerTwoEq;
	IBOutlet UILabel *playerThreeEq;
	IBOutlet UILabel *playerFourEq;
	
	UILabel *playerEqs[4];
	
	//IBOutlet UILabel *playerOneOdds;
	//IBOutlet UILabel *playerTwoOdds;
	//IBOutlet UILabel *playerThreeOdds;
	//IBOutlet UILabel *playerFourOdds;	
	
	//UILabel *playerOdds[4];
	
	IBOutlet UILabel *playerOneWins;
	IBOutlet UILabel *playerTwoWins;
	IBOutlet UILabel *playerThreeWins;
	IBOutlet UILabel *playerFourWins;	
	
	UILabel *playerWins[4];
	
	IBOutlet UILabel *playerOneLosses;
	IBOutlet UILabel *playerTwoLosses;
	IBOutlet UILabel *playerThreeLosses;
	IBOutlet UILabel *playerFourLosses;	
	
	UILabel *playerLosses[4];
	
	IBOutlet UILabel *playerOneTies;
	IBOutlet UILabel *playerTwoTies;
	IBOutlet UILabel *playerThreeTies;
	IBOutlet UILabel *playerFourTies;	
	
	UILabel *playerTies[4];
	
	IBOutlet UILabel *numIters;
	
	IBOutlet UIButton *runButton;
	UIImage *stopSign;
															
	IBOutlet UIButton *boardDeadButton;
															
	IBOutlet UIButton *flopRiverTurnButton;
															
															
	IBOutlet UILabel *waitInitLabel;
	
	IBOutlet UILabel *altBoardDeadValue;
	
	Simulation *lastSim;
	
	BOOL isInited;
	
	BOOL isHandEvalInited;
	
	BOOL isWorking;
	
	BOOL isWorkerCancelled;	
	
	BOOL isResultsDisplaySwitched;
	
	BOOL isDeadCardsShowing;
	
	BOOL isComboCountsShowing;
	
	//NSTimer *clearAllTimer;
	
	BOOL doClearAll;
															
	FlipsideViewController *helpScreen;
																
	//BOOL clearPressedLast;
	
	NSDate *runDownLastPressedAt;
	
	NSMutableArray *playerOneComboCounts;
	NSMutableArray *playerTwoComboCounts;
	NSMutableArray *playerThreeComboCounts;
	NSMutableArray *playerFourComboCounts;
	
	NSMutableArray *handCodeComboCounts[4];

	HandComboCount *activeComboCount;
	
	CGPoint lastTouch;	
	
	NSTimer *arrowRepeatTimer;
	int arrowRepeatTimerIters;
}

@property (retain, nonatomic) Simulation *lastSim;

@property (retain, nonatomic) FlipsideViewController *helpScreen;

@property (retain, nonatomic) UIButton *activeLabel;

@property (retain, nonatomic) UIImage *stopSign;

@property (retain, nonatomic) NSDate *runDownLastPressedAt;

@property (retain, nonatomic) HandComboCount *activeComboCount;

- (IBAction)buttonPressed:(id) sender;

- (IBAction)percentPressed:(id) sender;

- (IBAction)heartsPressed:(id) sender;

- (IBAction)diamondsPressed:(id) sender;

- (IBAction)spadesPressed:(id) sender;

- (IBAction)clubsPressed:(id) sender;

- (void)appendActiveLabel:(NSString*)withCard;

- (IBAction)backspacePressed:(id) sender;

- (IBAction)clearPressed:(id) sender;

- (IBAction)clearDoublePressed:(id)sender;

- (IBAction)downArrowTouchedUp:(id) sender;

- (IBAction)downArrowTouchedDown:(id) sender;

- (IBAction)downArrowTouchedUpOutside:(id) sender;

- (void)doDownArrow;

- (IBAction)upArrowTouchedUp:(id) sender;

- (IBAction)upArrowTouchedDown:(id) sender;

- (IBAction)upArrowTouchedUpOutside:(id) sender;

- (void)doUpArrow;

- (void)adjustPercentHand:(UIButton *)playerHand by:(int)increment;

- (IBAction)cycleFlopRiverTurn:(id)sender;

- (IBAction)cycleBoardDeadCardsView:(id)sender;

- (IBAction)showHelp;

- (void)updateFlopRiverTurnButton;

- (void)updateBoardDeadUI;

//- (IBAction)clearPressedUpInside:(id) sender;
//
//- (IBAction)clearPressedUpOutside:(id) sender;
//
//- (void)cancelClearAllTimer;
//
//- (IBAction)clearPressedDown:(id) sender;
//
//- (void)clearAll:(NSTimer*)theTimer;

- (IBAction)runPressedDown:(id) sender;

- (UIView *)view:(SniperView *)view hitTest:(CGPoint)point
       withEvent:(UIEvent *)event hitView:(UIView *)hitView;

- (void)displayComboCounts;

- (NSString *)fullySpecifiedCardsShowing;

- (void)removeComboCounts;

- (IBAction)runPressed:(id) sender;

#ifdef USE_TRADITIONAL_LITE
- (void)showNagDialog;

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex;

#endif

- (IBAction)handLabelSelected:(id) sender;

- (IBAction)randomHandPressed:(id) sender;

- (IBAction)switchHandEquityPresentationPressed:(id) sender;

- (BOOL)validateActiveLabel;

- (BOOL)validateLabel:(UIButton *) label;

- (BOOL)isActiveLabelPercentHand;

- (BOOL)doPreRunChecks;

- (BOOL)run;

- (BOOL)checkHandCollision;

- (BOOL)checkFlopVsDealMode;

- (int)countPlayers;

- (void)initWorker;

- (void)enablePostInit;

- (void)runWorker;

- (void)updateProgress;

+ (void)displayAlert:(NSString *)msg entitled:(NSString *)title;

- (void)applicationWillTerminate:(NSNotification *)notification;

- (NSString *)dataFilePath;

- (NSString *)settingsFilePath;

- (void)checkEasterEgg;

- (IBAction)click:(id)sender;

//returns the number of chars written to the handBuf for the player, or -error code if invalid hand
//+ (int)expandHands:(NSString*)handCode intoBuf:(char*)handBuf;


//- (IBAction)handLabelDblClicked:(id) sender;


@end

BOOL isCard(NSString* card);

//BOOL isPercentHand(UIButton* btn);

//returns the number of chars written to the handBuf for the player, or -error code if invalid hand
//int expandHands(NSString* handCode, char* handBuf);






