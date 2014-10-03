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

#ifndef INCLUDED_GR_TIME_H
#define INCLUDED_GR_TIME_H

#include <gnuradio/api.h>
#include <iostream>
#include <cmath>

namespace gr {

  class GR_RUNTIME_API grtime_t
  {
  private:
    long int d_secs;
    double d_fracs;

    void check_overflow();
    void check_underflow();

  public:
    grtime_t();
    grtime_t(long int secs, double fracs=0);
    grtime_t(const grtime_t &t);
    ~grtime_t();

    long int secs() const;
    double fracs() const;

    //! Operator definition to add two grtime_t objects together
    grtime_t operator+(const grtime_t &t);

    //! Operator definition to add a grtime_t object to the current one
    grtime_t &operator+=(const grtime_t &t);

    //! Operator definition to subtract two grtime_t objects
    grtime_t operator-(const grtime_t &t);

    //! Operator definition to subtract two grtime_t objects
    grtime_t &operator-=(const grtime_t &t);

    //! Operator definition to multiply a grtime_t object by a scalar
    grtime_t operator*(double x);

    //! Operator definition to multiply a grtime_t object by a scalar (t*x)
    grtime_t &operator*=(double x);

    //! Define less than operator
    bool operator<(const grtime_t &t) const;

    //! Define less than or equal operator
    bool operator<=(const grtime_t &t) const;

    //! Define greater than operator
    bool operator>(const grtime_t &t) const;

    //! Define greater than or equal operator
    bool operator>=(const grtime_t &t) const;

    //! Define equal to operator
    bool operator==(const grtime_t &t) const;

    //! Return a printable string with the combine secs.fracs
    std::string write(int precision=15) const;

    //! Define output operator
    friend std::ostream& operator<<(std::ostream &os, const grtime_t &t)
    {
      os << t.write();
      return os;
    }

    //! Allows x*t version of multiplying by a scalar
    friend grtime_t operator*(double x, grtime_t t)
    {
      double secs = x*t.secs();
      double rm = fmod(secs, 1.0);
      double fracs = x*t.fracs() + rm;
      return grtime_t(static_cast<long int>(secs), fracs);
    }

  };

} /* namespace gr */

#endif /* INCLUDED_GR_TIME_H */
