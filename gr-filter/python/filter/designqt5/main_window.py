#!/usr/bin/env python
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

from optparse import OptionParser
from PyQt5 import QtWidgets, QtCore, QtGui
import pyqtgraph as pg
import sys, warnings, csv, re

from utils import *
from parameters import *
from plots import *
from api_object import *

class main_window(QtWidgets.QMainWindow):
    def __init__(self, options, callback=None):
        super(main_window, self).__init__(None)

        self.action_open = QtWidgets.QAction('&Open', self)
        self.action_save = QtWidgets.QAction('&Save', self)
        self.action_exit = QtWidgets.QAction('&Exit', self)

        self.shortcut_open = QtWidgets.QShortcut(self)
        self.shortcut_open.setKey(QtGui.QKeySequence.Open)
        self.shortcut_open.activated.connect(self.action_open_dialog)

        self.shortcut_save = QtWidgets.QShortcut(self)
        self.shortcut_save.setKey(QtGui.QKeySequence.Save)
        self.shortcut_save.activated.connect(self.action_save_dialog)

        self.menu_file = self.menuBar().addMenu('&File')
        self.menu_file.addAction(self.action_open)
        self.menu_file.addAction(self.action_save)
        self.menu_file.addAction(self.action_exit)
        self.menu_analysis = self.menuBar().addMenu('&Analysis')

        self.restype = None
        self.callback = callback
        if(options):
            if(type(options.type) is str):
                self.restype = options.type.lower()

        centralWidget = QtWidgets.QWidget(self)
        self.mainLayout = QtWidgets.QHBoxLayout()
        self.paramWidget = parameters(self.restype)
        self.mainLayout.addLayout(self.paramWidget)

        self.plotWidget = plots()
        self.plotWidget.setSizePolicy(QtWidgets.QSizePolicy.Expanding,
                                      QtWidgets.QSizePolicy.Expanding)
        self.mainLayout.addWidget(self.plotWidget)

        self.design()
        self.paramWidget.designButton.clicked.connect(self.design)

        self.mainLayout.setStretch(1, 1)
        centralWidget.setLayout(self.mainLayout)

        self.setCentralWidget(centralWidget)

        self.setGeometry(10, 10, 2000, 1200)
        self.setWindowTitle("GNU Radio Filter Design Tool")

        self.paramWidget.designButton.setShortcut(QtCore.Qt.Key_Return)
        self.action_exit.triggered.connect(QtWidgets.qApp.quit)

        # Add checkable items into the menu Analysis item
        self.actions = []
        for tab in self.plotWidget.stdplots.tablabels:
            self.actions.append(self.menu_analysis.addAction(tab))
            self.actions[-1].setCheckable(True)
            self.actions[-1].setChecked(True)

            self.actions[-1].toggled.connect(self.plotWidget.stdplots.tabToggle)

        self.show()

    def design(self):
        filterInfo = self.paramWidget.design()
        if filterInfo is None:
            return

        self.plotWidget.update(filterInfo)

        if self.callback:
            retobj = ApiObject()
            if not self.paramWidget.iir():
                retobj.update_all("fir", filterInfo.params(), filterInfo.taps(), 1)
            else:
                retobj.update_all("iir", filterInfo.params(), filterInfo.taps(), 1)
            self.callback(retobj)
        else:
            self.params = filterInfo.params()
            self.taps = filterInfo.taps()


    def action_save_dialog(self):
        filename = QtWidgets.QFileDialog.getSaveFileName(self, "Save CSV Filter File", ".", "")[0]
        if filename == "":
            return
        try:
            handle = open(filename, "wb")
        except IOError:
            reply = QtWidgets.QMessageBox.information(self, 'File Name',
                                                      "Could not save to file: {0}".format(filename),
                                                      QtWidgets.QMessageBox.Ok)
            return

        csvhandle = csv.writer(handle, delimiter=",")
        #indicate fir/iir for easy reading
        if self.paramWidget.iir():
            csvhandle.writerow(["restype", "iir"])
        else:
            csvhandle.writerow(["restype", "fir"])

        for k in self.params.keys():
            csvhandle.writerow([k, self.params[k]])
        if self.paramWidget.iir():
            csvhandle.writerow(["b",] + self.b)
            csvhandle.writerow(["a",] + self.a)
        else:
            csvhandle.writerow(["taps",] + self.taps)
        handle.close()

    def action_open_dialog(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, "Open CSV Filter File", ".", "")[0]
        if(len(filename) == 0):
            return
        try:
            handle = open(filename, "rb")
        except IOError:
            reply = QtWidgets.QMessageBox.information(self, 'File Name',
                                                      "Could not open file: {0}".format(filename),
                                                      QtWidgets.QMessageBox.Ok)
            return

        csvhandle = csv.reader(handle, delimiter=",")
        b_a={}
        taps = []
        params = {}
        for row in csvhandle:
            if (row[0] == "restype"):
                restype = row[1]
            elif(row[0] == "taps"):
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    taps = [complex(r) for r in row[1:]]
                else:
                    taps = [float(r) for r in row[1:]]
            elif(row[0] == "b" or row[0] == "a"):
                testcpx = re.findall("[+-]?\d+\.*\d*[Ee]?[-+]?\d+j", row[1])
                if(len(testcpx) > 0): # it's a complex
                    b_a[row[0]] = [complex(r) for r in row[1:]]
                else:
                    b_a[row[0]]= [float(r) for r in row[1:]]
            else:
                params[row[0]] = row[1]
        handle.close()

        filterInfo = Filter(params)
        filterInfo.set_taps(taps)

        self.paramWidget.setFilterParams(filterInfo)
        self.design()

def launch(args, options=None, callback=None):
    if callback == None:
        app = QtWidgets.QApplication(args)
        gplt = main_window(options, callback)
        app.exec_()
        if gplt.paramWidget.iir():
            retobj = ApiObject()
            retobj.update_all("iir", gplt.params, (gplt.b, gplt.a), 1)
            return retobj
        else:
            retobj = ApiObject()
            retobj.update_all("fir", gplt.params, gplt.taps, 1)
            return retobj
    else:
        gplt = main_window(options, callback)
        return gplt

def main(args, options=None):
    app = QtWidgets.QApplication(args)
    mw = main_window(options, None)
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
