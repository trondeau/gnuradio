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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/time.h>
#include <sstream>
#include <iomanip>

namespace gr {

  grtime_t::grtime_t()
    : d_secs(0), d_fracs(0)
  {
  }

  grtime_t::grtime_t(long int secs, double fracs)
    : d_secs(secs), d_fracs(fracs)
  {
    check_overflow();
    check_underflow();
  }

  grtime_t::grtime_t(const grtime_t &t)
    : d_secs(t.d_secs), d_fracs(t.d_fracs)
  {
    check_overflow();
    check_underflow();
  }

  grtime_t::~grtime_t() { }

  long int
  grtime_t::secs() const
  {
    return d_secs;
  }

  double
  grtime_t::fracs() const
  {
    return d_fracs;
  }

  void
  grtime_t::check_overflow()
  {
    while(d_fracs > 1) {
      d_secs += 1;
      d_fracs -= 1;
    }
  }

  void
  grtime_t::check_underflow()
  {
    while(d_fracs < 0) {
      d_secs -= 1;
      d_fracs += 1;
    }
  }

  grtime_t
  grtime_t::operator+(const grtime_t &t)
  {
    long int secs = d_secs + t.secs();
    double fracs = d_fracs + t.fracs();
    return grtime_t(secs, fracs);
  }

  grtime_t &
  grtime_t::operator+=(const grtime_t &t)
  {
    d_secs = d_secs + t.secs();
    d_fracs = d_fracs + t.fracs();
    return *this;
  }

  grtime_t
  grtime_t::operator-(const grtime_t &t)
  {
    long int secs = d_secs - t.secs();
    double fracs = d_fracs - t.fracs();
    return grtime_t(secs, fracs);
  }

  grtime_t &
  grtime_t::operator-=(const grtime_t &t)
  {
    d_secs = d_secs - t.secs();
    d_fracs = d_fracs - t.fracs();
    check_underflow();
    return *this;
  }

  grtime_t
  grtime_t::operator*(double x)
  {
    long int secs = x*d_secs;
    double fracs = x*d_fracs;
    return grtime_t(secs, fracs);
  }

  grtime_t &
  grtime_t::operator*=(double x)
  {
    d_secs *= x;
    d_fracs *= x;
    check_overflow();
    return *this;
  }

  bool
  grtime_t::operator<(const grtime_t &t) const
  {
    if(d_secs < t.secs())
      return true;
    if(d_secs == t.secs()) {
      if(d_fracs < t.fracs())
        return true;
    }
    return false;
  }

  bool
  grtime_t::operator<=(const grtime_t &t) const
  {
    if(d_secs < t.secs())
      return true;
    if(d_secs == t.secs()) {
      if(d_fracs <= t.fracs())
        return true;
    }
    return false;
  }

  bool
  grtime_t:: operator>(const grtime_t &t) const
  {
    if(d_secs > t.secs())
      return true;
    if(d_secs == t.secs()) {
      if(d_fracs > t.fracs())
        return true;
    }
    return false;
  }

  bool
  grtime_t:: operator>=(const grtime_t &t) const
  {
    if(d_secs > t.secs())
      return true;
    if(d_secs == t.secs()) {
      if(d_fracs >= t.fracs())
        return true;
    }
    return false;
  }

  bool
  grtime_t::operator==(const grtime_t &t) const
  {
    if((d_secs == t.secs()) && (d_fracs == t.fracs()))
      return true;
    else
      return false;
  }

  std::string
  grtime_t::write() const
  {
    // Maybe a niceer way to display this with stripping off the
    // leading 0 of the fractional part.
    std::stringstream s;
    std::stringstream frac;
    frac << std::setprecision(15) << d_fracs;
    s << d_secs << frac.str().substr(1);
    return s.str();
  }

} /* namespace gr */
