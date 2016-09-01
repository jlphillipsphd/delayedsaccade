all: DelayedSaccadeTask.exe

DelayedSaccadeTask.exe: wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp DelayedSaccadeTask.cpp
	g++ -g -std=c++11 wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp DelayedSaccadeTask.cpp -lgsl -lblas -o DelayedSaccadeTask.exe
