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
from PyQt4.QtCore import pyqtProperty
import pyqtgraph as pg
import sys, warnings

try:
    import scipy, numpy
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1


class StandardPlots(QtGui.QTabWidget):
    def __init__(self, parent=None):
        super(StandardPlots, self).__init__(parent)

        self.tablabels = ["Magnitude Response", "Filter Taps",
                          "Phase Response", "Group Delay", "Phase Delay",
                          "Impulse Response", "Step Response",
                          "Filter Coefficients",]

        self.tabwidgets = {}
        for tab in self.tablabels[:-1]:
            self.tabwidgets[tab] = pg.PlotWidget()
            self.addTab(self.tabwidgets[tab], tab)

        labelstyle = {'font-size': '32px'}
        self._width = 0
        self._color = "FF6905"

        # Specifics of Magnitude Response
        plot = self.tabwidgets[self.tablabels[0]]
        plot.setLabel('left', 'Magnitude', 'dB', **labelstyle)
        plot.setLabel('bottom', 'Frequency', 'Hz', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self.getWidth()})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self.getWidth()})
        self.plotFreq = plot.plot(range(0), range(0))
        self.plotFreq.setPen({'color': self._color, "width": self.getWidth()})

        # Specifics of Filter Taps
        plot = self.tabwidgets[self.tablabels[1]]
        plot.setLabel('left', 'Amplitude', '', **labelstyle)
        plot.setLabel('bottom', 'Tap Number', '', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotTime = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotTimeIm = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotTime.setPen({'color': self._color, "width": self._width})
        self.plotTimeIm.setPen({'color': "000000", "width": self._width})

        # Specifics of Phase Response
        plot = self.tabwidgets[self.tablabels[2]]
        plot.setLabel('left', 'Phase (Radians)', '', **labelstyle)
        plot.setLabel('bottom', 'Frequency', 'Hz', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotPhase = plot.plot(range(0), range(0))
        self.plotPhase.setPen({'color': self._color, "width": self._width})

        # Specifics of Group Delay
        plot = self.tabwidgets[self.tablabels[3]]
        plot.setLabel('left', 'Group Delay', 's', **labelstyle)
        plot.setLabel('bottom', 'Frequency', 'Hz', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotGroupDel = plot.plot(range(0), range(0))
        self.plotGroupDel.setPen({'color': self._color, "width": self._width})

        # Specifics of Phase Delay
        plot = self.tabwidgets[self.tablabels[4]]
        plot.setLabel('left', 'Phase Delay', '', **labelstyle)
        plot.setLabel('bottom', 'Frequency', 'Hz', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotPhaseDel = plot.plot(range(0), range(0))
        self.plotPhaseDel.setPen({'color': self._color, "width": self._width})

        # Specifics of Impulse Response
        plot = self.tabwidgets[self.tablabels[5]]
        plot.setLabel('left', 'Amplitude', '', **labelstyle)
        plot.setLabel('bottom', 'n (samples)', '', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotImpulseResponse = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotImpulseResponseIm = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotImpulseResponse.setPen({'color': self._color, "width": self._width})
        self.plotImpulseResponseIm.setPen({'color': "000000", "width": self._width})

        # Specifics of Step Response
        plot = self.tabwidgets[self.tablabels[6]]
        plot.setLabel('left', 'Amplitude', '', **labelstyle)
        plot.setLabel('bottom', 'n (samples)', '', **labelstyle)
        plot.getAxis('left').setPen({'color': self._color, "width": self._width})
        plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.plotStepResponse = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotStepResponseIm = plot.plot(range(0), range(0), symbol='o', symbolsize=16)
        self.plotStepResponse.setPen({'color': self._color, "width": self._width})
        self.plotStepResponseIm.setPen({'color': "000000", "width": self._width})

        # Specifics of Filter Coeffs
        self.tabwidgets[self.tablabels[7]] = QtGui.QTextEdit(self)
        self.addTab(self.tabwidgets[self.tablabels[7]], self.tablabels[7])
        self.filterCoeffs = self.tabwidgets[self.tablabels[7]]
        self.filterCoeffs.setReadOnly(True)
        p = self.filterCoeffs.palette()
        p.setColor(QtGui.QPalette.Base, QtGui.QColor("#000000"))
        self.filterCoeffs.setPalette(p)
        self.filterCoeffs.setTextColor(QtGui.QColor("#"+self._color))

        self.setMovable(True)

    def update(self, filterInfo):
        filterInfo.calcfft(filterInfo.samp_rate, filterInfo.taps(),
                           max(filterInfo.ntaps(), 10000))
        self.plotFreq.setData(filterInfo.freq, filterInfo.fftdB)

        # Call for complex FIR filters
        if isinstance(filterInfo.taps()[0], complex):
            self.plotTime.setData(range(filterInfo.ntaps()),
                                  scipy.real(filterInfo.taps()))
            self.plotTimeIm.setData(range(filterInfo.ntaps()),
                                    scipy.imag(filterInfo.taps()))
            self.plotImpulseResponse.setData(range(filterInfo.ntaps()),
                                             scipy.real(filterInfo.impulse))
            self.plotImpulseResponseIm.setData(range(filterInfo.ntaps()),
                                               scipy.imag(filterInfo.impulse))
            self.plotStepResponse.setData(range(filterInfo.ntaps()),
                                          scipy.real(filterInfo.step))
            self.plotStepResponseIm.setData(range(filterInfo.ntaps()),
                                            scipy.imag(filterInfo.step))
            self.filterCoeffs.setText("taps = " + str(filterInfo.taps()))

            alpha = "88"
            self.plotTimeIm.setPen({'color': self._color+alpha, 'width': self._width,
                                    'style': QtCore.Qt.DashLine})
            self.plotImpulseResponseIm.setPen({'color': self._color+alpha, 'width': self._width,
                                               'style': QtCore.Qt.DashLine})
            self.plotStepResponseIm.setPen({'color': self._color+alpha, 'width': self._width,
                                            'style': QtCore.Qt.DashLine})

        # Call for IIR filters
        elif isinstance(filterInfo.taps()[0], list):
            self.plotTime.setData(range(filterInfo.ntaps()), filterInfo.taps()[0])
            self.plotTimeIm.setData(range(filterInfo.ntaps()), filterInfo.taps()[1])

            self.plotImpulseResponse.setData(range(len(filterInfo.impulse)), filterInfo.impulse)
            self.plotStepResponse.setData(range(len(filterInfo.step)), filterInfo.step)
            self.filterCoeffs.setText("b = " + str(filterInfo.taps()[0]) + "\n" +
                                      "a = " + str(filterInfo.taps()[1]))

            alpha = "88"
            self.plotTimeIm.setPen({'color': self._color+alpha, 'width': self._width,
                                    'style': QtCore.Qt.DashLine})

        # Call for regular FIR filters
        else:
            self.plotTime.setData(range(filterInfo.ntaps()), filterInfo.taps())
            self.plotImpulseResponse.setData(range(filterInfo.ntaps()), filterInfo.impulse)
            self.plotStepResponse.setData(range(filterInfo.ntaps()), filterInfo.step)
            self.filterCoeffs.setText("taps = " + str(filterInfo.taps()))

        self.plotPhase.setData(filterInfo.freq, filterInfo.fftDeg)
        self.plotGroupDel.setData(filterInfo.freq[1:], filterInfo.groupDelay)
        self.plotPhaseDel.setData(filterInfo.freq[1:], filterInfo.phaseDelay)

    def tabToggle(self, enabled):
        sendername = self.sender().text()
        if enabled:
            self.addTab(self.tabwidgets[sendername], sendername)
        else:
            index = self.indexOf(self.tabwidgets[sendername])
            self.removeTab(index)

    def getWidth(self):
        return self._width

    def setWidth(self, value):
        self._width = value
        self.updateStyle()

    def getColor(self):
        return self._color

    def setColor(self, value):
        self._color = str(value)
        self.updateStyle()

    def updateStyle(self):
        self.plotFreq.setPen({'color': self._color, "width": self._width})
        self.plotTime.setPen({'color': self._color, "width": self._width})
        self.plotTimeIm.setPen({'color': "000000", "width": self._width})
        self.plotPhase.setPen({'color': self._color, "width": self._width})
        self.plotGroupDel.setPen({'color': self._color, "width": self._width})
        self.plotPhaseDel.setPen({'color': self._color, "width": self._width})
        self.plotImpulseResponse.setPen({'color': self._color, "width": self._width})
        self.plotImpulseResponseIm.setPen({'color': "000000", "width": self._width})
        self.plotStepResponse.setPen({'color': self._color, "width": self._width})
        self.plotStepResponseIm.setPen({'color': "000000", "width": self._width})
        self.filterCoeffs.setTextColor(QtGui.QColor("#"+self._color))

        for i in self.tablabels:
            plot = self.tabwidgets[i]
            if type(plot) is QtGui.QTextEdit:
                continue
            plot.getAxis('left').setPen({'color': self._color, "width": self._width})
            plot.getAxis('bottom').setPen({'color': self._color, "width": self._width})

        super(StandardPlots, self).update()

    width = pyqtProperty(int, fget=getWidth, fset=setWidth)
    color = pyqtProperty(str, fget=getColor, fset=setColor)


class DesignPlots(QtGui.QTabWidget):
    def __init__(self, parent=None):
        super(DesignPlots, self).__init__(parent)

        labelstyle = {'font-size': '32px'}
        self._width = 1
        self._symsize = 16
        self._color = "FF6905"
        self._border_color = "888888"

        self.pzwidget = pg.PlotWidget()
        self.pzwidget.getAxis('left').setPen({'color': self._color, "width": self._width})
        self.pzwidget.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.pzplot_zeros = self.pzwidget.plot(range(0), range(0),
                                               pen=None, symbol='o',
                                               symbolPen=pg.mkPen({'color': self._border_color, 'width': 2}),
                                               symbolBrush=pg.mkBrush(self._color),
                                               symbolSize=self._symsize)
        self.pzplot_poles = self.pzwidget.plot(range(0), range(0),
                                               pen=None, symbol='x',
                                               symbolPen=pg.mkPen({'color': self._border_color, 'width': 2}),
                                               symbolBrush=pg.mkBrush(self._color),
                                               symbolSize=self._symsize)
        self.pzwidget.setXRange(-3, 3)
        self.pzwidget.setYRange(-2, 2)
        self.addTab(self.pzwidget, "Pole-Zero Plot")

        self.vLine = pg.InfiniteLine(angle=90, movable=False)
        self.hLine = pg.InfiniteLine(angle=0, movable=False)
        self.pzwidget.addItem(self.vLine, ignoreBounds=True)
        self.pzwidget.addItem(self.hLine, ignoreBounds=True)
        self.pzwidget.scene().sigMouseMoved.connect(self.mouseMoved)

        self.bdwidget = pg.PlotWidget()
        self.bdwidget.getAxis('left').setPen({'color': self._color, "width": self._width})
        self.bdwidget.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.bdplot = self.bdwidget.plot(range(0), range(0))
        self.bdplot.setPen({'color': self._color, "width": self._width})
        self.addTab(self.bdwidget, "Band Diagram")

        self.setMovable(True)

    def mouseMoved(self, evt):
        vb = self.pzwidget.plotItem.vb
        if self.pzwidget.plotItem.sceneBoundingRect().contains(evt):
            mousePoint = vb.mapSceneToView(evt)
            self.vLine.setPos(mousePoint.x())
            self.hLine.setPos(mousePoint.y())

    def update(self, filterInfo):
        self.zeros = filterInfo.get_zeros()
        self.poles = filterInfo.get_poles()
        self.pzplot_zeros.setData(scipy.real(self.zeros), scipy.imag(self.zeros))
        if(filterInfo.iir):
            self.pzplot_poles.setData(scipy.real(self.poles), scipy.imag(self.poles))
        else:
            self.pzplot_poles.setData(range(0), range(0))

    def getWidth(self):
        return self._width

    def setWidth(self, value):
        self._width = value
        self.updateStyle()

    def getColor(self):
        return self._color

    def setColor(self, value):
        self._color = str(value)
        self.updateStyle()

    def getBorderColor(self):
        return self._border_color

    def setBorderColor(self, value):
        self._border_color = str(value)
        self.updateStyle()

    def getSymbolSize(self):
        return self._symsize

    def setSymbolSize(self, value):
        self._symsize = value
        self.updateStyle()

    def updateStyle(self):
        self.pzwidget.getAxis('left').setPen({'color': self._color, "width": self._width})
        self.pzwidget.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.bdwidget.getAxis('left').setPen({'color': self._color, "width": self._width})
        self.bdwidget.getAxis('bottom').setPen({'color': self._color, "width": self._width})
        self.bdplot.setPen({'color': self._color, "width": self._width})

        self.pzplot_zeros.setSymbolPen(pg.mkPen(self._border_color))
        self.pzplot_zeros.setSymbolBrush(pg.mkBrush(self._color))
        self.pzplot_zeros.setSymbolSize(self._symsize)
        self.pzplot_poles.setSymbolPen(pg.mkPen(self._border_color))
        self.pzplot_poles.setSymbolBrush(pg.mkBrush(self._color))
        self.pzplot_poles.setSymbolSize(self._symsize)


    width = pyqtProperty(int, fget=getWidth, fset=setWidth)
    symbol_color = pyqtProperty(str, fget=getColor, fset=setColor)
    symbol_border = pyqtProperty(str, fget=getBorderColor, fset=setBorderColor)
    symbol_size = pyqtProperty(int, fget=getSymbolSize, fset=setSymbolSize)

class plots(QtGui.QGroupBox):
    def __init__(self, parent=None):
        super(plots, self).__init__(parent)

        self.vlayout = QtGui.QVBoxLayout(self)

        self.stdplots = StandardPlots()
        self.desplots = DesignPlots()
        self.vlayout.addWidget(self.stdplots)
        self.vlayout.addWidget(self.desplots)

    def update(self, filterInfo):
        self.stdplots.update(filterInfo)
        self.desplots.update(filterInfo)
