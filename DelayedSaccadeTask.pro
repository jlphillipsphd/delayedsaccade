## Delayed Saccade Task (Qt Demo)
## Demo Application for the Working Memory Toolkit
## Copyright (C) 2006, Joshua L. Phillips (1),
##                     Trenton Kriete (2) &
##                     David C. Noelle (3)
## (1) Department of Computer Science, Middle Tennessee State
##     University
##     Email: Joshua.Phillips@mtsu.edu
## (2) Chief Data Scientist, RxREVU
## (3) School of Social Sciences, Humanities, and Arts, University of
##     California, Merced

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

TEMPLATE = app
INCLUDEPATH += . /usr/include/qt4/Qt WMtk WMtk/src
LIBS += WMtk/src/.libs/libWMtk.a
QT += qt3support

wmtk.target = WMtk/src/.libs/libWMtk.a
wmtk.commands = cd WMtk; ./configure ; make
wmtk.depends = wmtk_output

wmtk_output.commands = @echo Building the Working Memory Toolkit...

PRE_TARGETDEPS += $$wmtk.target
QMAKE_EXTRA_TARGETS += wmtk wmtk_output

# Input
HEADERS += delayedsaccade.h DelayedSaccadeTask.h unfilledellipse.h
SOURCES += delayedsaccade.cpp \
           DelayedSaccadeTask.cpp \
           main.cpp \
           unfilledellipse.cpp
