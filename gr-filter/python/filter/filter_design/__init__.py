#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import sys
from optparse import OptionParser

usage="%prog: [options] (input_filename)"
description = "The GNU Radio filter design tool"
parser = OptionParser(conflict_handler="resolve",
                      usage=usage, description=description)
parser.add_option("-G", "--gui", type="choice", choices=['QWT', 'QT4'],
                  default='QT4', help="Set the GUI backend [QWT, QT4]")
parser.add_option("-t", "--type", type="choice", choices=['FIR', 'IIR', 'fir', 'iir'],
                  default=None, help="restrict filter to either FIR or IIR")
(options, args) = parser.parse_args()


if(options.gui == "QWT"):
    try:
        import PyQt4.Qwt5 as Qwt
        from gnuradio.filter.designqwt import main_window
    except ImportError:
        sys.stderr.write("GUI type set to QWT but PyQWT not found. "
                         "Please install PyQWT or use another GUI type.\n")
        sys.exit(1)

elif(options.gui == "QT4"):
    try:
        from PyQt4 import QtCore
        import pyqtgraph
        from gnuradio.filter.designqt4 import main_window
    except ImportError:
        sys.stderr.write("GUI type set to QT4 but either PyQt4 or PyQtGraph not found. "
                         "Please make sure PyQT4 and PyQtGraph are installed "
                         "or use another GUI type.\n")
        sys.exit(1)

#elif(options.gui == "QT5"):
#    try:
#        from PyQt5 import QtWidgets
#        from gnuradio.filter.designqt5 import main_window
#    except ImportError:
#        sys.stderr.write("GUI type set to QT5 but either PyQt5 or PyQtGraph not found. "
#                         "Please make sure PyQT5 and PyQtGraph are installed "
#                         "or use another GUI type.\n")
#        sys.exit(1)

else:
    sys.stderr.write("GUI type '{0}' selected is not valid.\n".format(options.gui))
    parser.print_help()
    sys.exit(1)


main = main_window.main
launch = main_window.launch
