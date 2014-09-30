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
#include <config.h>
#endif

#include <qa_time.h>
#include <gnuradio/time.h>

void
qa_time::t0()
{
  gr::grtime_t t1(1, 0.123);
  gr::grtime_t t_test(1, 0.123);

  gr::grtime_t t_res0(t1);
  gr::grtime_t t_res1;

  t_res1 = t1;

  CPPUNIT_ASSERT_EQUAL(t_res0, t_test);
  CPPUNIT_ASSERT_EQUAL(t_res1, t_test);
}

void
qa_time::t1()
{
  gr::grtime_t t1(1, 0.123);
  gr::grtime_t t_test(2, 0.123);
  gr::grtime_t t_res0(1, 0);
  gr::grtime_t t_res1(1, 0);

  t_res0 += t1;
  t_res1  = t_res1 + t1;

  CPPUNIT_ASSERT_EQUAL(t_res0, t_test);
  CPPUNIT_ASSERT_EQUAL(t_res1, t_test);
}

void
qa_time::t2()
{
  gr::grtime_t t1(0, 0.123);
  gr::grtime_t t_test(0, 1.0 - 0.123);
  gr::grtime_t t_res0(1, 0);
  gr::grtime_t t_res1(1, 0);

  t_res0 -= t1;
  t_res1 = t_res1 - t1;

  CPPUNIT_ASSERT_EQUAL(t_res0, t_test);
  CPPUNIT_ASSERT_EQUAL(t_res1, t_test);
}

void
qa_time::t3()
{
  gr::grtime_t t1(1, 0.123);
  gr::grtime_t t2(1, 0.001);
  gr::grtime_t t3(1, 0.123);
  gr::grtime_t t4(1, 0.231);

  bool res0, res1, res2, res3;

  res0 = t2 < t1;  // true
  res1 = t3 <= t1; // true
  res2 = t4 < t1;  // false
  res3 = t4 <= t1; // false

  CPPUNIT_ASSERT_EQUAL(res0, true);
  CPPUNIT_ASSERT_EQUAL(res1, true);
  CPPUNIT_ASSERT_EQUAL(res2, false);
  CPPUNIT_ASSERT_EQUAL(res3, false);
}

void
qa_time::t4()
{
  gr::grtime_t t1(1, 0.123);
  gr::grtime_t t2(1, 0.001);
  gr::grtime_t t3(1, 0.123);
  gr::grtime_t t4(1, 0.231);

  bool res0, res1, res2, res3;

  res0 = t2 > t1;  // false
  res1 = t3 >= t1; // true
  res2 = t4 > t1;  // true
  res3 = t4 >= t1; // true

  CPPUNIT_ASSERT_EQUAL(res0, false);
  CPPUNIT_ASSERT_EQUAL(res1, true);
  CPPUNIT_ASSERT_EQUAL(res2, true);
  CPPUNIT_ASSERT_EQUAL(res3, true);
}


void
qa_time::t5()
{
  gr::grtime_t t1(1, 0.123);
  gr::grtime_t t2(1, 0.001);
  gr::grtime_t t3(1, 0.123);

  bool res0, res1;

  res0 = t2 == t1;  // false
  res1 = t3 == t1;  // true

  CPPUNIT_ASSERT_EQUAL(res0, false);
  CPPUNIT_ASSERT_EQUAL(res1, true);
}

void
qa_time::t6()
{
  // Time struct outputs 15 decimals of precision.
  gr::grtime_t t(1234567890123456789, 0.01234567890123456789);
  std::string test = "1234567890123456789.0123456789012346";
  std::string res = t.write();

  CPPUNIT_ASSERT(test == res);
}


void
qa_time::t7()
{
  double x = 0.876;
  long int secs = 2;
  double fracs = 0.123;
  gr::grtime_t t1(secs, fracs);
  gr::grtime_t t_res0, t_res1;
  gr::grtime_t t_res2 = t1;
  gr::grtime_t t_test(1, 0.859748);

  t_res0 = x*t1;
  t_res1 = t1*x;
  t_res2 *= x;

  CPPUNIT_ASSERT_EQUAL(t_res0, t_test);
  CPPUNIT_ASSERT_EQUAL(t_res1, t_test);
  CPPUNIT_ASSERT_EQUAL(t_res2, t_test);
}
