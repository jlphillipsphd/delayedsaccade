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

//
// DelayedSaccadeTask.h
//
// Provides Josh Phillips's simulation code, for use in the graphical
// visualization code.
//
// Original code by Joshua Phillips (Tue Oct 19 18:12:27 CDT 2004)
//

#ifndef DST_H
#define DST_H

#include "delayedsaccade.h"
#include <WMtk.h>

// Items in the world - these are the possible things that can be in view.
enum item {
	NOTHING,		// Essentially what it says.
	CROSS,			// A fixation cross.
	TARGET			// A target.
};

// Positions that we can focus upon or also where items could be, note that
// the GUI does not use up, down, left, and right. However, it uses four
// locations that map directly onto this structure. Therefore, I will use the
// five positions listed here in the code, though they will be rotated in some
// arbitrary way on the screen when using the GUI.
enum focus {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	MIDDLE
};

// Something to remember about the world -- This is the data that will be
// stored in a Chunk that pertains to remembering the precence and location of
// an object in the world.
struct item_data {
	item thing;			// The item's type
	focus position;		// The position of the item.
};

// The current state of the world -- items and focus position
struct state {
	item middle;		// What's in the middle.
	item up;			// What's in the up position.
	item down;			// What's in the down position.
	item left;			// What's in the left position.
	item right;			// What's in the right position.
	focus position;		// Where the agent is currently looking.

	// State information for past experiences -- Not provided to the Working
	// Memory System, but needed for the reward function.
	int time_step;			// The current time into the trial.
	int cross_on_time;		// The time that the cross should appear.
	int cross_off_time;		// The time that the cross should disappear.
	int target_on_time;		// The time that the target should appear.
	int target_off_time;	// The time that the target should disappear.
	focus target_position;	// The position where the target should appear.

	bool trial_ok;			// This is a flag that tells whether the agent has
							// broken any rules so far during this episode.
							// This means it should be false at the beginning
							// of an episode and become false once the agent
							// has broken a rule.

	int arrival_time;		// The time at which the agent arrived at the
							// location of the target. This is initially set
							// to -1 to indicate that the agent has not looked
							// at the location where the target was. Once the
							// agent does this, we record the timestep of the
							// action here. It only records the FIRST time the
							// agent looks where the target was and only AFTER
							// the target has appeared.

	bool success;			// This flag indicates whether or not the agent
							// successfully completed the task this episode.
							// It is set false initially. Then we simply check
							// if the arrival_time value is equal to one time
							// step after the cross_off_time (right after the
							// cross disappears) and the trial_ok flag is
							// still true. This means that the agent was
							// successful and we set this flag to true.
};

// Prototype for function that will initialize a state structure for the
// start of an new episode.
void generateTrial(state& current_state);

// User-defined function prototypes
double user_reward_function(WorkingMemory& wm);
void user_state_function(FeatureVector& fv, WorkingMemory& wm);
void user_chunk_function(FeatureVector& fv, Chunk& chk, WorkingMemory& wm);
void user_delete_function(Chunk& chk);

// The main function for the program. This is the function that actually
// performs the simulation.
void RunSimulation(SaccadeWindow& win);

#endif  // ifndef DST_H
