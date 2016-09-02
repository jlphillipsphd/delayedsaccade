all: DelayedSaccadeTask.exe dst2.exe

DelayedSaccadeTask.exe: wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp DelayedSaccadeTask.cpp
	g++ -g -std=c++11 wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp DelayedSaccadeTask.cpp -lgsl -lblas -o DelayedSaccadeTask.exe

dst2.exe: wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp dst2.cpp
	g++ -g -std=c++11 wmtk/hrr/hrrengine.cpp wmtk/hrr/hrrOperators.cpp wmtk/CriticNetwork.cpp wmtk/WorkingMemory.cpp dst2.cpp -lgsl -lblas -o dst2.exe
