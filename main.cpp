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

#include <qapplication.h>
#include <iostream>
#include <unistd.h>
#include "delayedsaccade.h"

using namespace std;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  SaccadeWindow *mainWin = new SaccadeWindow(0,"Main Window");
  app.setMainWidget(mainWin);
  mainWin->initializeDisplay();

  // QEvent *kickoff = new QEvent(QEvent::User);
  // app.postEvent(mainWin, kickoff);

//  mainWin->startTimer(1000);

  return app.exec();
}

/* qmake-qt4 (builds .pro file)
 * make 
 */
