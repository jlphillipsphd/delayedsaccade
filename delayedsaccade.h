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
// delayedsaccade.h
//
// Provides tools for a visualization of the delayed saccade 
// demonstration of the Working Memory Toolkit.
//
// Original code by Trent Kriete
// Modified Tue Oct 19 13:07:30 CDT 2004 by David Noelle
//

#ifndef SACCADEWINDOW_H
#define SACCADEWINDOW_H

#include <qmainwindow.h>
#include <q3canvas.h>
#include <q3painter.h>
#include <q3popupmenu.h>

class Q3Action;
class Q3Canvas;
class Q3CanvasView;
class UnfilledEllipse;

class SaccadeWindow : public QMainWindow
{
 Q_OBJECT
 public:

  SaccadeWindow(QWidget *parent = 0, const char *name = 0);

  void addFixation();
  void addTarget();
  void addGaze();
  void addReward();
  void addTrialLabel();
  void addTickLabel();
  void addFChunkLabel();
  void addCChunkLabel();
  void addTChunkLabel();

  void showFixation(); 
  void showTarget(); 
  void showGaze(); 
  void showReward();
  void showTrialLabel();
  void showTickLabel();
  void showFChunkLabel();
  void showCChunkLabel();
  void showTChunkLabel();

  void hideFixation(); 
  void hideTarget(); 
  void hideGaze();  
  void hideReward();
  void hideTrialLabel();
  void hideTickLabel();
  void hideFChunkLabel();
  void hideCChunkLabel();
  void hideTChunkLabel();

  void moveTarget(int);
  void moveGaze(int);

  void setTrialLabel(int);
  void setTickLabel(int);
  void setFChunkLabel(bool);
  void setCChunkLabel(bool);
  void setTChunkLabel(bool);

  void initializeDisplay();
  void updateDisplay(int, int, bool, int, int, bool, bool, bool, bool);
  void pauseDisplay();

  bool forceGoodSeed();

 protected:

 public slots:
  void doRunSimulation();
  void setSlowDuration();
  void setFastDuration();
  void toggleStateVisibility();
  void toggleForcedSeed();

 signals:

 private:

  Q3CanvasView *canvasView;
  Q3Canvas *canvas;
  Q3CanvasLine *fixationHLine;	
  Q3CanvasLine *fixationVLine;	
  Q3CanvasEllipse *target;	
  UnfilledEllipse *gaze;	
  Q3CanvasEllipse *reward;
  Q3CanvasText *trialLabel;
  Q3CanvasText *tickLabel;
  Q3CanvasText *fChunkLabel;
  Q3CanvasText *cChunkLabel;
  Q3CanvasText *tChunkLabel;
  bool visibleState;
  bool forceSeed;

  unsigned long pauseDuration;

  Q3Action *runAct;
  Q3Action *visibleAct;
  Q3Action *seedAct;
  Q3Action *exitAct;
  Q3Action *slowAct;
  Q3Action *fastAct;

  Q3PopupMenu *actionsMenu;
  Q3PopupMenu *speedMenu;

};

#endif  // ifndef SACCADEWINDOW_H
