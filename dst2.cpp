#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>

#include "dst2.h"

using namespace std;

bool DSTDebug = false;

int main (int argc, char* argv[]) {

	int number_of_trials = 10000;	// Maximum number of trials before giving
									// up on the task.

    if (argc > 2 && argv[1] == (string) "debug") {
        DSTDebug = true;
        number_of_trials = atoi(argv[2]);
    } else if (argc > 1 && argv[1] == (string) "debug") {
        DSTDebug = true;
    } else if (argc > 1) {
        number_of_trials = atoi(argv[1]);
    }

    cout << "# trials: " << number_of_trials << "\n";
    cout << "Debug Mode: " << ((DSTDebug) ? "On\n" : "Off\n");

	double finished_percentage = 0.98;	// Not really a percentage, more like a
										// decimal. Once this percentage of the
										// last window_size episodes were
										// completed successfully, then we quit.

	// Variables to calculate how well the agent is doing by analyzing the
	// last window_size episodes.
	int window_size = 20;					// Number of episodes to process.
	double goodness = 0.0;					// Fraction of successful trials.
	int goodness_index = 0;					// Indexing variable.
	int window[window_size];				// Stores the results of the last
											// window_size episodes
	int q;									// Counter exclusively used to
											// perform "goodness" calculations.
	// Initialize the window.
	for (q = 0; q < window_size; q++) {
		window[q] = 0;
    }

    int x;	// Local counter

    int wm_size = 3;						// Working memory size.
    //int state_feature_vector_size = 15; 	// State vector size.
    //int chunk_feature_vector_size = 3;		// Chunk vector size.
    int vector_size = 64;                   // Size of HRRs

    double lrate = 0.01; 					// Learning rate of the network.
    double lambda = 0.7;					// Past responsibility parameter.
    double ngamma = 0.99;					// Reward discounting parameter.
    double exploration_percentage = 0.05;	// The percentage of the time that
                                            // the WM does something different
                                            // from what it thinks it should
                                            // (to encourage exploration.)

    // Initialize Random Number Generators
    time_t random_seed = time(NULL);

    cerr << "Random Seed:  " << random_seed << "\n";

    srand(random_seed);

    // The episode and state data structures for the task
    Episode currentEpisode;
    State currentState;

    // The reward for a given episode
    double reward = 0.0;

    // Instantiate the Working Memory System.
	WorkingMemory WM(lrate,
                     ngamma,
                     lambda,
                     exploration_percentage,
                     vector_size,
                     wm_size);

    // A list of chunk items for consideration for the Working Memory System
    vector<string> candidateChunks;
    // A single chunk
    string chunk;

    // Loop for the specified number of episodes
	for (int trial = 0; trial < number_of_trials; trial++) {

        if (DSTDebug) {
            cout << "Start of episode " << trial << "!\n";
        }

        // Initailize the episode
        generateTrial(currentEpisode, currentState);

        // Determine if there is anything to fixate upon
        checkFixation(WM, currentState);

        // Get the string representation of the initial state
        string stateString = constructStateString(WM, currentState);

        if (DSTDebug) {
            cout << "Initial state: " << stateString << "\n";
        }

        // Call hte erward function to determine how the agent is performing
        reward = rewardFunction(currentEpisode, currentState);

        WM.initializeEpisode(stateString, reward);

        // Main loop of the episode
        while (currentEpisode.arrivalTime < 0) {
            // Update the current state of the environment
            updateState(currentEpisode, currentState);

            // Determine if there is anything to fixate upon
            checkFixation(WM, currentState);

            // Get the string representation of the current state
            stateString = constructStateString(WM, currentState);

            if (DSTDebug) {
                cout << "State: " << stateString << "\n";
            }

            // Call the reward function to determine how the agent is performing
            reward = rewardFunction(currentEpisode, currentState);

            // Provide the current state to Working Memory
            WM.step(stateString, reward);

            if (DSTDebug) {
                for (int i = 0; i < WM.workingMemorySlots() - 1; i++) {
                    cout << WM.queryWorkingMemory(i) << " | ";
                }
                cout << WM.queryWorkingMemory(WM.workingMemorySlots() - 1) << "\n";
            }

            //////////////////////////////////////////////////
			// Act on the environment based on working memory.
			//////////////////////////////////////////////////

            chooseAndPerformAction(WM, currentState);

            currentState.timeStep++;
        }

        // Call the reward function to determine how the agent is performing
        reward = rewardFunction(currentEpisode, currentState);

        // Absorb the reward from the episode
        stateString = constructStateString(WM, currentState);
        WM.absorbReward(stateString, reward);

		// Output the trial number
		cout << trial << " ";

        // Calculate the goodness so far
        if (currentEpisode.success) {
            window[goodness_index++] = 1;
            for (q = 0; q < window_size; q++) {
                goodness += (double) window[q];
            }
            goodness /= (double) window_size;

            cout << "1 " << goodness << "\n";
        } else {
            window[goodness_index++] = 0;
            for (q = 0; q < window_size; q++) {
                goodness += (double) window[q];
            }
            goodness /= (double) window_size;

            cout << "0 " << goodness << "\n";
        }

        if (goodness_index == window_size) {
            goodness_index = 0;
        }

        if (goodness >= finished_percentage) {
            break;
        }
    }

    return 0;
}

void generateTrial(Episode& currentEpisode, State& currentState) {

    // Set up the sequence of events.
    currentEpisode.crossOnTime = 5;
    currentEpisode.crossOffTime = 15;
    currentEpisode.targetOnTime = 9;
    currentEpisode.targetOffTime = 12;

    // Generate the location the target will appear in.
    currentEpisode.targetPosition = (Location) (rand()%4);
    if (DSTDebug) {
        cout << "Target position set to " << currentEpisode.targetPosition << "\n";
    }

    // Finish initializing episode
    currentEpisode.arrivalTime = -1;
    currentEpisode.trialOK = true;
    currentEpisode.success = false;

    // Set up the initial state for this episode
    currentState.targetLocation = Nowhere;
    currentState.crossLocation = Nowhere;
    currentState.gaze = Center;
    currentState.fixating = false;
    currentState.timeStep = 0;
}

// Change the state data t orefelc tthe current staet fo the environment
void updateState(Episode& episode ,State& state) {

    if (episode.crossOnTime == state.timeStep) {
        state.crossLocation = Center;
        if (DSTDebug) {
            cout << "\tCross on!\n";
        }
    }
    if (episode.crossOffTime == state.timeStep) {
        state.crossLocation = Nowhere;
        if (DSTDebug) {
            cout << "\tCross off!\n";
        }
    }
    if (episode.targetOnTime == state.timeStep) {
        state.targetLocation = episode.targetPosition;
        if (DSTDebug) {
            cout << "\tTarget on!\n";
        }
    }
    if (episode.targetOffTime == state.timeStep) {
        state.targetLocation = Nowhere;
        if (DSTDebug) {
            cout << "\tTarget off!\n";
        }
    }
}


string constructStateString(WorkingMemory& WM, State& state) {
    bool inStore = false;
    vector<string> stateConcepts;

    // Add target location to state concept list,
    // if target is present and is not already in memory
    string targetString = "EMPTY";
    switch (state.targetLocation) {
    case Up:
        targetString = "up";
        break;
    case Down:
        targetString = "down";
        break;
    case Left:
        targetString = "left";
        break;
    case Right:
        targetString = "right";
        break;
    default:
        break;
    }

    if (targetString != "EMPTY") {
        // Check to see if there is already a chunk for the target
        // in working memory. We don't need to provide a duplicate
        inStore = false;
        for (int i = 0; i < WM.workingMemorySlots(); i++) {
            string chunk = WM.queryWorkingMemory(i);
            if (chunk == "up" ||
                chunk == "down" ||
                chunk == "left" ||
                chunk == "right" ) {

                inStore = true;
                break;
            }
        }

        // If it wasn't in working memory, then add cross to the
        // state concept list
        if (!inStore) {
            stateConcepts.push_back("target");
        }
    }

    // Add cross to state concept if it is present
    // In the current test, cross can only be in center
    if (state.crossLocation == Center) {
        // Check to see if there is already a chunk for the cross
        // in working memory. We don't need to provide a duplicate
        inStore = false;
        for (int i = 0; i < WM.workingMemorySlots(); i++) {
            string chunk = WM.queryWorkingMemory(i);
            if (chunk == "cross") {
                inStore = true;
                break;
            }
        }

        // If it wasn't in working memory, then add cross to the
        // state concept list
        if (!inStore) {
            stateConcepts.push_back("cross");
        }
    }

    // If agent is fixating, then add fixate to
    // state concept list
    if (state.fixating) {
        stateConcepts.push_back("fixate");
    }

    if (stateConcepts.size() > 0) {
        string stateString = stateConcepts[0];
        for (int i = 1; i < stateConcepts.size(); i++){
            stateString += ("+" + stateConcepts[i]);
        }

        return stateString;
    } else {
        return "I";
    }
}

void checkFixation(WorkingMemory& WM, State& state) {

    bool inStore = false;       // Used when deciding if a chunk is
                                // already currently in working memory.

    bool lookingAtSomething = false;    // True if the agent is
                                        // currently looking at an
                                        // object that is present.

    bool lookingWhereSomethingWas =false;   // True if the agent is
                                            // looking at a location
                                            // that corresponds with
                                            // some chunk in working
                                            // memory.

    // Determine if the agent is looking at something
    if (state.gaze != Nowhere && (state.gaze == state.targetLocation || state.gaze == state.crossLocation) ) {
        lookingAtSomething = true;
    }

    // Go through the working memory store and determine if there is a
    // fixation chunk present and also if the agent is
    // looking at a location relevant to memory
    for (int i = 0; i < WM.workingMemorySlots(); i++) {
        string chunk = WM.queryWorkingMemory(i);
        if (chunk == "fixate") {
            inStore = true;
            break;
        } else if ( (chunk == "cross" && state.gaze == Center) ||
                    (chunk == "up" && state.gaze == Up) ||
                    (chunk == "down" && state.gaze == Down) ||
                    (chunk == "left" && state.gaze == Left) ||
                    (chunk == "right" && state.gaze == Right) ) {

            lookingWhereSomethingWas = true;
        }
    }

    // If there was no fixation chunk in memory
    // and yet the agent is looking at some relevant location, then
    // supply the working memory system with the option to fixate.
    if (!inStore && (lookingAtSomething || lookingWhereSomethingWas)) {
        state.fixating = true;
    } else {
        state.fixating = false;
    }
}

void chooseAndPerformAction(WorkingMemory& WM, State& state) {
    // A random position that will be used for automatic processing
    // and exploration
    Location randomGaze = (Location) (rand()%5);

    bool inStore = false;       // Used when deciding if a chunk is
                                // already currently in working memory.

    bool lookingAtSomething = false;    // True if the agent is
                                        // currently looking at an
                                        // object that is present.

    bool lookingWhereSomethingWas =false;   // True if the agent is
                                            // looking at a location
                                            // that corresponds with
                                            // some chunk in working
                                            // memory.

    Location rememberedCross = Nowhere;     // Whether or not the
                                            // cross is in memory,
                                            // and in what location.

    Location rememberedTarget = Nowhere;    // Whether or not the
                                            // target is in memory,
                                            // and in what location.

    // Determine if the agent is looking at something
    if (state.gaze == state.targetLocation || state.gaze == state.crossLocation) {
        lookingAtSomething = true;
    }

    // Go through the working memory store and determine if there is a
    // fixation chunk present and also if the agent is
    // looking at a location relevant to memory
    for (int i = 0; i < WM.workingMemorySlots(); i++) {
        string chunk = WM.queryWorkingMemory(i);
        if (chunk == "fixate") {
            state.fixating = true;
            break;
        } else if (chunk == "cross") {
            rememberedCross = Center;
            if (state.gaze == Center) {
                lookingWhereSomethingWas = true;
            }
        } else if (chunk == "up") {
            rememberedTarget = Up;
            if (state.gaze == Up) {
                lookingWhereSomethingWas = true;
            }
        } else if (chunk == "down") {
            rememberedTarget = Down;
            if (state.gaze == Down) {
                lookingWhereSomethingWas = true;
            }
        } else if (chunk == "left") {
            rememberedTarget = Left;
            if (state.gaze == Left) {
                lookingWhereSomethingWas = true;
            }
        } else if (chunk == "right") {
            rememberedTarget = Right;
            if (state.gaze == Right) {
                lookingWhereSomethingWas = true;
            }
        }
    }

    /////////////////////////////////////////////////////////
    // Select actions based on memory or default to automatic
    // processes.
    /////////////////////////////////////////////////////////

    // If we are trying to fixate and it makes sense then do it.
    // However, very rarely we fail to do this.
    if ( state.fixating && ( lookingAtSomething || lookingWhereSomethingWas ) ) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = state.gaze;
        }
    }
    // If we remember both and both are on the screen thne we choose
    // equally between them nad look at one.
    // However, very rarely we fail to do this.
    else if (rememberedCross != Nowhere && state.crossLocation != Nowhere &&
             rememberedTarget != Nowhere && state.targetLocation != Nowhere) {

        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            if (rand()%2 == 0) {
                state.gaze = rememberedCross;
            } else {
                state.gaze = rememberedTarget;
            }
        }
    }
    // If we remember just the target and it is on the screen, we look
    // at it.
    // However, very rarely we fail to do this
    else if (rememberedTarget != Nowhere && state.targetLocation != Nowhere) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = rememberedTarget;
        }
    }
    // If we remember just the cross and it is on the screen, we look
    // at it.
    // However, very rarely we fail to do this
    else if (rememberedCross != Nowhere && rememberedTarget != Nowhere) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = rememberedCross;
        }
    }
    // If we remember both, but neither are on the screen, we look
    // where one was, equally choosing between the two.
    // However, very rarely we fail to do this.
    else if (rememberedTarget != Nowhere && rememberedCross != Nowhere) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            if (rand()%2 == 0) {
                state.gaze = rememberedCross;
            } else {
                state.gaze = rememberedTarget;
            }
        }
    }
    // If we only remember the targte and it is not on the screen, we
    // look where it was.
    // However, very rarely we fail to do this.
    else if (rememberedTarget != Nowhere) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = rememberedTarget;
        }
    }
    // If we only remember the cross and it is not on the screen, we
    // look where it was.
    // However, very rarely we fail to do this.
    else if (rememberedCross != Nowhere) {
        if (rand()%1000 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = rememberedCross;
        }
    }
    // If both are on the screen, but we don't remember them, then
    // we choose to look at either with equal probability.
    // However, occasionally we fail to do this.
    else if (state.crossLocation != Nowhere && state.targetLocation != Nowhere) {
        if (rand()%10 == 0) {
            state.gaze = randomGaze;
        } else {
            if (rand()%2 == 0) {
                state.gaze = state.crossLocation;
            } else {
                state.gaze = state.targetLocation;
            }
        }
    }
    // If the target is on the screen (this shouldn't happen but I
    // kept this for completeness) by itself and we don't remember ti,
    // then we look at it.
    // However, occasionally we fail to do this.
    else if (state.targetLocation != Nowhere) {
        if (rand()%10 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = state.targetLocation;
        }
    }
    // If the cross is on the screen by itself and we don't remember
    // it, then we look at it.
    // However, occasionally we fail to do this.
    else if (state.crossLocation != Nowhere) {
        if (rand()%10 == 0) {
            state.gaze = randomGaze;
        } else {
            state.gaze = state.crossLocation;
        }
    }
    // If nothing is in memory or on the screen, we just do whatever.
    else {
        state.gaze = randomGaze;
    }
}

double rewardFunction(Episode& episode, State& state) {
    if (state.timeStep > episode.crossOnTime &&
        state.timeStep <= episode.crossOffTime &&
        state.gaze != Center ) {

        // Broke the fixation rule!
        episode.trialOK = false;
    }
    if (state.timeStep == episode.crossOffTime + 1 &&
        state.gaze != episode.targetPosition ) {

        // Broke the move rule!
        episode.trialOK = false;
    }
    if (state.timeStep == episode.crossOffTime + 1 &&
        state.gaze == episode.targetPosition ) {

        // Moved to correct location
        episode.arrivalTime = state.timeStep;
    }
    if (state.timeStep > episode.crossOffTime + 1) {

        // Broke the time-out rule!
        episode.arrivalTime = state.timeStep;
        episode.trialOK = false;
    }

    if (episode.arrivalTime < 0) {
        // Reward: Provided -> 0
        return 0.0;
    }
    if (episode.trialOK) {
        // Reward: Provided -> 1
        episode.success = true;
        return 20.0;
    }
}
