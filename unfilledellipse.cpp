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
// unfilledellipse.cpp
//
// Provides a canvas object akin to the existing QCanvaseEllipse, but
// with a border, allowing for unfilled ellipse objects.
//
// Original code by Trent Kriete
// Modified Tue Oct 19 13:07:30 CDT 2004 by David Noelle
//

#include <q3canvas.h>
#include <q3painter.h>
#include "unfilledellipse.h"

UnfilledEllipse::UnfilledEllipse(int width, int height, Q3Canvas *canvas)
  : Q3CanvasEllipse(width, height, canvas)
{
  selectedPen = QPen (Qt::blue, 1);
  showBorder=false;
}

/* OVERRIDE drawShape */
void UnfilledEllipse::drawShape (QPainter &p) 
{
  Q3CanvasEllipse::drawShape(p);
  
  // draw a border around the ellipse
  if(showBorder) {
    int penWidth = selectedPen.width();
    p.setPen(selectedPen);	
    p.drawArc(int( x() - width()/2 + penWidth /2 ), 
	      int( y() - height()/2 + penWidth/2 ), 
	      width() - penWidth,
	      height() - penWidth, 
	      0, 16*360);
  }
}
/* END drawShape */

UnfilledEllipse::~UnfilledEllipse() 
{
  // hide item must be called because it calls areaPoints()
  this->hide();
};

