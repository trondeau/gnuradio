/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

//#pragma SWIG nowarn=401

%include "std_string.i"
%include "stdint.i"

%{
#include <gnuradio/time.h>
%}

namespace gr {

  class grtime_t
  {
  public:
    grtime_t();
    grtime_t(long int secs, double fracs=0);
    grtime_t(const grtime_t &t);
    ~grtime_t();

    long int secs() const;
    double fracs() const;

    grtime_t operator+(const grtime_t &t);
    grtime_t &operator+=(const grtime_t &t);
    grtime_t operator-(const grtime_t &t);
    grtime_t &operator-=(const grtime_t &t);
    grtime_t operator*(double x);
    grtime_t &operator*=(double x);
    friend grtime_t operator*(double x, grtime_t t);
    bool operator<(const grtime_t &t) const;
    bool operator<=(const grtime_t &t) const;
    bool operator>(const grtime_t &t) const;
    bool operator>=(const grtime_t &t) const;
    bool operator==(const grtime_t &t) const;
    std::string write() const;
  };

#ifdef SWIGPYTHON
  // Make easily printable in Python
  %pythoncode %{
    grtime_t.__repr__ = lambda self: "%s" % (self.write())
  %}
#endif

} /* namespace gr */
