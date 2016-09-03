#ifndef DST2_H
#define DST2_H

#include "wmtk/WMtk.h"

enum Location {
    Up,
    Down,
    Left,
    Right,
    Center,
    Nowhere
};

struct Episode {
    int crossOnTime;        // The time that the cross should appear.
    int crossOffTime;       // The time that the cross should disappear.
    int targetOnTime;       // The time that the target should appear.
    int targetOffTime;      // The time that the target should disappear.

    Location targetPosition;    // The location that the target will appear in.

    bool trialOK;			// This is a flag that tells whether the agent has
							// broken any rules so far during this episode.
							// This means it should be false at the beginning
							// of an episode and become false once the agent
							// has broken a rule.

    int arrivalTime;		// The time at which the agent arrived at the
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

struct State {
    // Environment info
    Location targetLocation;        // Where is the target located?
    Location crossLocation;         // Where is the cross located?
    Location gaze;                  // Where is the agent looking?
    bool fixating;                  // Is the agent fixated on what it is looking at?

    // State information for past experiences. Needed to determine the agent's performance
    int timeStep;
};

void generateTrial(Episode& currentEpisode, State& currentState);
void updateState(Episode& episode ,State& state);
string constructStateString(WorkingMemory& WM, State& state);
void checkFixation(WorkingMemory& WM, State& state);
void chooseAndPerformAction(WorkingMemory& WM, State& state);
double rewardFunction(Episode& episode, State& state);

#endif /* DST2_H */
