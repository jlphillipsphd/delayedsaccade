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
// unfilledellipse.h
//
// Provides a canvas object akin to the existing QCanvaseEllipse, but
// with a border, allowing for unfilled ellipse objects.
//
// Original code by Trent Kriete
// Modified Tue Oct 19 13:07:30 CDT 2004 by David Noelle
//

#ifndef UNFILLEDELLIPSE_H
#define UNFILLEDELLIPSE_H

#include <q3canvas.h>


class UnfilledEllipse : public Q3CanvasEllipse
{
 public:
  enum { RTTI = 1002 };
  UnfilledEllipse(int width, int height, Q3Canvas *canvas);
  ~UnfilledEllipse(); 
  int rtti() const { return RTTI; };
  void useBorder(bool flag) { showBorder=flag; };
  void setBorderColor(QColor color) { selectedPen.setColor(color); };
  void setBorderWidth(int width) { selectedPen.setWidth(width); };
	
 protected:
  void drawShape(QPainter &p);
  // void draw(QPainter &p);

 private:
  QPen selectedPen;
  bool showBorder;
};

#endif  // ifndef UNFILLEDELLIPSE_H

