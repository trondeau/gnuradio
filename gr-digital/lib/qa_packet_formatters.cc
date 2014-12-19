/* -*- c++ -*- */
/*
 * Copyright 2002,2013 Free Software Foundation, Inc.
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

#include <gnuradio/attributes.h>

#include <stdio.h>
#include <cmath>
#include <volk/volk.h>
#include <gnuradio/expj.h>
#include <cppunit/TestAssert.h>

#include "qa_packet_formatters.h"
#include <gnuradio/digital/packet_formatter_default.h>
#include <gnuradio/blocks/unpack_k_bits.h>

void
qa_packet_formatters::test_default_format()
{
  static const int N = 4800;
  int upper8 = (N >> 8) & 0xFF;
  int lower8 = N & 0xFF;

  std::string ac = "1010101010101010"; //0xAAAA
  unsigned char *data = (unsigned char*)volk_malloc(N*sizeof(unsigned char),
                                                    volk_get_alignment());
  srand (time(NULL));
  for(unsigned int i = 0; i < N; i++) {
    data[i] = rand() % 256;
  }

  gr::digital::packet_formatter_default::sptr formatter;
  formatter = gr::digital::packet_formatter_default::make(ac);

  pmt::pmt_t output;
  pmt::pmt_t info = pmt::make_dict();

  bool ret = formatter->format(N, data, output, info);
  size_t length = pmt::length(output);

  CPPUNIT_ASSERT(ret);
  CPPUNIT_ASSERT_EQUAL(length, formatter->header_nbytes());
  CPPUNIT_ASSERT_EQUAL(8*length, formatter->header_nbits());

  // Test access code formatted correctly
  unsigned char h0 = pmt::u8vector_ref(output, 0);
  unsigned char h1 = pmt::u8vector_ref(output, 1);
  CPPUNIT_ASSERT_EQUAL(0xAA, (int)h0);
  CPPUNIT_ASSERT_EQUAL(0xAA, (int)h1);

  // Test upper and lower portion of length field, repeated twice
  unsigned char h2 = pmt::u8vector_ref(output, 2);
  unsigned char h3 = pmt::u8vector_ref(output, 3);
  unsigned char h4 = pmt::u8vector_ref(output, 4);
  unsigned char h5 = pmt::u8vector_ref(output, 5);
  CPPUNIT_ASSERT_EQUAL(upper8, (int)h2);
  CPPUNIT_ASSERT_EQUAL(lower8, (int)h3);
  CPPUNIT_ASSERT_EQUAL(upper8, (int)h4);
  CPPUNIT_ASSERT_EQUAL(lower8, (int)h5);

  volk_free(data);
}


void
qa_packet_formatters::test_default_parse()
{
  static const int nbytes = 106;
  static const int nbits = 8*nbytes;
  unsigned char *bytes = (unsigned char*)volk_malloc(nbytes*sizeof(unsigned char),
                                                     volk_get_alignment());
  unsigned char *bits = (unsigned char*)volk_malloc(nbits*sizeof(unsigned char),
                                                    volk_get_alignment());

  srand(time(NULL));

  // Fill bytes with random values
  for(unsigned int i = 0; i < nbytes; i++) {
    bytes[i] = rand() % 256;
  }

  // Find a random place to insert the 6-byte header
  int index = rand() % (nbytes - 6);

  bytes[index+0] = 0xAA;
  bytes[index+1] = 0xAA;
  bytes[index+2] = 0x00;
  bytes[index+3] = 0x64;
  bytes[index+4] = 0x00;
  bytes[index+5] = 0x64;

  gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
  unpacker.unpack(bits, bytes, nbytes);

  std::string ac = "1010101010101010"; //0xAAAA
  gr::digital::packet_formatter_default::sptr formatter;
  formatter = gr::digital::packet_formatter_default::make(ac);
  formatter->set_threshold(0);

  std::vector<pmt::pmt_t> info;
  bool ret = formatter->parse(nbits, bits, info);

  CPPUNIT_ASSERT(ret);
  CPPUNIT_ASSERT_EQUAL(info.size(), (size_t)1);

  pmt::pmt_t dict = info[0];
  int payload_bits = pmt::to_long(pmt::dict_ref(dict, pmt::intern("payload bits"),
                                                pmt::PMT_NIL));
  int skip_samps = pmt::to_long(pmt::dict_ref(dict, pmt::intern("skip samps"),
                                              pmt::PMT_NIL));

  int hdr_bits = (int)formatter->header_nbits();
  int expected_bits = nbits - hdr_bits;
  int expected_skip = index * 8 + hdr_bits;
  CPPUNIT_ASSERT_EQUAL(payload_bits, expected_bits);
  CPPUNIT_ASSERT_EQUAL(skip_samps, expected_skip);

  volk_free(bytes);
  volk_free(bits);
}

void
qa_packet_formatters::test_default_parse_soft()
{
  static const int nbytes = 106;
  static const int nbits = 8*nbytes;
  unsigned char *bytes = (unsigned char*)volk_malloc(nbytes*sizeof(unsigned char),
                                                     volk_get_alignment());
  unsigned char *bits = (unsigned char*)volk_malloc(nbits*sizeof(unsigned char),
                                                    volk_get_alignment());
  float *soft = (float*)volk_malloc(nbits*sizeof(float),
                                    volk_get_alignment());

  srand(time(NULL));

  // Fill bytes with random values
  for(unsigned int i = 0; i < nbytes; i++) {
    bytes[i] = rand() % 256;
  }

  // Find a random place to insert the 6-byte header
  int index = rand() % (nbytes - 6);

  bytes[index+0] = 0xAA;
  bytes[index+1] = 0xAA;
  bytes[index+2] = 0x00;
  bytes[index+3] = 0x64;
  bytes[index+4] = 0x00;
  bytes[index+5] = 0x64;

  gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
  unpacker.unpack(bits, bytes, nbytes);

  // Convert bits to +/-1 and add a small bit of noise
  std::vector<float> sub(nbits, 1.0f);
  volk_8i_s32f_convert_32f(soft, (const int8_t*)bits, 1.0, nbits);
  volk_32f_s32f_multiply_32f(soft, soft, 2.0, nbits);
  volk_32f_x2_subtract_32f(soft, soft, &sub[0], nbits);
  for(unsigned int i = 0; i < nbytes; i++) {
    soft[i] += 0.1 * ((float)rand() / (float)RAND_MAX - 0.5);
  }

  std::string ac = "1010101010101010"; //0xAAAA
  gr::digital::packet_formatter_default::sptr formatter;
  formatter = gr::digital::packet_formatter_default::make(ac);
  formatter->set_threshold(0);

  std::vector<pmt::pmt_t> info;
  bool ret = formatter->parse_soft(nbits, soft, info);

  CPPUNIT_ASSERT(ret);
  CPPUNIT_ASSERT_EQUAL(info.size(), (size_t)1);

  pmt::pmt_t dict = info[0];
  int payload_bits = pmt::to_long(pmt::dict_ref(dict, pmt::intern("payload bits"),
                                                pmt::PMT_NIL));
  int skip_samps = pmt::to_long(pmt::dict_ref(dict, pmt::intern("skip samps"),
                                              pmt::PMT_NIL));

  int hdr_bits = (int)formatter->header_nbits();
  int expected_bits = nbits - hdr_bits;
  int expected_skip = index * 8 + hdr_bits;
  CPPUNIT_ASSERT_EQUAL(payload_bits, expected_bits);
  CPPUNIT_ASSERT_EQUAL(skip_samps, expected_skip);

  volk_free(bytes);
  volk_free(bits);
}
