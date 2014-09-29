/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Example Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Example Public License for more details.
 *
 * You should have received a copy of the GNU Example Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_QA_GR_TIME_H
#define INCLUDED_QA_GR_TIME_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class qa_time : public CppUnit::TestCase
{
public:
  CPPUNIT_TEST_SUITE(qa_time);
  CPPUNIT_TEST(t0);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST(t2);
  CPPUNIT_TEST(t3);
  CPPUNIT_TEST(t4);
  CPPUNIT_TEST(t5);
  CPPUNIT_TEST(t6);
  CPPUNIT_TEST(t7);
  CPPUNIT_TEST_SUITE_END();

 private:
  void t0(); // test copy/assignment
  void t1(); // test addition
  void t2(); // test subtraction
  void t3(); // test < and <=
  void t4(); // test > and >=
  void t5(); // test ==
  void t6(); // test write

  void t7(); // test multipliers
};

#endif /* INCLUDED_QA_GR_TIME_H */
