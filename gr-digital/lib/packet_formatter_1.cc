/* -*- c++ -*- */
/* Copyright 2012,2014 Free Software Foundation, Inc.
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

#include <string.h>
#include <volk/volk.h>
#include <gnuradio/digital/packet_formatter_1.h>

namespace gr {
  namespace digital {

    packet_formatter_1::sptr
    packet_formatter_1::make(long header_len, int bits_per_byte)
    {
      return packet_formatter_1::sptr
        (new packet_formatter_1(header_len, bits_per_byte));
    }

    const unsigned MASK_LUT[9] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x2F, 0x7F, 0xFF};
    packet_formatter_1::packet_formatter_1(long header_len, int bits_per_byte)
      : packet_formatter_default(""),
        d_header_len(header_len),
        d_bits_per_byte(bits_per_byte),
        d_header_number(0)
    {
      if(d_bits_per_byte < 1 || d_bits_per_byte > 8) {
	throw std::invalid_argument("bits_per_byte must be in [1, 8]");
      }
      d_mask = MASK_LUT[d_bits_per_byte];
    }

    packet_formatter_1::~packet_formatter_1()
    {
    }

    bool
    packet_formatter_1::format(int nbytes_in,
                               const unsigned char *input,
                               pmt::pmt_t &output,
                               pmt::pmt_t &info)
    {
      nbytes_in &= 0x0FFF;
      d_crc_impl.reset();
      d_crc_impl.process_bytes((void const *) &nbytes_in, 2);
      d_crc_impl.process_bytes((void const *) &d_header_number, 2);
      unsigned char crc = d_crc_impl();

      size_t header_size = header_nbytes();
      uint8_t* bytes_out = (uint8_t*)volk_malloc(header_size*sizeof(uint8_t),
                                                 volk_get_alignment());

      int k = 0; // Position in out
      for(int i = 0; i < 12 && k < d_header_len; i += d_bits_per_byte, k++) {
	output[k] = (unsigned char) ((nbytes_in >> i) & d_mask);
      }
      for(int i = 0; i < 12 && k < d_header_len; i += d_bits_per_byte, k++) {
	output[k] = (unsigned char) ((d_header_number >> i) & d_mask);
      }
      for(int i = 0; i < 8 && k < d_header_len; i += d_bits_per_byte, k++) {
	output[k] = (unsigned char) ((crc >> i) & d_mask);
      }
      d_header_number++;
      d_header_number &= 0x0FFF;

      return true;
    }

    bool
    packet_formatter_1::parse(int nbits_in,
                              const unsigned char *input,
                              std::vector<pmt::pmt_t> &info)
    {
      unsigned header_len = 0;
      unsigned header_num = 0;

      d_info = pmt::make_dict();

      int k = 0; // Position in "in"
      for(int i = 0; i < 12 && k < d_header_len; i += d_bits_per_byte, k++) {
	header_len |= (((int) input[k]) & d_mask) << i;
      }

      d_info = pmt::dict_add(d_info, pmt::intern("packet_len"),
                             pmt::from_long(header_len));

      if(k >= d_header_len) {
        info.push_back(d_info);
	return true;
      }

      for(int i = 0; i < 12 && k < d_header_len; i += d_bits_per_byte, k++) {
        header_num |= (((int) input[k]) & d_mask) << i;
      }

      d_info = pmt::dict_add(d_info, pmt::intern("packet_num"),
                             pmt::from_long(header_num));

      if(k >= d_header_len) {
        info.push_back(d_info);
	return true;
      }

      d_crc_impl.reset();
      d_crc_impl.process_bytes((void const *) &header_len, 2);
      d_crc_impl.process_bytes((void const *) &header_num, 2);
      unsigned char crc_calcd = d_crc_impl();
      for(int i = 0; i < 8 && k < d_header_len; i += d_bits_per_byte, k++) {
        if((((int) input[k]) & d_mask) != (((int) crc_calcd >> i) & d_mask) ) {
          info.push_back(d_info);
          return false;
        }
      }

      info.push_back(d_info);
      return true;
    }


    size_t
    packet_formatter_1::header_nbits() const
    {
      return d_header_len/8;
    }

    size_t
    packet_formatter_1::header_nbytes() const
    {
      return d_header_len;
    }

  } /* namespace digital */
} /* namespace gr */
