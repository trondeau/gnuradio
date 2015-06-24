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

from PyQt5 import QtWidgets, QtGui
from gnuradio import eng_notation
import sys, warnings

from gnuradio.filter import firdes, optfir

try:
    import scipy
    from scipy import fftpack, poly1d, signal
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1

EQUIRIPPLE = 1000

filterTypesDict = {
    "Low Pass": 0,
    "Band Pass": 1,
    "Complex Band Pass": 2,
    "Band Notch": 3,
    "High Pass": 4,
    "Root Raised Cosine": 5,
    "Gaussian": 6,
    "Half Band": 7
}

filterParamTypesDict = {
    "lpf": "Low Pass" ,
    "bpf": "Band Pass",
    "cbpf": "Complex Band Pass",
    "bnf": "Band Notch",
    "hpf": "High Pass",
    "rrc": "Root Raised Cosine",
    "gaus": "Gaussian",
    "hb": "Half Band"
}

windowTypeDict = {
    "Hann": firdes.WIN_HANN,
    "Hamming": firdes.WIN_HAMMING,
    "Blackman": firdes.WIN_BLACKMAN,
    "Blackman-harris": firdes.WIN_BLACKMAN_HARRIS,
    "Kaiser": firdes.WIN_KAISER,
    "Rectangular": firdes.WIN_RECTANGULAR,
    "Bartlett": firdes.WIN_BARTLETT,
    "Equiripple": EQUIRIPPLE,
}

windowTypeDictRev = {
    firdes.WIN_HANN: "Hann",
    firdes.WIN_HAMMING: "Hamming",
    firdes.WIN_BLACKMAN: "Blackman",
    firdes.WIN_BLACKMAN_HARRIS: "Blackman-harris",
    firdes.WIN_KAISER: "Kaiser",
    firdes.WIN_RECTANGULAR: "Rectangular",
    firdes.WIN_BARTLETT: "Bartlett",
    EQUIRIPPLE: "Equiripple",
}

iirTypes = {
    "Elliptic": "ellip",
    "Butterworth": "butter",
    "Chebyshev-1": "cheby1",
    "Chebyshev-2": "cheby2",
    "Bessel": "bessel",
}


class GrLineEdit(QtWidgets.QLineEdit):
    def __init__(self, text):
        super(GrLineEdit, self).__init__("")
        self.setText(text)
        validator = QtGui.QDoubleValidator(self)
        validator.setBottom(0)
        super(GrLineEdit, self).setValidator(validator)

    def setText(self, text):
        n = eng_notation.str_to_num(text)
        super(GrLineEdit, self).setText(eng_notation.num_to_str(n))

    def text(self):
        text = super(GrLineEdit, self).text()
        return eng_notation.str_to_num(text)

class Filter:
    def __init__(self, params=None):
        if(params):
            self.set_params(params)
            self._taps = []

        else:
            self.gain = 0
            self.samp_rate = 0
            self.window = ""
            self.win_beta = 0
            self.filttype = ""
            self._taps = []
            self._ntaps = 0

            self.start_passband = 0
            self.stop_passband = 0
            self.start_stopband = 0
            self.stop_stopband = 0
            self.transition_band = 0
            self.atten = 0
            self.ripple = 0

            # Used for pulse shaping filters
            self.symbol_rate = 0
            self.rolloff = 0

            self.filt_params = {}

    def set_params(self, params):
        self.filt_params = params

        if params.has_key('gain'):
            self.gain = params['gain']

        if params.has_key('fs'):
            self.samp_rate = params['fs']

        if params.has_key('wintype'):
            self.window = params['wintype']

        if params.has_key('winbeta'):
            self.win_beta = params['winbeta']

        if params.has_key('pbstart'):
            self.start_passband = params['pbstart']

        if params.has_key('pbend'):
            self.stop_passband = params['pbend']

        if params.has_key('sbstart'):
            self.start_stopband = params['sbstart']

        if params.has_key('sbend'):
            self.stop_stopband = params['sbend']

        if params.has_key('tb'):
            self.transition_band = params['tb']

        if params.has_key('atten'):
            self.atten = params['atten']

        if params.has_key('ripple'):
            self.ripple = params['ripple']

        if params.has_key('srate'):
            self.ripple = params['srate']

        if params.has_key('rolloff'):
            self.ripple = params['rolloff']

        if params.has_key('ntaps'):
            self._ntaps = params['ntaps']

        if params.has_key('filttype'):
            # FIXME: protect this call
            self.filttype = filterParamTypesDict[params['filttype']]

    def params(self):
        return self.filt_params

    def set_taps(self, t):
        self._taps = list(t)
        self._ntaps = len(t)

    def taps(self):
        if not isinstance(self._taps[0], scipy.ndarray):
            return list(self._taps)
        else:
            return (list(self._taps[0]), list(self._taps[1]))

    def ntaps(self):
        return self._ntaps

    def calcfft(self, fs, taps, Npts):
        if not isinstance(self._taps[0], scipy.ndarray):
            self.calcfft_fir(fs, taps, Npts)
            self.impulse_response(taps)
            self.step_response(taps)
        else:
            self.calcfft_iir(fs, taps, Npts)
            self.impulse_response(taps[0], taps[1])
            self.step_response(taps[0], taps[1])

    def calcfft_fir(self, fs, taps, Npts):
        Ts = 1.0/fs

        fftpts = fftpack.fft(taps, Npts)
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            self.fftdB = 20.0*scipy.log10(abs(fftpts))
            if any(self.fftdB == float('-inf')):
                sys.stderr.write('Filter design failed (taking log10 of 0).\n')
                self.fftdB = scipy.zeros([len(fftpts)])

        # If complex, shift and display both pos and neg halves
        if isinstance(taps[0], complex):
            fftpts = fftpack.fftshift(fftpts)
            self.fftdB = fftpack.fftshift(self.fftdB)
            self.freq = scipy.arange(-fs/2.0, fs/2.0, 1.0/(Npts*Ts))
            self.fftdB = self.fftdB

        # else, only take and show the positive half
        else:
            self.freq = scipy.arange(0, fs, 1.0/(Npts*Ts))
            self.freq = self.freq[0:Npts/2]
            self.fftdB = self.fftdB[0:Npts/2]
            fftpts = fftpts[0:Npts/2]

        self.fftDeg = scipy.unwrap(scipy.angle(fftpts))
        self.groupDelay = -scipy.diff(self.fftDeg)
        self.phaseDelay = -self.fftDeg[1:]/self.freq[1:]


    def calcfft_iir(self, fs, taps, Npts):
        w,h = signal.freqz(taps[0], taps[1])
        self.fftdB = 20 * scipy.log10 (abs(h))
        self.freq = w/max(w)
        self.fftDeg = scipy.unwrap(scipy.arctan2(scipy.imag(h),scipy.real(h)))
        self.groupDelay = -scipy.diff(self.fftDeg)
        self.phaseDelay = -self.fftDeg[1:]/self.freq[1:]

        (self.z,self.p,self.k) = signal.tf2zpk(taps[0], taps[1])

    def get_zeros(self):
        if self.iir:
            return self.z
        else:
            hz = poly1d(self._taps, r=0)
            return hz.r

    def get_poles(self):
        if self.iir:
            return self.p
        else:
            if len(self._taps):
                hp = scipy.zeros(len(self._taps)-1, complex)
                return hp
            else:
                return []

    def impulse_response(self, b, a=1):
        l = len(b)
        if self.iir:
            l = 50
        impulse = scipy.repeat(0.0, l)
        impulse[0] = 1.0
        x = scipy.arange(0, l)
        self.impulse = signal.lfilter(b, a, impulse).tolist()

    def step_response(self, b, a=1):
        l = len(b)
        if self.iir:
            l = 50
        impulse = scipy.repeat(0.0, l)
        impulse[0] = 1.0
        x = scipy.arange(0, l)
        response = signal.lfilter(b, a, impulse)
        self.step = scipy.cumsum(response)

    def low_pass(self):
        self.iir = False
        if(self.window != EQUIRIPPLE):
            self._taps = firdes.low_pass_2(self.gain,
                                           self.samp_rate,
                                           self.stop_passband,
                                           self.start_stopband - self.stop_passband,
                                           self.atten,
                                           self.window,
                                           self.win_beta)
        else:
            self._taps = optfir.low_pass(self.gain,
                                         self.samp_rate,
                                         self.stop_passband,
                                         self.start_stopband,
                                         self.ripple,
                                         self.atten)

        self._ntaps = len(self._taps)
        self.filt_params = {
            'filttype': 'lpf',
            'fs': self.samp_rate,
            'gain': self.gain,
            'pbend': self.stop_passband,
            'sbstart': self.start_stopband,
            'atten': self.atten,
            'wintype': self.window,
            'winbeta': self.win_beta,
            'ntaps': self.ntaps()
        }

        if(self.window == EQUIRIPPLE):
            self.filt_params['ripple'] = self.ripple


    def high_pass(self):
        self.iir = False
        self._taps = firdes.high_pass_2(self.gain,
                                        self.samp_rate,
                                        self.start_passband,
                                        self.start_passband - self.stop_stopband,
                                        self.atten,
                                        self.window,
                                        self.win_beta)
        self._ntaps = len(self._taps)
        self.filt_params = {
            'filttype': 'hpf',
            'fs': self.samp_rate,
            'gain': self.gain,
            'sbend': self.stop_stopband,
            'pbstart': self.start_passband,
            'atten': self.atten,
            'wintype': self.window,
            'winbeta': self.win_beta,
            'ntaps': self.ntaps()
        }

        if(self.window == EQUIRIPPLE):
            self.filt_params['ripple'] = self.ripple

    def band_pass(self):
        self.iir = False
        self._taps = firdes.band_pass_2(self.gain,
                                       self.samp_rate,
                                       self.start_passband,
                                       self.stop_passband,
                                       self.transition_band,
                                       self.atten,
                                       self.window,
                                       self.win_beta)
        self._ntaps = len(self._taps)
        self.filt_params = {
            'filttype': 'bpf',
            'fs': self.samp_rate,
            'gain': self.gain,
            'pbstart': self.start_passband,
            'pbend': self.stop_passband,
            'tb': self.transition_band,
            'atten': self.atten,
            'wintype': self.window,
            'winbeta': self.win_beta,
            'ntaps': self.ntaps()
        }

        if(self.window == EQUIRIPPLE):
            self.filt_params['ripple'] = self.ripple

    def complex_band_pass(self):
        self.iir = False
        self._taps = firdes.complex_band_pass_2(self.gain,
                                                self.samp_rate,
                                                self.start_passband,
                                                self.stop_passband,
                                                self.transition_band,
                                                self.atten,
                                                self.window,
                                                self.win_beta)
        self._ntaps = len(self._taps)
        self.filt_params = {
            'filttype': 'cbpf',
            'fs': self.samp_rate,
            'gain': self.gain,
            'pbstart': self.start_passband,
            'pbend': self.stop_passband,
            'tb': self.transition_band,
            'atten': self.atten,
            'wintype': self.window,
            'winbeta': self.win_beta,
            'ntaps': self.ntaps()
        }

        if(self.window == EQUIRIPPLE):
            self.filt_params['ripple'] = self.ripple

    def band_notch(self):
        self.iir = False
        self._taps = firdes.band_reject_2(self.gain,
                                          self.samp_rate,
                                          self.stop_passband,
                                          self.start_passband,
                                          self.transition_band,
                                          self.atten,
                                          self.window,
                                          self.win_beta)
        self._ntaps = len(self._taps)
        self.filt_params = {
            'filttype': 'bpf',
            'fs': self.samp_rate,
            'gain': self.gain,
            'pbend': self.stop_passband,
            'pbstart': self.start_passband,
            'tb': self.transition_band,
            'atten': self.atten,
            'wintype': self.window,
            'winbeta': self.win_beta,
            'ntaps': self.ntaps()
        }

        if(self.window == EQUIRIPPLE):
            self.filt_params['ripple'] = self.ripple

    def rrc(self):
        self.iir = False
        self._taps = firdes.root_raised_cosine(self.gain,
                                               self.samp_rate,
                                               self.symbol_rate,
                                               self.rolloff,
                                               int(self._ntaps))
        self.filt_params = {
            'filttype': 'rrc',
            'fs': self.samp_rate,
            'gain': self.gain,
            'srate': self.symbol_rate,
            'rolloff': self.rolloff,
            'ntaps': self.ntaps()
        }

    def gaussian(self):
        self.iir = False
        self._taps = firdes.gaussian(self.gain,
                                     self.samp_rate/self.symbol_rate,
                                     self.rolloff,
                                     int(self._ntaps))
        self.filt_params = {
            'filttype': 'gaus',
            'fs': self.samp_rate,
            'gain': self.gain,
            'srate': self.symbol_rate,
            'rolloff': self.rolloff,
            'ntaps': self.ntaps()
        }

    def iir_low_pass(self, wtype):
        self.iir = True
        self._taps = signal.iirdesign(self.stop_passband,
                                      self.start_stopband,
                                      self.ripple,
                                      self.atten,
                                      analog=False,
                                      ftype=wtype,
                                      output='ba')
        (z,p,k) = signal.tf2zpk(self._taps[0], self._taps[1])

        self._ntaps = len(self._taps[0])

        self.filt_params = {
            'bandtype': 'lpf',
            'paramtype': 'digital',
            'filttype': wtype,
            'gstop': self.atten,
            'pbedge': self.stop_passband,
            'sbedge': self.start_stopband,
            'gpass': self.gain,
            'ripple': self.ripple,
            'fs': 1,
            'ntaps': self.ntaps()
        }

    def iir_high_pass(self, wtype):
        self.iir = True
        self._taps = signal.iirdesign(self.stop_stopband,
                                      self.start_passband,
                                      self.ripple,
                                      self.atten,
                                      analog=False,
                                      ftype=wtype,
                                      output='ba')
        (z,p,k) = signal.tf2zpk(self._taps[0], self._taps[1])

        self._ntaps = len(self._taps[0])

        self.filt_params = {
            'bandtype': 'hpf',
            'paramtype': 'digital',
            'filttype': wtype,
            'gstop': self.atten,
            'pbedge': self.start_passband,
            'sbedge': self.stop_stopband,
            'gpass': self.gain,
            'ripple': self.ripple,
            'fs': 1,
            'ntaps': self.ntaps()
        }

    def iir_band_pass(self, wtype):
        self.iir = True
        self._taps = signal.iirdesign([self.start_passband, self.stop_passband],
                                      [self.stop_stopband, self.start_stopband],
                                      self.ripple,
                                      self.atten,
                                      analog=False,
                                      ftype=wtype,
                                      output='ba')
        (z,p,k) = signal.tf2zpk(self._taps[0], self._taps[1])

        self._ntaps = len(self._taps[0])

        self.filt_params = {
            'bandtype': 'bpf',
            'paramtype': 'digital',
            'filttype': wtype,
            'gstop': self.atten,
            'pbedge': [self.start_passband, self.stop_passband],
            'sbedge': [self.stop_stopband, self.start_stopband],
            'gpass': self.gain,
            'ripple': self.ripple,
            'fs': 1,
            'ntaps': self.ntaps()
        }

    def iir_band_stop(self, wtype):
        self.iir = True
        self._taps = signal.iirdesign([self.stop_passband, self.start_passband],
                                      [self.start_stopband, self.stop_stopband],
                                      self.ripple,
                                      self.atten,
                                      analog=False,
                                      ftype=wtype,
                                      output='ba')
        (z,p,k) = signal.tf2zpk(self._taps[0], self._taps[1])

        self._ntaps = len(self._taps[0])

        self.filt_params = {
            'bandtype': 'bnf',
            'paramtype': 'digital',
            'filttype': wtype,
            'gstop': self.atten,
            'pbedge': [self.stop_passband, self.start_passband],
            'sbedge': [self.start_stopband, self.stop_stopband],
            'gpass': self.gain,
            'ripple': self.ripple,
            'fs': 1,
            'ntaps': self.ntaps()
        }
