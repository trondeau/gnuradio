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

from PyQt4 import QtGui
from utils import *


class filterParams(QtGui.QGroupBox):
    def __init__(self):
        super(filterParams, self).__init__("")

        self.filter = Filter()

        self.startPassbandLineEdit = GrLineEdit("{0}".format(self.filter.start_passband))
        self.stopPassbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit = GrLineEdit("{0}".format(self.filter.start_stopband))
        self.stopStopbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_stopband))
        self.attenLineEdit = GrLineEdit("{0}".format(self.filter.atten))
        self.maxPassbandLossLineEdit = GrLineEdit("{0}".format(self.filter.ripple))

    def setStartPassband(self, x):
        self.filter.start_passband = x
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))

    def setStopPassband(self, x):
        self.filter.stop_passband = x
        self.stopPassbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_passband))

    def setStartStopband(self, x):
        self.filter.start_stopband = x
        self.startStopbandLineEdit = GrLineEdit("{0}".format(self.filter.start_stopband))

    def setStopStopband(self, x):
        self.filter.stop_stopband = x
        self.stopStopbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_stopband))

    def setAttenuation(self, x):
        self.filter.attenuation = x
        self.attenLineEdit = GrLineEdit("{0}".format(self.filter.atten))

    def setMaxPassbandLoss(self, x):
        self.filter.ripple = x
        self.maxPassbandLossLineEdit = GrLineEdit("{0}".format(self.filter.ripple))


    def startPassband(self):
        self.filter.start_passband = self.startPassbandLineEdit.text()
        return self.filter.start_passband

    def stopPassband(self):
        self.filter.stop_passband =  self.stopPassbandLineEdit.text()
        return self.filter.stop_passband

    def startStopband(self):
        self.filter.start_passband =  self.startStopbandLineEdit.text()
        return self.filter.start_passband

    def stopStopband(self):
        self.filter.stop_stopband =  self.stopStopbandLineEdit.text()
        return self.filter.stop_stopband

    def attenuation(self):
        self.filter.atten = self.attenLineEdit.text()
        return self.filter.atten

    def ripple(self):
        self.filter.ripple = self.maxPassbandLossLineEdit.text()
        return self.filter.ripple

    def setParams(self, infilter):
        self.filter = infilter

    def design(self, wtype):
        pass

class paramsLowpass(filterParams):
    def __init__(self):
        super(paramsLowpass, self).__init__()

        self.type = "iir::low_pass"
        self.filter.stop_passband = 0.2
        self.filter.start_stopband = 0.3
        self.filter.atten = 40
        self.filter.ripple = 0.1

        self.layout = QtGui.QFormLayout(self)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.layout.addRow("Start of Stop Band", self.startStopbandLineEdit)
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.layout.addRow("Max Loss in Passband (dB)", self.maxPassbandLossLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, wtype):
        super(paramsLowpass, self).design(wtype)

        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.start_stopband = self.startStopbandLineEdit.text()
        self.filter.ripple = self.maxPassbandLossLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.iir_low_pass(wtype)

        return self.filter

class paramsHighpass(filterParams):
    def __init__(self):
        super(paramsHighpass, self).__init__()

        self.type = "iir::high_pass"
        self.filter.start_passband = 0.3
        self.filter.stop_stopband = 0.35
        self.filter.atten = 40
        self.filter.ripple = 0.1

        self.layout = QtGui.QFormLayout(self)
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.layout.addRow("End of Stop Band", self.stopStopbandLineEdit)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.layout.addRow("Max Loss in Passband (dB)", self.maxPassbandLossLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, wtype):
        super(paramsHighpass,self).design(wtype)

        self.filter.stop_stopband = self.stopStopbandLineEdit.text()
        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.ripple = self.maxPassbandLossLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.iir_high_pass(wtype)

        return self.filter


class paramsBandpass(filterParams):
    def __init__(self):
        super(paramsBandpass, self).__init__()

        self.type = "iir::band_pass"
        self.filter.stop_stopband = 0.2
        self.filter.start_passband = 0.25
        self.filter.stop_passband = 0.3
        self.filter.start_stopband = 0.35
        self.filter.atten = 40
        self.filter.ripple = 0.1

        self.layout = QtGui.QFormLayout(self)
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.layout.addRow("End of Stop Band", self.stopStopbandLineEdit)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.layout.addRow("Start of Stop Band", self.startStopbandLineEdit)
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.layout.addRow("Max Loss in Passband (dB)", self.maxPassbandLossLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, wtype):
        super(paramsBandpass,self).design(wtype)

        self.filter.stop_stopband = self.stopStopbandLineEdit.text()
        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.start_stopband = self.startStopbandLineEdit.text()
        self.filter.ripple = self.maxPassbandLossLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.iir_band_pass(wtype)

        return self.filter

class paramsBandstop(filterParams):
    def __init__(self):
        super(paramsBandstop, self).__init__()
        self.type = "iir::band_stop"

        self.filter.stop_passband = 0.20
        self.filter.start_stopband = 0.25
        self.filter.stop_stopband = 0.30
        self.filter.start_passband = 0.35
        self.filter.atten = 40
        self.filter.ripple = 0.1

        self.layout = QtGui.QFormLayout(self)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.layout.addRow("Start of Stop Band", self.startStopbandLineEdit)
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.layout.addRow("End of Stop Band", self.stopStopbandLineEdit)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.layout.addRow("Max Loss in Passband (dB)", self.maxPassbandLossLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.maxPassbandLossLineEdit.setText("{0}".format(self.filter.ripple))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, wtype):
        super(paramsBandstop,self).design(wtype)

        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.start_stopband = self.startStopbandLineEdit.text()
        self.filter.stop_stopband = self.stopStopbandLineEdit.text()
        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.ripple = self.maxPassbandLossLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.iir_band_stop(wtype)

        return self.filter


class IIRFilterParams(QtGui.QGroupBox):
    def __init__(self):
        super(IIRFilterParams, self).__init__("")

        self.ftypes = ["Low Pass", "High Pass",
                       "Band Pass", "Band Notch"
        ]

        self.filterParamsLowpass = paramsLowpass()
        self.filterParamsHighpass = paramsHighpass()
        self.filterParamsBandpass = paramsBandpass()
        self.filterParamsBandstop = paramsBandstop()

        self.filterParamsHighpass.setVisible(False)
        self.filterParamsBandpass.setVisible(False)
        self.filterParamsBandstop.setVisible(False)

        self.filterParamBoxesDict = {
            self.ftypes[0]: self.filterParamsLowpass,
            self.ftypes[1]: self.filterParamsHighpass,
            self.ftypes[2]: self.filterParamsBandpass,
            self.ftypes[3]: self.filterParamsBandstop,
        }

        self.paramVlayout = QtGui.QVBoxLayout(self)

        self.filtTypeComboBox = QtGui.QComboBox()
        self.filtTypeComboBox.addItem(self.ftypes[0])
        self.filtTypeComboBox.addItem(self.ftypes[1])
        self.filtTypeComboBox.addItem(self.ftypes[2])
        self.filtTypeComboBox.addItem(self.ftypes[3])
        self.paramVlayout.addWidget(self.filtTypeComboBox)

        self.windowTypeComboBox = QtGui.QComboBox()
        self.windowTypeComboBox.addItem("Elliptic")
        self.windowTypeComboBox.addItem("Butterworth")
        self.windowTypeComboBox.addItem("Chebyshev-1")
        self.windowTypeComboBox.addItem("Chebyshev-2")
        #self.windowTypeComboBox.addItem("Bessel")
        self.paramVlayout.addWidget(self.windowTypeComboBox)

        self.filterParams = self.filterParamBoxesDict[str(self.filtTypeComboBox.currentText())]
        self.paramVlayout.addWidget(self.filterParams)

        self.filtTypeComboBox.currentIndexChanged.connect(
            self.changedFilterType)

    def setFilterParams(self, params):
        self.filterParamBoxesDict[params.filttype].setParams(params)
        self.windowTypeComboBox.setCurrentText(windowTypeDictRev[int(params.window)])
        self.filtTypeComboBox.setCurrentIndex(filterTypesDict[params.filttype])

    def changedFilterType(self, ftype):
        if(self.filterParamBoxesDict.has_key(self.ftypes[ftype])):
            newFilterParams = self.filterParamBoxesDict[self.ftypes[ftype]]
            index = self.paramVlayout.indexOf(self.filterParams)
            self.paramVlayout.removeWidget(self.filterParams)
            self.filterParams.setVisible(False)

            self.filterParams = newFilterParams
            self.filterParams.setVisible(True)
            self.paramVlayout.insertWidget(index, self.filterParams)

    def design(self):
        filt = self.filterParams.design(iirTypes[str(self.windowTypeComboBox.currentText())])
        #try:
        #    filt = self.filterParams.design(windowTypeDict[self.windowTypeComboBox.currentText()])
        #except StandardError, e:
        #    reply = QtGui.QMessageBox.information(self, 'IIR design error', e.args[0],
        #                                              QtGui.QMessageBox.Ok)
        return filt
