// Jeff Walker
// My Yahtzee Game
// Feb 18th 2015
// ___________________________________________________________________

#include "stdafx.h"
#include <iostream>
#include <ctime>
#include "windows.h"
#include <algorithm>
#include <string>
#include <vector>
#undef max
using namespace std;

// Function Prototypes
int DiceRoll(int(&dice)[5], bool(&saveDice)[5], int sum);
int Menu();
int DiceSave(int dice[5], bool (&saveDice)[5], int sum, int scores[15], bool saveScores[14]);
bool Score(int dice[5], int sum, int (&scores)[15], bool (&saveScores)[14], int &yahtzeeBonus);
int Count(int dice[5], int value);
int CountMatch(int dice[5]);
int CountAscending(int dice[5]);
void ScoreBoard(int scores[15], bool saveScores[14]);
void Help();
bool CheckInt();

int _tmain(int argc, _TCHAR* argv[])
{
	// Variable initialization
	int menuChoice = 0;
	// Vector for storing high scores
	vector<pair<int, string>> playerRecords;

	while (menuChoice != 3)
	{
		// Our call to menu - which saves their choice into menuChoice.
		menuChoice = Menu();

		// Cancel the game since the player doesn't want to play.
		if (menuChoice == 3)
		{
			cout << "Thank you for playing Yahtzee!" << endl;
			system("pause");
			return 0;
		}

		// Variable Initialization
		// Storing my 5 dice
		int dice[5] = { 0 };
		// For checking if dice need to be saved
		bool saveDice[5] = { 0 };
		// Storing score values [0] = ones, twos, threes, fours, fives, sixes, 3 of a kind, 4 of a kind, Full house, Small Straight, Large Straight,
		// Yahtzee, Yahtzee Bonus, Chance, and Upper Bonus = 14.
		int scores[15] = { 0 };
		// Saving scores - Only need 14 as there is no player interaction with Upper Bonus.
		bool saveScores[14] = { 0 };
		// Loop variable
		bool finalScore = false;
		// Stores the number of yahtzeeBonus. Needed to bring it out of the score loop, otherwise, I would
		// have kept setting it to 0 everytime score looped through which would break this score option.
		int yahtzeeBonus = 0;
		// Our initial dice rolling loop. This will loop until 3 total rolls have happened.
		while (!finalScore)
		{
			// Use this as an exit function for the player to score early
			bool score = false;
			// Stores the sum of the dice
			int sum = 0;
			// This is depicting exit functionality out of while loops for # of rolls allowed
			int rolls = 0;
			// Initial loop to setup simulation of rolling
			while (rolls < 3)
			{
				// Call our dice roll, save the value into sum (For scoring later on down the road)
				sum = DiceRoll(dice, saveDice, sum);

				// Storing the players choice in the DiceSave function
				int diceChoice = 0;
				// This roll only allows the player to interact with their dice twice.
				while (rolls < 2)
				{
					// This call to DiceSave allows players to interact with the dice they wish to save
					// to simulate the full playing of Yahtzee
					diceChoice = DiceSave(dice, saveDice, sum, scores, saveScores);

					// Dice choice 7 is to roll again
					// Breaking out of the while loop to roll the dice again
					if (diceChoice == 7)
						break;

					// Dice choice 8 is to score
					// Set score to true so that we actually move into scoring.
					if (diceChoice == 8)
					{
						score = true;
						break;
					}
				}

				// Break out if the player wishes to score.
				// This is necessary to get out of the second while loop early
				if (score)
					break;
				// Our termination evaluation
				rolls++;
				// This is to prevent someone rolling so fast to get repeat score values.
				Sleep(1000);
			}

			// Call to score. Return false if the game isn't complete, otherwise true.
			finalScore = Score(dice, sum, scores, saveScores, yahtzeeBonus);
			
			// Reseting variables
			rolls = 0;
			score = false;
			for (int i = 0; i < 5; i++)
			{
				saveDice[i] = false;
			}

			// This is our end-game if statement.
			// Collecting score total, player name and storing into our vector.
			if (finalScore)
			{
				system("CLS");
				ScoreBoard(scores, saveScores);

				int scoreTotal = 0;
				for (int i = 0; i < 15; i++)
				{
					scoreTotal += scores[i];
				}

				string playerName;
				cout << "Enter your name for high scores!\n";
				cin >> playerName;

				playerRecords.push_back(make_pair(scoreTotal, playerName));
			}
		}

		// Displaying high scores, by sorting the playerRecords in reverse.
		// This will show high scores on top.
		cout << "High Scores:\n";
		vector<pair<int, string>>::const_iterator iter;
		sort(playerRecords.rbegin(), playerRecords.rend());

		int count = 1;
		for (iter = playerRecords.begin(); iter != playerRecords.end(); ++iter)
		{
			cout << count << ") " << iter->second << "-\t" << iter->first << ".\n";
		}

		system("pause");

	}


	system("pause");
	return 0;
}

// Displays our menu options.
int Menu()
{
	// Variable initialization
	int value;
	// Our check to make sure the value is what we expected
	bool checkInt = false;
	// Our while loop exit check.
	bool exit = false;
	// Displays our welcome menu with the options to play or exit.
	while (!exit)
	{
		cout << "Welcome to Yahtzee!" << endl;
		cout << "Please enter your choice from the selection below:" << endl;
		cout << "1) Start a game of Yahtzee!" << endl;
		cout << "2) Help" << endl;
		cout << "3) Exit Program." << endl;
		cout << ">: ";

		cin >> value;
		// I decided to capture the absolute value of the entry here. This is to prevent
		// the user from entering a negative number and causing there to be bugs in the system.
		value = abs(value);

		// Displays our help text and then continues to the
		// top of the while loop
		if (value == 2)
		{
			Help();
			continue;
		}

		// CheckInt verifies that we received an integer and not non-alphanumeric characters.
		exit = CheckInt();

		// Making sure checkInt is true and that value is 1 or 3.
		if (checkInt && (value == 1 || value == 3))
			exit = true;

	}

	system("CLS");
	return value;
}

// Our RNG for dice rolling.
int DiceRoll(int (&dice)[5], bool (&saveDice)[5], int sum)
{
	sum = 0;
	srand(static_cast<unsigned int>(time(0)));
	// Rolling dice and storing them into our array for dice.
	for (int i = 0; i < 5; i++)
	{
		if (!saveDice[i])
		{
			dice[i] = (rand() % 6 + 1);
		}
		sum += dice[i];
	}
	
	return sum;
}

// This is where the meat of the game lies. This allows players to save dice, score, or see the scoreboard.
int DiceSave(int dice[5], bool (&saveDice)[5], int sum, int scores[15], bool saveScores[14])
{
	// Variable initialization
	int diceChoice = 0;
	// This is our check to make sure the player puts in an int.
	bool checkValue = false;
	while (true)
	{
		// Display our menu options to the player.
		cout << "Please select which dice you wish to save or unsave:" << endl;
		if (saveDice[0])
			cout << "1) First die is SAVED: " << dice[0] << endl;
		else
			cout << "1) First die which rolled a: " << dice[0] << endl;
		if (saveDice[1])
			cout << "2) Second die is SAVED: " << dice[1] << endl;
		else
			cout << "2) Second die which rolled a: " << dice[1] << endl;
		if (saveDice[2])
			cout << "3) Third die is SAVED: " << dice[2] << endl;
		else
			cout << "3) Third die which rolled a: " << dice[2] << endl;
		if (saveDice[3])
			cout << "4) Fourth die is SAVED: " << dice[3] << endl;
		else
			cout << "4) Fourth die which rolled a: " << dice[3] << endl;
		if (saveDice[4])
			cout << "5) Fifth die is SAVED: " << dice[4] << endl;
		else
			cout << "5) Fifth die which rolled a: " << dice[4] << endl;
		if (saveDice[0] && saveDice[1] && saveDice[2] && saveDice[3] && saveDice[4])
			cout << "6) Unsave all dice." << endl;
		else
			cout << "6) Save all dice." << endl;
		cout << "7) Roll Again" << endl;
		cout << "8) Score" << endl;
		cout << "9) Scoreboard" << endl;
		cout << "The sum of your dice is: " << sum << endl;
		cout << ">: ";

		cin >> diceChoice;;
		diceChoice = abs(diceChoice);

		// Making sure we got a int
		checkValue = CheckInt();

		if (checkValue && (diceChoice > 0 && diceChoice < 10))
		{
			checkValue = false;
		}
		else
			continue;

		// Our decision tree is below. This section controls the saving of specific dice.
		if (diceChoice == 1)
			saveDice[0] = !saveDice[0];

		if (diceChoice == 2)
			saveDice[1] = !saveDice[1];

		if (diceChoice == 3)
			saveDice[2] = !saveDice[2];

		if (diceChoice == 4)
			saveDice[3] = !saveDice[3];

		if (diceChoice == 5)
			saveDice[4] = !saveDice[4];

		// Automatically save all or unsave all dice.
		if (diceChoice == 6)
		{
			if (saveDice[0] && saveDice[1] && saveDice[2] && saveDice[3] && saveDice[4])
			{
				for (int i = 0; i < 5; i++)
				{
					saveDice[i] = false;
				}
			}
			else
			{
				for (int i = 0; i < 5; i++)
				{
					saveDice[i] = true;
				}
			}

		}

		// Player either wants to roll again or score
		if (diceChoice == 7 || diceChoice == 8)
			return diceChoice;

		// Display scoreboard
		if (diceChoice == 9)
			ScoreBoard(scores, saveScores);
	}

}

// This is what runs the scoreboard
void ScoreBoard(int scores[15], bool saveScores[14])
{
	// Variable for showing the current score total.
	int runningScore = 0;
	for (int i = 0; i < 15; i++)
	{
		runningScore += scores[i];
	}

	// Saving our scoreboard text into the scoreBoard.
	// We use an if statement chain to display a scratch if necessary.
	// Otherwise, display the value of their score (0 or higher).
	string scoreBoard = "";
	scoreBoard += "Score Board:\n0) Mark a score for 0 points\n";
	
	if (saveScores[0] && scores[0] == 0)
		scoreBoard += "1) Ones:\tSCRA";
	else
		scoreBoard += "1) Ones:\t" + to_string(scores[0]);
	
	if (saveScores[6] && scores[6] == 0)
		scoreBoard += "\t\t7) 3 of a Kind: (Dice Total):\tSCRA\n";
	else
		scoreBoard += "\t\t7) 3 of a Kind: (Dice Total):\t" + to_string(scores[6]) + "\n";
	
	if (saveScores[1] && scores[1] == 0)
		scoreBoard += "2) Twos:\tSCRA";
	else
		scoreBoard += "2) Twos:\t" + to_string(scores[1]);
	
	if (saveScores[7] && scores[7] == 0)
		scoreBoard += "\t\t8) 4 of a Kind: (Dice Total):\tSCRA\n";
	else
		scoreBoard += "\t\t8) 4 of a Kind: (Dice Total):\t" + to_string(scores[7]) + "\n";
	
	if (saveScores[2] && scores[2] == 0)
		scoreBoard += "3) Threes:\tSCRA";
	else
		scoreBoard += "3) Threes:\t" + to_string(scores[2]);
	
	if (saveScores[8] && scores[8] == 0)
		scoreBoard += "\t\t9) Full House: (25P):\t\tSCRA\n";
	else
		scoreBoard += "\t\t9) Full House: (25P):\t\t" + to_string(scores[8]) + "\n";
	
	if (saveScores[3] && scores[3] == 0)
		scoreBoard += "4) Fours:\tSCRA";
	else
		scoreBoard += "4) Fours:\t" + to_string(scores[3]);
	
	if (saveScores[9] && scores[9] == 0)
		scoreBoard += "\t\t10) Small Straight: (30P):\tSCRA\n";
	else
		scoreBoard += "\t\t10) Small Straight: (30P):\t" + to_string(scores[9]) + "\n";
	
	if (saveScores[4] && scores[4] == 0)
		scoreBoard += "5) Fives:\tSCRA";
	else
		scoreBoard += "5) Fives:\t" + to_string(scores[4]);
	
	if (saveScores[10] && scores[10] == 0)
		scoreBoard += "\t\t11) Large Straight: (40P):\tSCRA\n";
	else
		scoreBoard += "\t\t11) Large Straight: (40P):\t" + to_string(scores[10]) + "\n";
	
	if (saveScores[5] && scores[5] == 0)
		scoreBoard += "6) Sixes:\tSCRA";
	else
		scoreBoard += "6) Sixes:\t" + to_string(scores[5]);
	
	if (saveScores[11] && scores[11] == 0)
		scoreBoard += "\t\t12) Yahtzee: (50P):\t\tSCRA\n";
	else
		scoreBoard += "\t\t12) Yahtzee: (50P):\t\t" + to_string(scores[11]) + "\n";
	scoreBoard += "Bonus: (35P):\t" + to_string(scores[14]) + "\t\t13) Yahtzee Bonus: (100P):\t" + to_string(scores[12]) + "\n";
	
	if (saveScores[13] && scores[13] == 0)
		scoreBoard += "\t\t\t\t14) Chance: (Dice Total):\tSCRA\n";
	else
		scoreBoard += "\t\t\t\t14) Chance: (Dice Total):\t" + to_string(scores[13]) + "\n";

	scoreBoard += "Running Score: " + to_string(runningScore) + "\n";

	cout << scoreBoard;
}

// Here is where the player decides its final score.
bool Score(int dice[5], int sum, int(&scores)[15], bool (&saveScores)[14], int &yahtzeeBonus)
{
	// Variable Initialization
	// Saving the players score choice
	int scoreChoice = 0;
	// This is for saving the players scratch choice
	int scratchChoice = 0;
	// This is the check to get out of the function
	bool done = false;
	// This is the check for making sure they have scored
	bool scored = false;
	// This is the check to make sure we got an int.
	bool checkValue = false;
	while (!done)
	{
		while (!scored)
		{
			ScoreBoard(scores, saveScores);
			cout << "Dice 1: " << dice[0] << "\nDice 2: " << dice[1] << "\nDice 3: " << dice[2] << "\nDice 4: " << dice[3] << "\nDice 5: " << dice[4] << "\n";
			cout << "Dice sum: " << sum << "\n";
			cout << "\nPlease select an option above to score or 0 to scratch: ";
			cin >> scoreChoice;
			// I decided to capture the absolute value of the entry here. This is to prevent
			// the user from entering a negative number and causing there to be bugs in the system.
			scoreChoice = abs(scoreChoice);

			checkValue = CheckInt();

			if (checkValue && (scoreChoice < 15 && scoreChoice >= 0))
			{
				checkValue = false;
				scored = true;
			}
			else
			{
				continue;
			}
		}
		
		// Count variables used in scoring
		int count = 0;
		int count2 = 0;
		// Check to get out of the while loop while scractching.
		bool scratched = false;
		// Using a switch statement to catch what the player wishes to score.
		switch (scoreChoice)
		{
			// Case 0 is for scratching.
		case 0:
			cout << "Select a score to scratch\n";
			cin >> scratchChoice;

			scratchChoice = abs(scratchChoice);
			while (!scratched)
			{
				// Verifying our int
				checkValue = CheckInt();

				// Making sure the player doesn't try to scratch the Yahtzee Bonus
				if (checkValue && scratchChoice == 13)
				{
					cout << "Yahtzee Bonus cannot be scratched.\n";
					break;
				}

				// Verifying our values are good otherwise start over.
				if (checkValue && ((scratchChoice < 13 && scratchChoice > 0) || scratchChoice == 14))
				{
					checkValue = false;
					scratched = true;
				}
				else
					break;
			}
			// Using a second switch statement to
			// verify we're scratching the correct scores.
			switch (scratchChoice)
			{
				// Scratch ones if not scratched or saved.
			case 1:
				if (saveScores[0])
				{
					if (scores[0] == 0)
						cout << "Ones have already been scratched!\n";
					else
						cout << "Ones have already been scored!\n";
					break;
				}
				else
				{
					scores[0] = 0;
					saveScores[0] = true;
					done = true;
					break;
				}
				// Scratch twos if not scratched or saved.
			case 2:
				if (saveScores[1])
				{
					if (scores[1] == 0)
						cout << "Twos have already been scratched!\n";
					else
						cout << "Twos have already been scored!\n";
					break;
				}
				else
				{
					scores[1] = 0;
					saveScores[1] = true;
					done = true;
					break;
				}
				// Scratch threes if not scratched or saved.
			case 3:
				if (saveScores[2])
				{
					if (scores[2] == 0)
						cout << "Threes have already been scractched!\n";
					else
						cout << "Threes have already been scored!\n";
					break;
				}
				else
				{
					scores[2] = 0;
					saveScores[2] = true;
					done = true;
					break;
				}
				// Scratch fours if not scratched or saved.
			case 4:
				if (saveScores[3])
				{
					if (scores[3] == 0)
						cout << "Fours have already been scratched!\n";
					else
						cout << "Fours have already been scored!\n";
					break;
				}
				else
				{
					scores[3] = 0;
					saveScores[3] = true;
					done = true;
					break;
				}
				// Scratch fives if not scratched or saved.
			case 5:
				if (saveScores[4])
				{
					if (scores[4] == 0)
						cout << "Fives have already been scratched!\n";
					else
						cout << "Fives have already been scored!\n";
					break;
				}
				else
				{
					scores[4] = 0;
					saveScores[4] = true;
					done = true;
					break;
				}
				// Scratch sixes if not scratched or saved.
			case 6:
				if (saveScores[5])
				{
					if (scores[5] == 0)
						cout << "Sixes have already been scratched!\n";
					else
						cout << "Sixes have already been scored!\n";
					break;
				}
				else
				{
					scores[5] = 0;
					saveScores[5] = true;
					done = true;
					break;						
				}
				// Scratch 3 of a kind if not scratched or saved.
			case 7:
				if (saveScores[6])
				{
					if (scores[6] == 0)
						cout << "3 of a Kind has already been scratched!\n";
					else
						cout << "3 of a Kind has already been scored!\n";
					break;
				}
				else
				{
					scores[6] = 0;
					saveScores[6] = true;
					done = true;
					break;
				}
				// Scratch 4 of a kind if not scratched or saved.
			case 8:
				if (saveScores[7])
				{
					if (scores[7] == 0)
						cout << "4 of a Kind has already been scratched!\n";
					else
						cout << "4 of a Kind has already been scored!\n";
					break;
				}
				else
				{
					scores[7] = 0;
					saveScores[7] = true;
					done = true;
					break;
				}
				// Scratch full house if not scratched or saved.
			case 9:
				if (saveScores[8])
				{
					if (scores[8] == 0)
						cout << "Full house has already been scratched!\n";
					else
						cout << "Full House has already been scored!\n";
					break;
				}
				else
				{
					scores[8] = 0;
					saveScores[8] = true;
					done = true;
					break;
				}
				// Scratch Small Straight if not scratched or saved.
			case 10:
				if (saveScores[9])
				{
					if (scores[9])
						cout << "Small Straight has already been scratched!\n";
					else
						cout << "Small Straight has already been scored!\n";
					break;
				}
				else
				{
					scores[9] = 0;
					saveScores[9] = true;
					done = true;
					break;
				}
				// Scratch Large Straight if not scratched or saved.
			case 11:
				if (saveScores[10])
				{
					if (scores[10] == 0)
						cout << "Large Straight has already been scratched!\n";
					else
						cout << "Large Straight has already been scored!\n";
					break;
				}
				else
				{
					scores[10] = 0;
					saveScores[10] = true;
					done = true;
					break;
				}
				// Scratch Yahtzee if not scratched or saved.
			case 12:
				if (saveScores[11])
				{
					if (scores[11] == 0)
						cout << "Yahtzee has already been scratched!\n";
					else
						cout << "Yahtzee has already been scored!\n";
					break;
				}
				else
				{
					scores[11] = 0;
					saveScores[11] = true;
					done = true;
					break;
				}
				// Scratch Chance if not scratched or saved.
			case 14:
				if (saveScores[13])
				{
					if (scores[13] == 0)
						cout << "Chance has already been scratched!\n";
					else
						cout << "Chance has already been scored!\n";
					break;
				}
				else
				{
					scores[13] = 0;
					saveScores[13] = true;
					done = true;
					break;
				}
			default:
				break;
			}
			break;
			// Score ones if not scratched or saved.
		case 1:
			if (saveScores[0])
			{
				cout << "Ones have already been scored!\n";
				break;
			}
			
			// Sending to count the number of dice that equal 1
			count = Count(dice, 1);
			if (count > 0)
			{
				scores[0] = count;
				saveScores[0] = true;
				done = true;
				break;
			}
			break;
			// Score twos if not scratched or saved.
		case 2:
			if (saveScores[1])
			{
				cout << "Twos have already been scored!\n";
				break;
			}

			// Sending to count the number of dice that equal 2
			count = Count(dice, 2);
			if (count > 0)
			{
				scores[1] = count * 2;
				saveScores[1] = true;
				done = true;
				break;
			}
			break;
			// Score threes if not scratched or saved.
		case 3:
			if (saveScores[2])
			{
				cout << "Threes have already been scored!\n";
				break;
			}
			// Sending to count the number of dice that equal 3
			count = Count(dice, 3);
			if (count > 0)
			{
				scores[2] = count * 3;
				saveScores[2] = true;
				done = true;
				break;
			}
			break;
			// Score fours if not scratched or saved.
		case 4:
			if (saveScores[3])
			{
				cout << "Fours have already been scored!\n";
				break;
			}
			// Sending to count the number of dice that equal 4
			count = Count(dice, 4);
			if (count > 0)
			{
				scores[3] = count * 4;
				saveScores[3] = true;
				done = true;
				break;
			}
			break;
			// Score fives if not scratched or saved.
		case 5:
			if (saveScores[4])
			{
				cout << "Fives have already been scored!\n";
				break;
			}
			// Sending to count the number of dice that equal 5
			count = Count(dice, 5);
			if (count > 0)
			{
				scores[4] = count * 5;
				saveScores[4] = true;
				done = true;
				break;
			}
			break;
			// Score sixes if not scratched or saved.
		case 6:
			if (saveScores[5])
			{
				cout << "Sixes have already been scored!\n";
				break;
			}
			// Sending to count the number of dice that equal 6
			count = Count(dice, 6);
			if (count > 0)
			{
				scores[5] = count * 6;
				saveScores[5] = true;
				done = true;
				break;
			}
			break;
			// Score 3 of a kind if not scratched or saved.
		case 7:
			if (saveScores[6])
			{
				cout << "3 of a Kind has already been scored!\n";
				break;
			}
			// Sorting the dice to put like numbers next to each other.
			sort(dice, dice + 5);
			// Counting how many matches we have.
			count = CountMatch(dice);
			// 2 matches means we have 3 of the same number.
			if (count >= 2)
			{
				scores[6] = sum;
				saveScores[6] = true;
				done = true;
				break;
			}
			// Score 4 of a kind if not scratched or saved.
		case 8:
			if (saveScores[7])
			{
				cout << "4 of a Kind has already been scored!\n";
				break;
			}
			// Sorting the dice to put like numbers next to each other.
			sort(dice, dice + 5);
			// Counting how many matches we have.
			count = CountMatch(dice);
			// 3 matches means we have 4 of the same number.
			if (count >= 3)
			{
				scores[7] = sum;
				saveScores[7] = true;
				done = true;
				break;
			}
			// Score full house if not scratched or saved.
		case 9:
			if (saveScores[8])
			{
				cout << "Full House has already been scored!\n";
				break;
			}
			// Sorting dice to get like numbers together.
			sort(dice, dice + 5);
			// This for loops runs until there is no longer a match.
			// This gives us how many matches were on the first number.
			for (int i = 0; i != 5;)
			{
				if (dice[i] == dice[i + 1])
				{
					count++;
					i++;
				}
				else
				{
					i = 5;
				}
					
			}

			// If two matches it means we need to start from index 3
			// To match the last two dice.
			if (count == 2)
			{
				for (int i = 3; i < 4; i++)
				{
					if (dice[i] == dice[i + 1])
					{
						count2++;
					}
				}
			}
			// If 1 match it means we need to start from index 2.
			// This lets us match the last 3 dice.
			else if (count == 1)
			{
				for (int i = 2; i < 4; i++)
				{
					if (dice[i] == dice[i + 1])
						count2++;
				}
			}
			else
			{
				break;
			}

			// Score 25 if accurate.
			if (count > 0 && count2 > 0)
			{
				scores[8] = 25;
				saveScores[8] = true;
				done = true;
				break;
			}
			else
				break;
			// Score small straight if not scratched or saved.
		case 10:
			if (saveScores[9])
			{
				cout << "Small Straight has already been scored!\n";
				break;
			}
			// Sort the dice to get sequential numbers
			sort(dice, dice + 5);
			// Call countascending to make sure that each dice is one higher than the next
			count = CountAscending(dice);
			// If 3 or more dice are this way, we can score!
			if (count >= 3)
			{
				scores[9] = 30;
				saveScores[9] = true;
				done = true;
				break;
			}
			break;
			// Score large straight if not scratched or saved.
		case 11:
			if (saveScores[10])
			{
				cout << "Large Straight has already been scored!\n";
				break;
			}
			sort(dice, dice + 5);
			// Checking for atleast 4 matches (1,2,3,4,5 or 2,3,4,5,6).
			count = CountAscending(dice);
			// Score!
			if (count == 4)
			{
				scores[10] = 40;
				saveScores[10] = true;
				done = true;
				break;
			}
			// Score Yahtzee if not scratched or saved.
		case 12:
			if (saveScores[11])
			{
				cout << "Yahtzee has already been scored! Please select Yahtzee Bonus!\n";
				break;
			}
			// Counting for 5 dice match
			count = CountMatch(dice);
			if (count == 4)
			{
				scores[11] = 50;
				saveScores[11] = true;
				done = true;
				break;
			}
			// Score Yahtzee bonus if not scratched or saved.
		case 13:
			// Making sure Yahtzee was scored and higher than 0.
			// Otherwise the player either scratched Yahtzee or hasn't scored it.
			if (saveScores[11] && scores[11] > 0)
			{
				// Checking for 5 matching dice.
				count = CountMatch(dice);
				if (count == 4)
				{
					// This is where yahtzeeBonus is used.
					// We increment it everytime we get one.
					// Use that number and multiply by 100 to
					// get the right score.
					yahtzeeBonus++;
					scores[12] = yahtzeeBonus * 100;
					done = true;
					break;
				}
			}
			else
			{
				cout << "You must score a Yahtzee first before scoring here!\n";
				break;
			}
			// Score chance if not scratched or saved.
		case 14:
			if (saveScores[13])
			{
				cout << "You already had your chance!\n";
				break;
			}
			scores[13] = sum;
			saveScores[13] = true;
			done = true;
			break;
		default:
			break;
		}

		// If player didn't score correctly tell them.
		if (!done)
		{
			cout << "Please select an option that you can actually score on!\n";
		}

		// Setting scored to false so that we can enter the loop again if the player
		// didn't score correctly.
		scored = false;

	}

	// Checking the total value of the upper scores
	int total = 0;
	for (int i = 0; i < 6; i++)
	{
		total += scores[i];
	}

	// If the upper scores are 63 or higher give the upper bonus 35 points.
	if (total >= 63)
	{
		scores[14] = 35;
	}

	// Checking how many scores have been scratched or saved (Not including upper bonus).
	int scoreCheck = 0;
	for (int i = 0; i < 14; i++)
	{
		if (saveScores[i])
		{
			scoreCheck++;
		}
	}

	system("CLS");
	ScoreBoard(scores, saveScores);
	cout << "\n";

	// If 13 values are scored then we are done. (Yahtzee bonus doesn't count).
	if (scoreCheck == 13)
	{
		return true;
	}

	return false;
}

// This is checking a match to a dice value.
int Count(int dice[5], int value)
{
	int count = 0;
	for (int i = 0; i < 5; i++)
	{
		if (dice[i] == value)
		{
			count++;
		}
	}

	return count;
}

// This is checking how many dice match each other (ex. Dice 1 = 1, dice 2 = 1 MATCH).
int CountMatch(int dice[5])
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (dice[i] == dice[i+1])
		{
			count++;
		}
	}
	return count;
}

// Checking if the following dice is one higher than the previous dice.
int CountAscending(int dice[5])
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (dice[i] + 1 == dice[i + 1])
		{
			count++;
		}
	}
	return count;
}

// Help menu to explain how to score values.
void Help()
{
	string help = "";
	help = "\nThe objective of Yahtzee is to roll 5 dice into specific kinds of combinations\n";
	help += "to score points. These points are then tallied up at the end of the game to\n";
	help += "get a final score. The game can be played by more than one player but this\n";
	help += "iteration only allows for a single player.\n\nHow to score in Yahtzee:\n";
	help += "Ones = Total number of 1s rolled\nTwos = Total number of 2s rolled\n";
	help += "Threes = Total number of 3s rolled\nFours = Total number of 4s rolled\n";
	help += "Fives = Total number of 5s rolled\nSixes = Total number of 6s rolled\n";
	help += "3 of a kind = At least 3 like dice\n4 of a kind = At least 4 like dice\n";
	help += "Small Straight = At least 4 sequential dice\nLarge Straight = 5 sequential dice\n";
	help += "Yahtzee = 5 like dice\nYahtzee Bonus = Same as Yahtzee\n";
	help += "Chance = Any dice combination\nUpper Bonus = Scored after scoring at least 63 points in ones-sixes\n";

	cout << help;

	system("pause");
	system("CLS");
}

// Checking our int to make sure its good.
bool CheckInt()
{
	// Checking to make sure the value is good (int) otherwise telling the user to try again.
	if (cin.good())
	{
		cin.clear();
		// We run another ignore here. This allows us to drop values still stuck in the buffer.
		// Specifically this allowed me to not pass along a 0 when someone entered a float value
		// like (10.0).
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return true;
	}
	else
	{
		// Here we're clearing the input stream
		cin.clear();
		// Here we're telling the stream what values we're ignoring. This is done to prevent users
		// from entering values outside of our numeric limits (Specifically stopping alpha characters).
		cin.ignore(numeric_limits<int>::max(), '\n');
		system("CLS");
		cout << "You entered an incorrect value, please try again!\n";
		return false;
	}
}