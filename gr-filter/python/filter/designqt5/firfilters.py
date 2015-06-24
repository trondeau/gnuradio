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
from utils import *

class filterParams(QtWidgets.QGroupBox):
    def __init__(self):
        super(filterParams, self).__init__("")

        self.filter = Filter()

        self.startPassbandLineEdit = GrLineEdit("{0}".format(self.filter.start_passband))
        self.stopPassbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit = GrLineEdit("{0}".format(self.filter.start_stopband))
        self.stopStopbandLineEdit = GrLineEdit("{0}".format(self.filter.stop_stopband))
        self.transitionBandLineEdit = GrLineEdit("{0}".format(self.filter.transition_band))
        self.attenLineEdit = GrLineEdit("{0}".format(self.filter.atten))
        self.rippleLineEdit = None
        self.betaLineEdit = None

    def addBetaLineEdit(self):
        self.betaLineEdit = GrLineEdit("{0}".format(self.filter.win_beta))
        self.layout.addRow("Beta", self.betaLineEdit)

    def removeBetaLineEdit(self):
        if self.betaLineEdit:
            label = self.layout.labelForField(self.betaLineEdit)
            if label is not None:
                label.deleteLater()
                self.betaLineEdit.deleteLater()
                self.betaLineEdit = None

    def addRippleLineEdit(self):
        self.rippleLineEdit = GrLineEdit("{0}".format(self.filter.ripple))
        self.layout.addRow("Ripple (dB)", self.rippleLineEdit)

    def removeRippleLineEdit(self):
        if self.rippleLineEdit:
            label = self.layout.labelForField(self.rippleLineEdit)
            if label is not None:
                label.deleteLater()
                self.rippleLineEdit.deleteLater()
                self.rippleLineEdit = None


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

    def setTransitionBand(self, x):
        self.filter.transition_band = x
        self.transitionBandLineEdit = GrLineEdit("{0}".format(self.filter.transition_band))

    def setAttenuation(self, x):
        self.filter.attenuation = x
        self.attenLineEdit = GrLineEdit("{0}".format(self.filter.atten))

    def setRipple(self, x):
        self.filter.ripple = x
        if self.rippleLineEdit:
            self.rippleLineEdit = GrLineEdit("{0}".format(self.filter.ripple))


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

    def transitionBand(self):
        self.filter.transition_band = self.transitionBandLineEdit.text()
        return self.filter.transition_band

    def attenuation(self):
        self.filter.atten = self.attenLineEdit.text()
        return self.filter.atten

    def ripple(self):
        if self.rippleLineEdit:
            self.filter.ripple = self.rippleLineEdit.text()
        return self.filter.ripple

    def setParams(self, infilter):
        self.filter = infilter

    def design(self, gain, samp_rate, window):
        self.filter.gain = gain
        self.filter.samp_rate = samp_rate
        self.filter.window = window

        if window == EQUIRIPPLE:
            self.filter.ripple = self.rippleLineEdit.text()

        if self.betaLineEdit:
            self.filter.win_beta = self.betaLineEdit.text()


class paramsLowpass(filterParams):
    def __init__(self):
        super(paramsLowpass, self).__init__()

        self.filter.stop_passband = 8000
        self.filter.start_stopband = 10000
        self.filter.transition_band = self.filter.start_stopband - self.filter.stop_passband
        self.filter.atten = 40
        self.filter.ripple = 0.1
        self.filter.win_beta = 6.76

        self.layout = QtWidgets.QFormLayout(self)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.layout.addRow("Start of Stop Band", self.startStopbandLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.startStopbandLineEdit.setText("{0}".format(self.filter.start_stopband))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, gain, samp_rate, window):
        super(paramsLowpass,self).design(gain, samp_rate, window)

        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.start_stopband = self.startStopbandLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.low_pass()

        return self.filter

class paramsHighpass(filterParams):
    def __init__(self):
        super(paramsHighpass, self).__init__()

        self.type = "high_pass_2"
        self.filter.start_passband = 10000
        self.filter.stop_stopband = 8000
        self.filter.transition_band = self.filter.start_stopband - self.filter.stop_passband
        self.filter.atten = 40
        self.filter.ripple = 0.1
        self.filter.win_beta = 6.76

        self.layout = QtWidgets.QFormLayout(self)
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.layout.addRow("End of Stop Band", self.stopStopbandLineEdit)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopStopbandLineEdit.setText("{0}".format(self.filter.stop_stopband))
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, gain, samp_rate, window):
        super(paramsHighpass,self).design(gain, samp_rate, window)

        self.filter.stop_stopband = self.stopStopbandLineEdit.text()
        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.high_pass()

        return self.filter


class paramsBandpass(filterParams):
    def __init__(self):
        super(paramsBandpass, self).__init__()

        self.type = "band_pass_2"
        self.filter.start_passband = 8000
        self.filter.stop_passband = 10000
        self.filter.transition_band = 1000
        self.filter.atten = 40
        self.filter.ripple = 0.1
        self.filter.win_beta = 6.76

        self.layout = QtWidgets.QFormLayout(self)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.transitionBandLineEdit.setText("{0}".format(self.filter.transition_band))
        self.layout.addRow("Transition Width", self.transitionBandLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.transitionBandLineEdit.setText("{0}".format(self.filter.transition_band))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, gain, samp_rate, window):
        super(paramsBandpass,self).design(gain, samp_rate, window)

        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.transition_band = self.transitionBandLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.band_pass()

        return self.filter

class paramsComplexBandpass(paramsBandpass):
    def __init__(self):
        super(paramsComplexBandpass, self).__init__()
        self.type = "complex_band_pass_2"

    def setParams(self, infilter):
        self.filter = infilter
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.transitionBandLineEdit.setText("{0}".format(self.filter.transition_band))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, gain, samp_rate, window):
        super(paramsComplexBandpass,self).design(gain, samp_rate, window)

        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.transition_band = self.transitionBandLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.complex_band_pass()

        return self.filter


class paramsBandnotch(filterParams):
    def __init__(self):
        super(paramsBandnotch, self).__init__()

        self.type = "band_reject_2"
        self.filter.stop_passband = 8000
        self.filter.start_passband = 10000
        self.filter.transition_band = 1000
        self.filter.atten = 40
        self.filter.ripple = 0.1
        self.filter.win_beta = 6.76

        self.layout = QtWidgets.QFormLayout(self)
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.layout.addRow("End of Pass Band", self.stopPassbandLineEdit)
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.layout.addRow("Start of Pass Band", self.startPassbandLineEdit)
        self.transitionBandLineEdit.setText("{0}".format(self.filter.transition_band))
        self.layout.addRow("Transition Width", self.transitionBandLineEdit)
        self.attenLineEdit.setText("{0}".format(self.filter.atten))
        self.layout.addRow("Atten. (dB)", self.attenLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.stopPassbandLineEdit.setText("{0}".format(self.filter.stop_passband))
        self.startPassbandLineEdit.setText("{0}".format(self.filter.start_passband))
        self.transitionBandLineEdit.setText("{0}".format(self.filter.transition_band))
        self.attenLineEdit.setText("{0}".format(self.filter.atten))

    def design(self, gain, samp_rate, window):
        super(paramsBandnotch,self).design(gain, samp_rate, window)

        self.filter.stop_passband = self.stopPassbandLineEdit.text()
        self.filter.start_passband = self.startPassbandLineEdit.text()
        self.filter.transition_band = self.transitionBandLineEdit.text()
        self.filter.atten = self.attenLineEdit.text()

        self.filter.band_notch()

        return self.filter


class paramsRRC(filterParams):
    def __init__(self):
        super(paramsRRC, self).__init__()

        self.type = "root_raised_cosine"
        self.filter.symbol_rate = 8000
        self.filter.rolloff = 0.35
        self.filter._ntaps = 11

        self.layout = QtWidgets.QFormLayout(self)
        self.symbolRateLineEdit = GrLineEdit("{0}".format(self.filter.symbol_rate))
        self.layout.addRow("Symbol Rate (sps)", self.symbolRateLineEdit)
        self.rolloffFactorLineEdit = GrLineEdit("{0}".format(self.filter.rolloff))
        self.layout.addRow("Roll-off Factor", self.rolloffFactorLineEdit)
        self.nTapsLineEdit = GrLineEdit("{0}".format(self.filter._ntaps))
        self.layout.addRow("Number of Taps", self.nTapsLineEdit)

    def setParams(self, infilter):
        self.filter = infilter
        self.symbolRateLineEdit.setText("{0}".format(self.filter.symbol_rate))
        self.rolloffFactorLineEdit.setText("{0}".format(self.filter.rolloff))
        self.nTapsLineEdit.setText("{0}".format(self.filter._ntaps))

    def design(self, gain, samp_rate, window):
        super(paramsRRC,self).design(gain, samp_rate, window)

        self.filter.symbol_rate = self.symbolRateLineEdit.text()
        self.filter.rolloff = self.rolloffFactorLineEdit.text()
        self.filter._ntaps = int(self.nTapsLineEdit.text())

        self.filter.rrc()

        return self.filter

class paramsGaussian(filterParams):
    def __init__(self):
        super(paramsGaussian, self).__init__()

        self.type = "gaussian"
        self.filter.symbol_rate = 2000
        self.filter.rolloff = 0.5
        self.filter._ntaps = 11

        self.layout = QtWidgets.QFormLayout(self)
        self.symbolRateLineEdit = GrLineEdit("{0}".format(self.filter.symbol_rate))
        self.layout.addRow("Symbol Rate (sps)", self.symbolRateLineEdit)
        self.rolloffFactorLineEdit = GrLineEdit("{0}".format(self.filter.rolloff))
        self.layout.addRow("Roll-off Factor", self.rolloffFactorLineEdit)
        self.nTapsLineEdit = GrLineEdit("{0}".format(self.filter._ntaps))
        self.layout.addRow("Number of Taps", self.nTapsLineEdit)


    def setParams(self, infilter):
        self.filter = infilter
        self.symbolRateLineEdit.setText("{0}".format(self.filter.symbol_rate))
        self.rolloffFactorLineEdit.setText("{0}".format(self.filter.rolloff))
        self.nTapsLineEdit.setText("{0}".format(self.filter._ntaps))

    def design(self, gain, samp_rate, window):
        super(paramsGaussian,self).design(gain, samp_rate, window)

        self.filter.symbol_rate = self.symbolRateLineEdit.text()
        self.filter.rolloff = self.rolloffFactorLineEdit.text()
        self.filter._ntaps = int(self.nTapsLineEdit.text())

        self.filter.gaussian()

        return self.filter

class paramsHalfband(filterParams):
    def __init__(self):
        super(paramsHalfband, self).__init__()

        self.type = "halfband"
        self.filter.symbol_rate = 8000
        self.filter.rolloff = 40
        self.filter.ntaps = 0.1
        self.filter.win_beta = 6.76


class FIRFilterParams(QtWidgets.QGroupBox):
    def __init__(self):
        super(FIRFilterParams, self).__init__("")

        self.filterParamsLowpass = paramsLowpass()
        self.filterParamsHighpass = paramsHighpass()
        self.filterParamsBandpass = paramsBandpass()
        self.filterParamsComplexBandpass = paramsComplexBandpass()
        self.filterParamsBandnotch = paramsBandnotch()
        self.filterParamsRRC = paramsRRC()
        self.filterParamsGaussian = paramsGaussian()
        self.filterParamsHalfband = paramsHalfband()

        self.filterParamsHighpass.setVisible(False)
        self.filterParamsBandpass.setVisible(False)
        self.filterParamsComplexBandpass.setVisible(False)
        self.filterParamsBandnotch.setVisible(False)
        self.filterParamsRRC.setVisible(False)
        self.filterParamsGaussian.setVisible(False)
        self.filterParamsHalfband.setVisible(False)

        self.filterParamBoxesDict = {
            "Low Pass": self.filterParamsLowpass,
            "High Pass": self.filterParamsHighpass,
            "Band Pass": self.filterParamsBandpass,
            "Complex Band Pass": self.filterParamsComplexBandpass,
            "Band Notch": self.filterParamsBandnotch,
            "Root Raised Cosine": self.filterParamsRRC,
            "Gaussian": self.filterParamsGaussian,
            "Half Band": self.filterParamsHalfband,
        }

        self.paramVlayout = QtWidgets.QVBoxLayout(self)

        self.filtTypeComboBox = QtWidgets.QComboBox()
        self.filtTypeComboBox.addItem("Low Pass")
        self.filtTypeComboBox.addItem("Band Pass")
        self.filtTypeComboBox.addItem("Complex Band Pass")
        self.filtTypeComboBox.addItem("Band Notch")
        self.filtTypeComboBox.addItem("High Pass")
        self.filtTypeComboBox.addItem("Root Raised Cosine")
        self.filtTypeComboBox.addItem("Gaussian")
        self.filtTypeComboBox.addItem("Half Band")
        self.paramVlayout.addWidget(self.filtTypeComboBox)

        self.windowTypeComboBox = QtWidgets.QComboBox()
        self.windowTypeComboBox.addItem("Hann")
        self.windowTypeComboBox.addItem("Hamming")
        self.windowTypeComboBox.addItem("Blackman")
        self.windowTypeComboBox.addItem("Blackman-harris")
        self.windowTypeComboBox.addItem("Kaiser")
        self.windowTypeComboBox.addItem("Rectangular")
        self.windowTypeComboBox.addItem("Bartlett")
        self.windowTypeComboBox.addItem("Equiripple")
        self.paramVlayout.addWidget(self.windowTypeComboBox)

        self.paramsFormLayout = QtWidgets.QFormLayout()
        self.sampleRateLineEdit = GrLineEdit("32000")
        self.paramsFormLayout.addRow("Sample Rate", self.sampleRateLineEdit)
        self.gainLineEdit = GrLineEdit("1")
        self.paramsFormLayout.addRow("Gain", self.gainLineEdit)
        self.paramVlayout.addLayout(self.paramsFormLayout)

        self.filterParams = self.filterParamBoxesDict[self.filtTypeComboBox.currentText()]
        self.paramVlayout.addWidget(self.filterParams)

        self.filtTypeComboBox.currentTextChanged.connect(
            self.changedFilterType)
        self.windowTypeComboBox.currentTextChanged.connect(
            self.changedWindowType)

    def setFilterParams(self, params):
        self.sampleRateLineEdit.setText(params.samp_rate)
        self.gainLineEdit.setText(params.gain)
        self.filterParamBoxesDict[params.filttype].setParams(params)
        self.windowTypeComboBox.setCurrentText(windowTypeDictRev[int(params.window)])
        self.filtTypeComboBox.setCurrentIndex(filterTypesDict[params.filttype])

    def changedFilterType(self, ftype):
        if(self.filterParamBoxesDict.has_key(ftype)):
            newFilterParams = self.filterParamBoxesDict[ftype]
            index = self.paramVlayout.indexOf(self.filterParams)
            self.paramVlayout.removeWidget(self.filterParams)
            self.filterParams.setVisible(False)

            self.filterParams = newFilterParams
            self.filterParams.setVisible(True)
            self.paramVlayout.insertWidget(index, self.filterParams)

    def changedWindowType(self, wtype):
        if(self.windowTypeComboBox.currentText() == "Equiripple"):
            self.filterParams.addRippleLineEdit()
        else:
            self.filterParams.removeRippleLineEdit()
            if(self.windowTypeComboBox.currentText() == "Kaiser"):
                self.filterParams.addBetaLineEdit()
            else:
                self.filterParams.removeBetaLineEdit()


    def design(self):
        filt = self.filterParams.design(self.gainLineEdit.text(),
                                        self.sampleRateLineEdit.text(),
                                        windowTypeDict[self.windowTypeComboBox.currentText()])
        return filt
