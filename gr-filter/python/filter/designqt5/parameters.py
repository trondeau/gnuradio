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

from PyQt5 import QtWidgets
from PyQt5.QtCore import Qt
import sys

from utils import *
from firfilters import FIRFilterParams
from iirfilters import IIRFilterParams

class parameters(QtWidgets.QVBoxLayout):
    def __init__(self, restype=None):
        super(parameters, self).__init__()

        self.firFilterParams = FIRFilterParams()
        self.iirFilterParams = IIRFilterParams()
        self.filterSelectsDict = {
            "FIR": self.firFilterParams,
            "IIR (scipy)": self.iirFilterParams
        }

        self.fselectComboBox = QtWidgets.QComboBox()
        self.fselectComboBox.addItem("FIR")
        self.fselectComboBox.addItem("IIR (scipy)")
        self.addWidget(self.fselectComboBox)

        # If we set a restriction type, force that selection in the
        # combo box and disable to prevent changes.
        if(restype):
            if(restype.lower() == "fir"):
                self.fselectComboBox.setCurrentText("FIR")
            else:
                self.fselectComboBox.setCurrentText("IIR (scipy)")
            self.fselectComboBox.setDisabled(True)

        self.filterParams = self.filterSelectsDict[self.fselectComboBox.currentText()]
        self.addWidget(self.firFilterParams)

        self.paramGroupBox = QtWidgets.QGroupBox("Filter Properties")
        self.filterPropsFormLayout = QtWidgets.QFormLayout()
        self.ntapsLineEdit = QtWidgets.QLineEdit("0")
        self.ntapsLineEdit.setReadOnly(True)
        self.filterPropsFormLayout.addRow("Number of Taps", self.ntapsLineEdit)
        self.paramGroupBox.setLayout(self.filterPropsFormLayout)
        self.addWidget(self.paramGroupBox)

        self.designButton = QtWidgets.QPushButton("&Design")
        self.addWidget(self.designButton)

        self.addStretch(1)

        self.fselectComboBox.currentTextChanged.connect(
            self.changedFilterSelect)

    def changedFilterSelect(self, fselect):
        newFilterSelect = self.filterSelectsDict[fselect]
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
            #reply = QtWidgets.QMessageBox.information(self.parent, 'Filter Design Error',
            #                                          "Filter Design Failed:\n{0}".format(e),
            #                                          QtWidgets.QMessageBox.Ok)
            return None

    def iir(self):
        return self.filterParams.iir
