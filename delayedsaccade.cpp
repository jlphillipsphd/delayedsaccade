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
// delayedsaccade.cpp
//
// Provides tools for a visualization of the delayed saccade 
// demonstration of the Working Memory Toolkit.
//
// Original code by Trent Kriete
// Modified Tue Oct 19 13:07:30 CDT 2004 by David Noelle
//

#include <qapplication.h>
#include <q3canvas.h>
#include <qobject.h>
#include <q3action.h>
#include <q3painter.h>
#include <qmenubar.h>
#include <iostream>
#include <string>
#include <sstream>
#include "delayedsaccade.h"
#include "unfilledellipse.h"
#include "DelayedSaccadeTask.h"

using namespace std;


SaccadeWindow::SaccadeWindow(QWidget *parent, const char *name)
  : QMainWindow(parent, name)
{
  // initialize fields
  fixationHLine = NULL;	
  fixationVLine = NULL;	
  target = NULL;	
  gaze = NULL;	
  reward = NULL;
  trialLabel = NULL;
  tickLabel = NULL;
  fChunkLabel = NULL;
  cChunkLabel = NULL;
  tChunkLabel = NULL;
  pauseDuration = (unsigned long) 50000;
  visibleState = true;
  forceSeed = false;

  // set up canvas
  int canvasWidth = 800;
  int canvasHeight = 600;
  resize(canvasWidth, canvasHeight);
  canvas = new Q3Canvas(canvasWidth, canvasHeight);
  canvas->setDoubleBuffering(true);
  canvasView = new Q3CanvasView(canvas, this);
  setCentralWidget(canvasView);
  canvas->setBackgroundColor(Qt::black);
  setCaption(tr("WMtk Delayed Saccade Demonstration"));
  canvas->update();
  canvasView->show();

  // create menu and actions
  runAct = new Q3Action("Run", tr("&Run"), tr("Ctrl+R"), this);
  runAct->setStatusTip(tr("Run"));
  connect(runAct, SIGNAL(activated()), this, SLOT(doRunSimulation()));
  visibleAct = new Q3Action("Toggle Visibility", 
			   tr("Toggle &Visibility"), 
			   tr("Ctrl+V"), this);
  visibleAct->setStatusTip(tr("Toggle Internal State Visibility"));
  connect(visibleAct, SIGNAL(activated()), this, 
	  SLOT(toggleStateVisibility()));
  seedAct = new Q3Action("Toggle Good Seed", tr("Toggle &Good Seed"), 
			tr("Ctrl+G"), this);
  seedAct->setStatusTip(tr("Toggle Good Seed"));
  connect(seedAct, SIGNAL(activated()), this, SLOT(toggleForcedSeed()));
  exitAct = new Q3Action("Exit", tr("E&xit"), tr("Ctrl+Q"), this);
  exitAct->setStatusTip(tr("Exit"));
  connect(exitAct, SIGNAL(activated()), this, SLOT(close()));

  actionsMenu = new Q3PopupMenu(this);
  runAct->addTo(actionsMenu);
  visibleAct->addTo(actionsMenu);
  seedAct->addTo(actionsMenu);
  exitAct->addTo(actionsMenu);
  menuBar()->insertItem(tr("&Actions"), actionsMenu);

  slowAct = new Q3Action("Slow", tr("Slo&w"), tr("Ctrl+S"), this);
  slowAct->setStatusTip(tr("Slow"));
  connect(slowAct, SIGNAL(activated()), this, SLOT(setSlowDuration()));
  fastAct = new Q3Action("Fast", tr("&Fast"), tr("Ctrl+F"), this);
  fastAct->setStatusTip(tr("Fast"));
  connect(fastAct, SIGNAL(activated()), this, SLOT(setFastDuration()));

  speedMenu = new Q3PopupMenu(this);
  slowAct->addTo(speedMenu);
  fastAct->addTo(speedMenu);
  menuBar()->insertItem(tr("&Speed"), speedMenu);

}

void SaccadeWindow::addFixation() {
  const int fixation_length = 25;
  const int fixation_thickness = 5;
  const QColor fixation_color = Qt::white;

  int canvas_width = width();
  int canvas_height = height();

  int HLineX1 = (int) (canvas_width / 2 - fixation_length);
  int HLineY1 = (int) (canvas_height / 2);
  int HLineX2 = (int) (canvas_width / 2 + fixation_length);
  int HLineY2 = (int) HLineY1;
  int VLineX1 = (int) (canvas_width / 2);
  int VLineY1 = (int) (canvas_height / 2 - fixation_length);
  int VLineX2 = (int) VLineX1;
  int VLineY2 = (int) (canvas_height / 2 + fixation_length);

  QPen fixationPen(fixation_color, fixation_thickness);

  if (!fixationHLine) {
    fixationHLine = new Q3CanvasLine(canvas);
  }
  if (!fixationVLine) {
    fixationVLine = new Q3CanvasLine(canvas);
  }

  fixationHLine->setPoints(HLineX1, HLineY1, HLineX2, HLineY2);
  fixationVLine->setPoints(VLineX1, VLineY1, VLineX2, VLineY2);

  fixationHLine->setPen(fixationPen);
  fixationVLine->setPen(fixationPen);

  canvas->update();
}

void SaccadeWindow::showFixation() {
  fixationHLine->show();	
  fixationVLine->show();
  canvas->update();
}

void SaccadeWindow::hideFixation() {
  fixationHLine->hide();	
  fixationVLine->hide();
  canvas->update();
}

void SaccadeWindow::addTarget() {
  const int diameter = 70;
  const QColor target_color = Qt::red;

  if (!target) {
    target = new Q3CanvasEllipse(diameter, diameter, canvas);
  }

  // set default initial location of target ...
  moveTarget(-1);

  target->setBrush(target_color);

  canvas->update();
}

void SaccadeWindow::showTarget() {
  target->show();	
  canvas->update();
}

void SaccadeWindow::hideTarget() {
  target->hide();	
  canvas->update();
}

void SaccadeWindow::moveTarget(int position) {
  const int offset = 150;

  int canvas_center_x = (int) (width() / 2);
  int canvas_center_y = (int) (height() / 2);

  int x, y;

  switch (position) {
  case 1:
    x = canvas_center_x - offset;
    y = canvas_center_y - offset;
    break;
  case 2:
    x = canvas_center_x + offset;
    y = canvas_center_y - offset;
    break;
  case 3:
    x = canvas_center_x + offset;
    y = canvas_center_y + offset;
    break;
  case 4:
    x = canvas_center_x - offset;
    y = canvas_center_y + offset;
    break;
  default:
    x = canvas_center_x;
    y = canvas_center_y + offset;
    break;
  }
  target->setX(x);
  target->setY(y);
}

void SaccadeWindow::addGaze() {
  const int diameter = 100;
  const int thickness = 3;
  const QColor gaze_color = Qt::yellow;

  if (!gaze) {
    gaze = new UnfilledEllipse(diameter, diameter, canvas);
  }

  // set initial default location of gaze ...
  moveGaze(-1);

  gaze->setBrush(canvas->backgroundColor());
  gaze->useBorder(true);
  gaze->setBorderColor(gaze_color);
  gaze->setBorderWidth(thickness);
  gaze->setZ(-100.0);
  canvas->update();
}

void SaccadeWindow::showGaze() {
  gaze->show();	
  canvas->update();
}

void SaccadeWindow::hideGaze() {
  gaze->hide();	
  canvas->update();
}

void SaccadeWindow::moveGaze(int position) {
  const int offset = 150;

  int canvas_center_x = (int) (width() / 2);
  int canvas_center_y = (int) (height() / 2);

  int x, y;

  switch (position) {
  case 0:
    x = canvas_center_x;
    y = canvas_center_y;
    break;
  case 1:
    x = canvas_center_x - offset;
    y = canvas_center_y - offset;
    break;
  case 2:
    x = canvas_center_x + offset;
    y = canvas_center_y - offset;
    break;
  case 3:
    x = canvas_center_x + offset;
    y = canvas_center_y + offset;
    break;
  case 4:
    x = canvas_center_x - offset;
    y = canvas_center_y + offset;
    break;
  default:
    x = canvas_center_x;
    y = canvas_center_y + offset;
    break;
  }
  gaze->setX(x);
  gaze->setY(y);
}

void SaccadeWindow::addReward() {
  const int diameter = 20;
  const int x = width() - (diameter * 3);
  const int y = 30;
  const QColor reward_color = Qt::blue;

  if (!reward) {
    reward = new Q3CanvasEllipse(diameter, diameter, canvas);
  }

  reward->setBrush(reward_color);
  reward->setX(x);
  reward->setY(y);
  
  canvas->update();
}

void SaccadeWindow::showReward() {
  reward->show();	
  canvas->update();
}

void SaccadeWindow::hideReward() {
  reward->hide();	
  canvas->update();
}

void SaccadeWindow::addTrialLabel() {
  const int x = 20;
  const int y = 20;
  const QColor trial_color = Qt::green;

  if (!trialLabel) {
    trialLabel = new Q3CanvasText(canvas);
  }

  // set location
  trialLabel->setX(x);
  trialLabel->setY(y);

  trialLabel->setColor(trial_color);

  setTrialLabel(-1);

  canvas->update();
}

void SaccadeWindow::showTrialLabel() {
  trialLabel->show();	
  canvas->update();
}

void SaccadeWindow::hideTrialLabel() {
  trialLabel->hide();	
  canvas->update();
}

void SaccadeWindow::setTrialLabel(int trial) {
  stringstream label_str;
  string label;

  label_str << trial;
  label_str >> label;
  label = string("Trial:  ") + label;

  QString temp = label.c_str();
  trialLabel->setText(temp);
}

void SaccadeWindow::addTickLabel() {
  const int x = 20;
  const int y = 40;
  const QColor tick_color = Qt::green;

  if (!tickLabel) {
    tickLabel = new Q3CanvasText(canvas);
  }

  // set location
  tickLabel->setX(x);
  tickLabel->setY(y);

  tickLabel->setColor(tick_color);

  setTickLabel(-1);

  canvas->update();
}

void SaccadeWindow::showTickLabel() {
  tickLabel->show();	
  canvas->update();
}

void SaccadeWindow::hideTickLabel() {
  tickLabel->hide();	
  canvas->update();
}

void SaccadeWindow::setTickLabel(int tick) {
  stringstream label_str;
  string label;

  label_str << tick;
  label_str >> label;
  label = string("Tick:   ") + label;

  QString temp = label.c_str();
  tickLabel->setText(temp);
}

void SaccadeWindow::addFChunkLabel() {
  const int x = width() - 200;
  const int y = 20;
  const QColor chunk_color = Qt::yellow;

  if (!fChunkLabel) {
    fChunkLabel = new Q3CanvasText(canvas);
  }

  // set location
  fChunkLabel->setX(x);
  fChunkLabel->setY(y);

  fChunkLabel->setColor(chunk_color);

  setFChunkLabel(false);

  canvas->update();
}

void SaccadeWindow::showFChunkLabel() {
  fChunkLabel->show();	
  canvas->update();
}

void SaccadeWindow::hideFChunkLabel() {
  fChunkLabel->hide();	
  canvas->update();
}

void SaccadeWindow::setFChunkLabel(bool present) {
  if (present)
    fChunkLabel->setText("F");
  else
    fChunkLabel->setText(" ");
}

void SaccadeWindow::addCChunkLabel() {
  const int x = width() - 180;
  const int y = 20;
  const QColor chunk_color = Qt::white;

  if (!cChunkLabel) {
    cChunkLabel = new Q3CanvasText(canvas);
  }

  // set location
  cChunkLabel->setX(x);
  cChunkLabel->setY(y);

  cChunkLabel->setColor(chunk_color);

  setCChunkLabel(false);

  canvas->update();
}

void SaccadeWindow::showCChunkLabel() {
  cChunkLabel->show();	
  canvas->update();
}

void SaccadeWindow::hideCChunkLabel() {
  cChunkLabel->hide();	
  canvas->update();
}

void SaccadeWindow::setCChunkLabel(bool present) {
  if (present)
    cChunkLabel->setText("C");
  else
    cChunkLabel->setText(" ");
}

void SaccadeWindow::addTChunkLabel() {
  const int x = width() - 160;
  const int y = 20;
  const QColor chunk_color = Qt::red;

  if (!tChunkLabel) {
    tChunkLabel = new Q3CanvasText(canvas);
  }

  // set location
  tChunkLabel->setX(x);
  tChunkLabel->setY(y);

  tChunkLabel->setColor(chunk_color);

  setTChunkLabel(false);

  canvas->update();
}

void SaccadeWindow::showTChunkLabel() {
  tChunkLabel->show();	
  canvas->update();
}

void SaccadeWindow::hideTChunkLabel() {
  tChunkLabel->hide();	
  canvas->update();
}

void SaccadeWindow::setTChunkLabel(bool present) {
  if (present)
    tChunkLabel->setText("T");
  else
    tChunkLabel->setText(" ");
}

bool SaccadeWindow::forceGoodSeed() {
  return forceSeed;
}

void SaccadeWindow::toggleForcedSeed() {
  forceSeed = !forceSeed;
}

void SaccadeWindow::initializeDisplay() {
  const QColor bg_color = Qt::black;

  setPaletteBackgroundColor(bg_color);
  addGaze();
  addTarget();
  addReward();
  addTrialLabel();
  addTickLabel();
  addFChunkLabel();
  addCChunkLabel();
  addTChunkLabel();
  addFixation();
  hideFixation(); 
  hideTarget(); 
  hideGaze(); 
  hideReward();
  hideTrialLabel();
  hideTickLabel();
  hideFChunkLabel();
  hideCChunkLabel();
  hideTChunkLabel();
  canvas->update();
  show();
}

void SaccadeWindow::updateDisplay(int trial,
				  int tick,
				  bool crosshairs,
				  int target_position,
				  int gaze_position,
				  bool f_chunk,
				  bool c_chunk,
				  bool t_chunk,
				  bool reward) {
  setTrialLabel(trial);
  if (visibleState)
    showTrialLabel();
  else
    hideTrialLabel();
  setTickLabel(tick);
  if (visibleState)
    showTickLabel();
  else
    hideTickLabel();
  if (target_position >= 0) {
    moveTarget(target_position);
    showTarget();
  } else {
    hideTarget();
  }
  if (gaze_position >= 0) {
    moveGaze(gaze_position);
    if (visibleState)
      showGaze();
    else
      hideGaze();
  } else {
    hideGaze();
  }
  if (crosshairs)
    showFixation();
  else
    hideFixation();
  setFChunkLabel(f_chunk);
  if (visibleState)
    showFChunkLabel();
  else
    hideFChunkLabel();
  setCChunkLabel(c_chunk);
  if (visibleState)
    showCChunkLabel();
  else
    hideCChunkLabel();
  setTChunkLabel(t_chunk);
  if (visibleState)
    showTChunkLabel();
  else
    hideTChunkLabel();
  if (reward) 
    showReward();
  else
    hideReward();
  canvasView->show();
  canvas->update();
}

void SaccadeWindow::pauseDisplay() {
  for (int i = 0; (unsigned long) i < pauseDuration; i++) {
    qApp->processEvents();
  }
}

void SaccadeWindow::toggleStateVisibility() {
  visibleState = !visibleState;
}

void SaccadeWindow::setSlowDuration() {
  pauseDuration = 50000;
}

void SaccadeWindow::setFastDuration() {
  pauseDuration = 10;
}

void SaccadeWindow::doRunSimulation()
{
  RunSimulation(*this);
}	

