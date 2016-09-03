// Delayed Saccade Task (Qt Demo)
// Demo Application for the Working Memory Toolkit
// Copyright (C) 2006, Joshua L. Phillips (1),
//                     Trenton Kriete (2) &
//                     David C. Noelle (3)
// (1) Department of Computer Science, Middle Tennessee State
//     University
//     Email: Joshua.Phillips@mtsu.edu
// (2) Chief Data Scientist, RxREVU
// (3) School of Social Sciences, Humanities, and Arts, University of
//     California, Merced

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// Standard Headers
#include <iostream>
#include <cstdlib>
#include <list>
#include <vector>
#include <ctime>

// Use the Working Memory Toolkit API...
#include "wmtk/WMtk.h"

// The header file for these functions.
#include "DelayedSaccadeTask.h"

using namespace std;

bool DSTDebug = false;

// The main function for the program. This is the function that actually
// performs the simulation.
int main(int argc, char *argv[]) {

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
	for (q = 0; q < window_size; q++)
		window[q] = 0;



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
	//time_t good_seed = 1098415842;	// After changes, this didn't work so
										// well.
	time_t good_seed = 1099072248;		// New seed ~= 300 episodes.
	// If you want an instance when it learns "fast" - may need to
	// be changed above for your system though! :)
	// random_seed = good_seed;
	srand(random_seed);
	srand48(random_seed);

	cerr << "Random Seed:  " << random_seed << "\n";

	// The state data structure for the task.
	state current_state;
    string state_string;

    // The reward for a given episode
    double reward = 0.0;

	//bool use_actor = false; // This part isn't implemented yet, so this flag
							// will just be ignored for now. However, it is
							// still required for the constructor.

	// The encoding of an OR of the various working memory chunks is often
	// useful for discovering useful combinations of chunks for the working
	// memory system to remember. Therefore, we select an OR_CODE to leverage
	// this gain. The definition of OR_CODE is in FeatureVector.h -- so see
	// this header for more options beyond what we used here.
	//OR_CODE or_code = NOISY_OR;

	// Instantiate the Working Memory System.
	WorkingMemory WM(lrate,
                     ngamma,
                     lambda,
                     exploration_percentage,
                     vector_size,
                     wm_size);

    if (DSTDebug) {
        cout << "Working Memory Instantiated\n";
    }

	// Use learning rate
	//WM.getCriticNetwork()->setLearningRate(lrate);

	/// Use lambda
	//WM.getCriticNetwork()->setLambda(lambda);

	// Use gamma
	//WM.getCriticNetwork()->setGamma(ngamma);

	// Use exploration percentage
	//WM.setExplorationPercentage(exploration_percentage);

	// A list of chunk items for consideration for the Working Memory System
	//list<Chunk> candidate_chunks;
    vector<string> candidate_chunks;

	// A single chunk
	//Chunk chunk;
    string chunk;

	// Save the network to a file before beginning the simulation.
	//WM.saveNetwork("./starting_network.dat");

	// Loop for the specified number of episodes
	for (int trial = 0; trial < number_of_trials; trial++) {

        if (DSTDebug) {
            cout << "Beginning of episode: " << trial << "\n";
        }

		// Generate the start of a trial
		state_string = generateTrial(current_state);

        if (DSTDebug) {
            cout << "Starting State: " << state_string << "\n";
        }

        /**
         *  Here is where we check our progress to see if we are
         *   performing the task correctly
         */

        if (current_state.time_step > current_state.cross_on_time &&
        current_state.time_step <= current_state.cross_off_time &&
        current_state.position != MIDDLE) {
            // Broke the fixation rule!
            current_state.trial_ok = false;
        }
        if (current_state.time_step == current_state.cross_off_time + 1 &&
        current_state.position != current_state.target_position) {
            // Broke the move rule!
            current_state.trial_ok = false;
        }
        if (current_state.time_step == current_state.cross_off_time + 1 &&
        current_state.position == current_state.target_position) {
            // Moved to correct location!
            current_state.arrival_time = current_state.time_step;
        }
        if (current_state.time_step > current_state.cross_off_time + 1) {
            // Broke the time-out rule!
            current_state.arrival_time = current_state.time_step;
            current_state.trial_ok = false;
        }

        if (current_state.arrival_time < 0) {
            // Reward: Provided -> 0
            reward = 0.0;
        }
        if (current_state.trial_ok) {
            // Reward: Provided -> 1
            current_state.success = true;
            reward = 1;//20.0;
        }

        WM.initializeEpisode(state_string);

        if (DSTDebug){
            cout << "Episode Initialized!\n";
        }

		// AFTER the state has been initialized to begin a new episode, then
		// we tell the working memory system to do so as well.
		//WM.newEpisode(true);

		// Loop until the episode is completed
		while (current_state.arrival_time < 0) {

			// Grab the current episode time from the WM clock.
			//current_state.time_step = WM.getEpisodeTime;

			// Change the state data structure to reflect the current state of
			// the environment.
			if (current_state.cross_on_time == current_state.time_step) {
				current_state.middle = CROSS;
			}
			if (current_state.cross_off_time == current_state.time_step) {
				current_state.middle = NOTHING;
			}
			if (current_state.target_on_time == current_state.time_step) {
				switch (current_state.target_position) {
					case UP:
						current_state.up = TARGET;
						break;
					case DOWN:
						current_state.down = TARGET;
						break;
					case LEFT:
						current_state.left = TARGET;
						break;
					case RIGHT:
						current_state.right = TARGET;
						break;
					case MIDDLE:
						current_state.middle = TARGET;
						break;
				}
			}
			if (current_state.target_off_time == current_state.time_step) {
				switch (current_state.target_position) {
					case UP:
						current_state.up = NOTHING;
						break;
					case DOWN:
						current_state.down = NOTHING;
						break;
					case LEFT:
						current_state.left = NOTHING;
						break;
					case RIGHT:
						current_state.right = NOTHING;
						break;
					case MIDDLE:
						current_state.middle = NOTHING;
						break;
				}
			}

			// Environment changed -- Analyze the environment for Chunks
			bool in_store = false;		// Used when deciding if a chunk is
										// already currently in working memory.
			bool c_on_screen = false;	// True if the cross is presently on
										// the screen.
			focus c_place = MIDDLE;		// The location of the cross.
			bool t_on_screen = false;	// True if the target is presently on
										// the screen.
			focus t_place = MIDDLE;		// The location of the target.

			bool looking_at_something = false;	// True if the agent is
												// currently looking at an
												// object that is present.

			bool looking_where_something_was = false;	// True if the agent is
														// looking at a location
														// that corresponds with
														// some chunk in working
														// memory.

			// Determine if there is a fixation cross on the screen.
			if (current_state.middle == CROSS) {
				c_on_screen = true;
				c_place = MIDDLE;
			}

			// Determine if there is a target on the screen.
			if (current_state.up == TARGET) {
				t_on_screen = true;
				t_place = UP;
			}
			else if (current_state.down == TARGET) {
				t_on_screen = true;
				t_place = DOWN;
			}
			else if (current_state.left == TARGET) {
				t_on_screen = true;
				t_place = LEFT;
			}
			else if (current_state.right == TARGET) {
				t_on_screen = true;
				t_place = RIGHT;
			}
			// NOTE: I have made a few assuptions about where the various
			// items can appear on the screen. These would need to be refined
			// in the more general case.

			// Determine if the agent is looking at something.
			switch (current_state.position) {
				case MIDDLE:
					if (current_state.middle != NOTHING)
						looking_at_something = true;
					break;
				case UP:
					if (current_state.up != NOTHING)
						looking_at_something = true;
					break;
				case DOWN:
					if (current_state.down != NOTHING)
						looking_at_something = true;
					break;
				case LEFT:
					if (current_state.left != NOTHING)
						looking_at_something = true;
					break;
				case RIGHT:
					if (current_state.right != NOTHING)
						looking_at_something = true;
					break;
			}

			// Go through the working memory store and determine if there is a
			// fixation chunk present (in_store) and also if the agent is
			// looking at a location relevant to memory
			// (looking_where_something_was).
			for (x = 0; x < WM.workingMemorySlots(); x++) {
				chunk = WM.queryWorkingMemory(x);
				if (chunk == "fixate") {
					in_store = true;
					break;
				}
				else {
					/*switch (((item_data*) chunk.getData())->position) {
						case MIDDLE:
							if (current_state.position == MIDDLE)
								looking_where_something_was = true;
							break;
						case UP:
							if (current_state.position == UP)
								looking_where_something_was = true;
							break;
						case DOWN:
							if (current_state.position == DOWN)
								looking_where_something_was = true;
							break;
						case LEFT:
							if (current_state.position == LEFT)
								looking_where_something_was = true;
							break;
						case RIGHT:
							if (current_state.position == RIGHT)
								looking_where_something_was = true;
							break;
					}*/
                    if ( (chunk == "cross" && current_state.position == MIDDLE) |
                         (chunk == "up" && current_state.position == UP) |
                         (chunk == "down" && current_state.position == DOWN) |
                         (chunk == "left" && current_state.position == LEFT) |
                         (chunk == "right" && current_state.position == RIGHT) ) {

                        looking_where_something_was = true;
                    }
				}
			}


			//////////////////////////
			// Build candidate chunks.
			//////////////////////////

			// If there was no fixation chunk in memory (in_store == false)
			// and yet the agent is looking at some relevant location, then
			// supply the working memory system with the option to fixate.
			if (!in_store && (looking_at_something ||
			looking_where_something_was)) {
				//chunk.setData(NULL);
				//chunk.setType("FIXATE");
                chunk = "fixate";

				candidate_chunks.push_back(chunk);
			}

			in_store = false;
			item_data* i1 = new item_data;
			// If the cross is present in the environment, then it could be
			// used to build a candidate chunk.
			if (c_on_screen) {
				in_store = false;
				// Check to see if there is already a chunk for the fixation
				// cross in working memory. We don't need to provide a
				// duplicate.
				for (x = 0; x < WM.workingMemorySlots(); x++) {
    				chunk = WM.queryWorkingMemory(x);
					//if (chunk.getType() == "ITEM" && ((item_data*)
					//chunk.getData())->thing == CROSS) {
                    if (chunk == "cross") {
						in_store = true;
						break;
					}
				}

				// If it wasn't in the store (and we already know it is on the
				// screen and the agent can see it) then build a candidate
				// chunk for it.
				if (!in_store) {
					//i1->thing = CROSS;
					//i1->position = MIDDLE;

					//chunk.setData(i1);
					//chunk.setType(string("ITEM"));
                    chunk = "cross";

					candidate_chunks.push_back(chunk);
					i1 = new item_data;
				}
			}

            if (DSTDebug) cout << "About to check for target!\n";

			// Check to see if there is already a chunk for the target
			// in working memory. We don't need to provide a duplicate.
			in_store = false;
			for (x = 0; x < WM.workingMemorySlots(); x++) {
				chunk = WM.queryWorkingMemory(x);
				//if (chunk.getType() == "ITEM" && ((item_data*)
				//chunk.getData())->thing == TARGET) {
                if (chunk == "up" |
                    chunk == "down" |
                    chunk == "left" |
                    chunk == "right" ) {

					in_store = true;
					break;
				}
			}

			// If it wasn't in the store (and we already know it is on the
			// screen and the agent can see it) then build a candidate
			// chunk for it.
			if (!in_store && t_on_screen) {
				//i1->thing = TARGET;
				//i1->position = t_place;

				//chunk.setData(i1);
				//chunk.setType(string("ITEM"));

                bool addTarget = true;

                switch (t_place) {
                case UP:
                    chunk = "up";
                case DOWN:
                    chunk = "down";
                case LEFT:
                    chunk = "left";
                case RIGHT:
                    chunk = "right";
                default:
                    addTarget = false;
                }

				if (addTarget) candidate_chunks.push_back(chunk);
			} else delete i1;


			// Process Working Memory with candidate chunks
			//WM.tickEpisodeClock(candidate_chunks);

            // NOTE: For the HWMtk update, here I am combining the candidate chunks
            // into a single string to pass to WM

            if (DSTDebug) {
                cout << "About to construct state string\n";
                cout << "Number of candidate chunks: " << candidate_chunks.size() << "\n";
            }

            if (candidate_chunks.size() > 0) {
                state_string = candidate_chunks[0];
                for (int i = 1; i < candidate_chunks.size(); i++) {
                    state_string += "+" + candidate_chunks[i];
                }
                if (DSTDebug) cout << state_string << "\n";
            } else {
                state_string = "I";
            }

            /**
             *  Here is where we check our progress to see if we are
             *   performing the task correctly
             */

            if (current_state.time_step > current_state.cross_on_time &&
         	current_state.time_step <= current_state.cross_off_time &&
         	current_state.position != MIDDLE) {
         		// Broke the fixation rule!
         		current_state.trial_ok = false;
         	}
         	if (current_state.time_step == current_state.cross_off_time + 1 &&
         	current_state.position != current_state.target_position) {
         		// Broke the move rule!
         		current_state.trial_ok = false;
         	}
         	if (current_state.time_step == current_state.cross_off_time + 1 &&
         	current_state.position == current_state.target_position) {
         		// Moved to correct location!
         		current_state.arrival_time = current_state.time_step;
         	}
         	if (current_state.time_step > current_state.cross_off_time + 1) {
         		// Broke the time-out rule!
         		current_state.arrival_time = current_state.time_step;
         		current_state.trial_ok = false;
         	}

         	if (current_state.arrival_time < 0) {
         		// Reward: Provided -> 0
         		reward = 0.0;
         	}
         	if (current_state.trial_ok) {
         		// Reward: Provided -> 1
         		current_state.success = true;
         		reward = 1;//20.0;
         	}

            WM.step(state_string);

			// NOTE: At this point the WM object has removed all chunks from
			// the candidate_chunks list.
            candidate_chunks.clear();

			//////////////////////////////////////////////////
			// Act on the environment based on working memory.
			//////////////////////////////////////////////////

			bool fixate = false;			// True if there is a fixation
											// chunk present in memory.
			bool t_in_memory = false;		// True if there is a target chunk
											// present in memory.
			bool c_in_memory = false;		// True if there is a cross chunk
											// present in memory.
			focus t_mem_pos = MIDDLE;		// The position of target
											// according to memory.
			focus c_mem_pos = MIDDLE;		// The position of the cross
											// according to memory.

			// A random position that will be used for automatic processing
			// and exploration.
			focus f_pos = (focus) (5.0*rand()/(RAND_MAX+1.0));

			// Redetermine if the agent is looking at a location where it
			// thinks something was. (Remember that the memory contents may
			// have changed.)
			looking_where_something_was = false;
			for (x = 0; x < WM.workingMemorySlots(); x++) {
				chunk = WM.queryWorkingMemory(x);
				if (chunk == "fixate") {
					fixate = true;	// Fixation if activated
				}
				//if (chunk.getType() == "ITEM") {
					//if (((item_data*) chunk.getData())->thing == TARGET) {
                    if (chunk == "up") {
						t_in_memory = true;			// We remember the target
						//t_mem_pos = ((item_data*)
						//chunk.getData())->position;
                        t_mem_pos = UP;
					} else if (chunk == "down") {
                        t_in_memory = true;
                        t_mem_pos = DOWN;
                    } else if (chunk == "left") {
                        t_in_memory = true;
                        t_mem_pos = LEFT;
                    } else if (chunk == "right") {
                        t_in_memory  = true;
                        t_mem_pos = RIGHT;
                    }

					//if (((item_data*) chunk.getData())->thing == CROSS) {
                    if (chunk == "cross") {
						c_in_memory = true;			// We remember the cross.
						//c_mem_pos = ((item_data*)
						//chunk.getData())->position;
                        c_mem_pos = MIDDLE;
					}

					// Check if we are looking there...
					/*switch (((item_data*) chunk.getData())->position) {
						case MIDDLE:
							if (current_state.position == MIDDLE)
								looking_where_something_was = true;
							break;
						case UP:
							if (current_state.position == UP)
								looking_where_something_was = true;
							break;
						case DOWN:
							if (current_state.position == DOWN)
								looking_where_something_was = true;
							break;
						case LEFT:
							if (current_state.position == LEFT)
								looking_where_something_was = true;
							break;
						case RIGHT:
							if (current_state.position == RIGHT)
								looking_where_something_was = true;
							break;
					}*/
                    if ( (chunk == "cross" && current_state.position == MIDDLE) |
                         (chunk == "up" && current_state.position == UP) |
                         (chunk == "down" && current_state.position == DOWN) |
                         (chunk == "left" && current_state.position == LEFT) |
                         (chunk == "right" && current_state.position == RIGHT) ) {

                        looking_where_something_was = true;
                    }
				//}
			}

			/////////////////////////////////////////////////////////
			// Select actions based on memory or default to automatic
			// processes.
			/////////////////////////////////////////////////////////

			// If we are trying to fixate and it makes sense then do it.
			// However, very rarely we fail to do this.
			if (fixate && (looking_at_something ||
			looking_where_something_was)) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = current_state.position;
			}
			// If we remember both and both are on the screen then we choose
			// equally between them and look at one.
			// However, very rarely we fail to do this.
			else if (c_in_memory && c_on_screen && t_in_memory && t_on_screen) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else {
					if ((int) (2.0*rand()/(RAND_MAX+1.0)) == 0)
						current_state.position = c_mem_pos;
					else
						current_state.position = t_mem_pos;
				}
			}
			// If we remember just the target and it is on the screen, we look
			// at it.
			// However, very rarely we fail to do this.
			else if (t_in_memory && t_on_screen) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = t_mem_pos;
			}
			// If we remember just the cross and it is on the screen, we look
			// at it.
			// However, very rarely we fail to do this.
			else if (c_in_memory && c_on_screen) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = c_mem_pos;
			}
			// If we remember both, but neither are on the screen, we look
			// where one was equally choosing between the two.
			// However, very rarely we fail to do this.
			else if (t_in_memory && c_in_memory) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else {
					if ((int) (2.0*rand()/(RAND_MAX+1.0)) == 0)
						current_state.position = c_mem_pos;
					else
						current_state.position = t_mem_pos;
				}
			}
			// If we only remember the target and its not on the screen, we
			// look where ti was.
			// However, very rarely we fail to do this.
			else if (t_in_memory) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = t_mem_pos;
			}
			// If we only remember the cross and its not on the screen, we
			// look where ti was.
			// However, very rarely we fail to do this.
			else if (c_in_memory) {
				if ((int) (1000.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = c_mem_pos;
			}
			// If both are one on the screen, but we don't remember them, then
			// we choose to look at either with equal probability.
			// However, occasionally we fail to do this.
			else if (c_on_screen && t_on_screen) {
				if ((int) (10.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else {
					if ((int) (2.0*rand()/(RAND_MAX+1.0)) == 0)
						current_state.position = c_mem_pos;
					else
						current_state.position = t_mem_pos;
				}
			}
			// If the target is on the screen (this shouldn't happen but I
			// kept this for completeness) by itself and we don't remember it,
			// then we look at it.
			// However, occasionally we fail to do this.
			else if (t_on_screen) {
				if ((int) (10.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = t_place;
			}
			// If the cross is on the screen by itself and we don't remember
			// it, then we look at it.
			// However, occasionally we fail to do this.
			else if (c_on_screen) {
				if ((int) (10.0*rand()/(RAND_MAX+1.0)) == 0)
					current_state.position = f_pos;
				else
					current_state.position = c_place;
			}
			// If nothing is in memory or on the screen, we just do whatever.
			else { // Automatic processing
				current_state.position = f_pos;
			}

			// Update display ...
			int target_pos_num = -1;
			if (t_on_screen) {
			  switch (t_place) {
			  case UP:
			    target_pos_num = 1;
			    break;
			  case DOWN:
			    target_pos_num = 2;
			    break;
			  case LEFT:
			    target_pos_num = 3;
			    break;
			  case RIGHT:
			    target_pos_num = 4;
			    break;
			  case MIDDLE:
			  default:
			    target_pos_num = -1;
			    break;
			  }
			}
			int gaze_pos_num = -1;
			switch (current_state.position) {
			case UP:
			  gaze_pos_num = 1;
			  break;
			case DOWN:
			  gaze_pos_num = 2;
			  break;
			case LEFT:
			  gaze_pos_num = 3;
			  break;
			case RIGHT:
			  gaze_pos_num = 4;
			  break;
			case MIDDLE:
			  gaze_pos_num = 0;
			  break;
			default:
			  gaze_pos_num = -1;
			  break;
			}



            // Update current time step
            current_state.time_step++;

		}  // while ...

        /**
         *  Here is where we check our progress to see if we are
         *   performing the task correctly
         */

        if (current_state.time_step > current_state.cross_on_time &&
        current_state.time_step <= current_state.cross_off_time &&
        current_state.position != MIDDLE) {
            // Broke the fixation rule!
            current_state.trial_ok = false;
        }
        if (current_state.time_step == current_state.cross_off_time + 1 &&
        current_state.position != current_state.target_position) {
            // Broke the move rule!
            current_state.trial_ok = false;
        }
        if (current_state.time_step == current_state.cross_off_time + 1 &&
        current_state.position == current_state.target_position) {
            // Moved to correct location!
            current_state.arrival_time = current_state.time_step;
        }
        if (current_state.time_step > current_state.cross_off_time + 1) {
            // Broke the time-out rule!
            current_state.arrival_time = current_state.time_step;
            current_state.trial_ok = false;
        }

        if (current_state.arrival_time < 0) {
            // Reward: Provided -> 0
            reward = 0.0;
        }
        if (current_state.trial_ok) {
            // Reward: Provided -> 1
            current_state.success = true;
            reward = 1;//20.0;
        }

        WM.absorbReward("I", reward);

		// Output the trial number
		cout << trial << " ";

		// Calculate the goodness so far
		if (current_state.success) {
			window[goodness_index++] = 1;
			for (q = 0; q < window_size; q++)
				goodness += (double) window[q];
			goodness /= (double) window_size;

			cout << "1 " << goodness << endl;
		}
		else {
			window[goodness_index++] = 0;
			for (q = 0; q < window_size; q++)
				goodness += (double) window[q];
			goodness /= (double) window_size;

			cout << "0 " << goodness << endl;
		}

		if (goodness_index == window_size)
			goodness_index = 0;

		// If we are performing as well as we want, then we're finished.
		if (goodness >= finished_percentage) {
			break;
		}

	}

	// Save the final network to a file.
	//WM.saveNetwork("./ending_network.dat");
	return 0;
}

// Clears the state information for a new trial (required by reward function)
string generateTrial(state& current_state) {

	// Clean the state
	current_state.middle = NOTHING;
	current_state.up = NOTHING;
	current_state.down = NOTHING;
	current_state.left = NOTHING;
	current_state.right = NOTHING;
	current_state.time_step = 0;

	// Set-up the sequence of events.
	// These can be different on each episode or the same for all episodes as
	// specified by the fixed_time flag.
	bool fixed_time = false;
	if (!fixed_time) {
		current_state.cross_on_time = 3+(int) (4.0*rand()/(RAND_MAX+1.0));
		current_state.cross_off_time = current_state.cross_on_time + 10 +
										(int) (4.0*rand()/(RAND_MAX+1.0));
		current_state.target_on_time = current_state.cross_on_time + 3 +
										(int) ((float) 5.0*rand()/(RAND_MAX+1.0));
		current_state.target_off_time = current_state.target_on_time + 3;
	}
	else {
		current_state.cross_on_time = 5;
		current_state.cross_off_time = 15;
		current_state.target_on_time = 9;
		current_state.target_off_time = 12;
	}

	// Determine a random position to set the target to.
	current_state.target_position = (focus) (4.0*rand()/(RAND_MAX+1.0));

	// Finish initialization
	current_state.arrival_time = -1;
	current_state.trial_ok = true;
	current_state.success = false;

	return "I";
}

/*
// Reward Function
double user_reward_function(WorkingMemory& wm) {
	state* current_state = (state*) wm.getStateDataStructure();

	if (current_state->time_step > current_state->cross_on_time &&
	current_state->time_step <= current_state->cross_off_time &&
	current_state->position != MIDDLE) {
		// Broke the fixation rule!
		current_state->trial_ok = false;
	}
	if (current_state->time_step == current_state->cross_off_time + 1 &&
	current_state->position != current_state->target_position) {
		// Broke the move rule!
		current_state->trial_ok = false;
	}
	if (current_state->time_step == current_state->cross_off_time + 1 &&
	current_state->position == current_state->target_position) {
		// Moved to correct location!
		current_state->arrival_time = current_state->time_step;
	}
	if (current_state->time_step > current_state->cross_off_time + 1) {
		// Broke the time-out rule!
		current_state->arrival_time = current_state->time_step;
		current_state->trial_ok = false;
	}

	if (current_state->arrival_time < 0) {
		// Reward: Provided -> 0
		return 0.0;
	}
	if (current_state->trial_ok) {
		// Reward: Provided -> 1
		current_state->success = true;
		return 20.0;
	}

	// Reward: Defaulted -> 0
	return 0.0;
}

// State Translation Function
void user_state_function(FeatureVector& fv, WorkingMemory& wm) {

	// Clear the vector
	fv.clearVector();

	// Get the state data structure
	state* current_state = (state*) wm.getStateDataStructure();

	// Test if there are objects at each of the five locations. (Some of these
	// combinations won't occur and we could make the vector representation
	// more efficient by leaving them out, but I include them here for
	// simplicity.)
	switch (current_state->middle) {
		case CROSS:
			fv.setValue(0,1.0);
			break;
		case TARGET:
			fv.setValue(5,1.0);
			break;
		case NOTHING:
			break;
	}
	switch (current_state->up) {
		case CROSS:
			fv.setValue(1,1.0);
			break;
		case TARGET:
			fv.setValue(6, 1.0);
			break;
		case NOTHING:
			break;
	}
	switch (current_state->down) {
		case CROSS:
			fv.setValue(2,1.0);
			break;
		case TARGET:
			fv.setValue(7, 1.0);
			break;
		case NOTHING:
			break;
	}
	switch (current_state->left) {
		case CROSS:
			fv.setValue(3,1.0);
			break;
		case TARGET:
			fv.setValue(8, 1.0);
			break;
		case NOTHING:
			break;
	}
	switch (current_state->right) {
		case CROSS:
			fv.setValue(4,1.0);
			break;
		case TARGET:
			fv.setValue(9, 1.0);
			break;
		case NOTHING:
			break;
	}
	switch (current_state->position) {
		case MIDDLE:
			fv.setValue(10, 1.0);
			break;
		case UP:
			fv.setValue(11, 1.0);
			break;
		case DOWN:
			fv.setValue(12, 1.0);
			break;
		case LEFT:
			fv.setValue(13, 1.0);
			break;
		case RIGHT:
			fv.setValue(14, 1.0);
			break;
	}

	return;
}

// Chunk Translation Function
void user_chunk_function(FeatureVector& fv, Chunk& chk, WorkingMemory& wm) {

	// Clear the vector
	fv.clearVector();

	// Simply determine the type of chunk an fill in the appropriate unit.
	if (chk.getType() == "FIXATE") {
		fv.setValue(0, 1.0);
	}
	else if (chk.getType() == "ITEM") {
		switch (((item_data*) chk.getData())->thing) {
			case CROSS:
				fv.setValue(1, 1.0);
				break;
			case TARGET:
				fv.setValue(2, 1.0);
				break;
			case NOTHING:
				break;
		}
	}
}

// Chunk Deletion Function
void user_delete_function(Chunk& chk) {

	// Determine the type of chunk and delete its data (if it has any.)
	if (chk.getType() == "FIXATE") {
		chk.setType("EMPTY");
	}
	else if (chk.getType() == "ITEM") {
		delete ((item_data*) chk.getData());
		chk.setType("EMPTY");
	}
	else {
		cerr << "**ERROR** Got an unidentifiable chunk here!!" << endl;
	}

	return;
}
*/
