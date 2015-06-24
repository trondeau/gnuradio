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

from PyQt4 import QtGui, QtCore
import sys

from utils import *
from firfilters import FIRFilterParams
from iirfilters import IIRFilterParams

class parameters(QtGui.QVBoxLayout):
    def __init__(self, restype=None):
        super(parameters, self).__init__()

        self.filttypes = ["FIR", "IIR (scipy)"]
        self.firFilterParams = FIRFilterParams()
        self.iirFilterParams = IIRFilterParams()
        self.filterSelectsDict = {
            self.filttypes[0]: self.firFilterParams,
            self.filttypes[1]: self.iirFilterParams
        }

        self.fselectComboBox = QtGui.QComboBox()
        self.fselectComboBox.addItem(self.filttypes[0])
        self.fselectComboBox.addItem(self.filttypes[1])
        self.addWidget(self.fselectComboBox)

        # If we set a restriction type, force that selection in the
        # combo box and disable to prevent changes.
        if(restype):
            if(restype.lower() == "fir"):
                self.fselectComboBox.setCurrentIndex(0)
            else:
                self.fselectComboBox.setCurrentIndex(1)
            self.fselectComboBox.setDisabled(True)

        self.filterParams = self.filterSelectsDict[str(self.fselectComboBox.currentText())]
        self.addWidget(self.firFilterParams)

        self.paramGroupBox = QtGui.QGroupBox("Filter Properties")
        self.filterPropsFormLayout = QtGui.QFormLayout()
        self.ntapsLineEdit = QtGui.QLineEdit("0")
        self.ntapsLineEdit.setReadOnly(True)
        self.filterPropsFormLayout.addRow("Number of Taps", self.ntapsLineEdit)
        self.paramGroupBox.setLayout(self.filterPropsFormLayout)
        self.addWidget(self.paramGroupBox)

        self.designButton = QtGui.QPushButton("&Design")
        self.addWidget(self.designButton)

        self.addStretch(1)

        self.fselectComboBox.currentIndexChanged.connect(
            self.changedFilterSelect)

    def changedFilterSelect(self, fselect):
        newFilterSelect = self.filterSelectsDict[self.filttypes[fselect]]
        index = self.indexOf(self.filterParams)
        self.removeWidget(self.filterParams)
        self.filterParams.setVisible(False)

        self.filterParams = newFilterSelect
        self.filterParams.setVisible(True)
        self.insertWidget(index, self.filterParams)

    def setFilterParams(self, params):
        self.filterParams.setFilterParams(params)

    def design(self):
        try:
            filt = self.filterParams.design()
            self.ntapsLineEdit.setText("{0}".format(filt.ntaps()))
            return filt

        except RuntimeError, e:
            #reply = QtGui.QMessageBox.information(self.parent, 'Filter Design Error',
            #                                          "Filter Design Failed:\n{0}".format(e),
            #                                          QtGui.QMessageBox.Ok)
            return None

    def iir(self):
        # Weak -- do better.
        return self.filterParams.filterParams.filter.iir
