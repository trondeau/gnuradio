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

#include <gnuradio/digital/packet_formatter_ofdm.h>
#include <gnuradio/digital/lfsr.h>
#include <iostream>

namespace gr {
  namespace digital {

    int
    packet_formatter_ofdm::_get_header_len_from_occupied_carriers
    (const std::vector<std::vector<int> > &occupied_carriers, int n_syms)
    {
      int header_len = 0;
      for(int i = 0; i < n_syms; i++) {
	header_len += occupied_carriers[i].size();
      }

      return header_len;
    }

    packet_formatter_ofdm::sptr
    packet_formatter_ofdm::make(const std::vector<std::vector<int> > &occupied_carriers,
                                int n_syms,
                                const std::string &len_tag_key,
                                const std::string &frame_len_tag_key,
                                const std::string &num_tag_key,
                                int bits_per_header_sym,
                                int bits_per_payload_sym,
                                bool scramble_header)
    {
      return packet_formatter_ofdm::sptr
        (new packet_formatter_ofdm(occupied_carriers, n_syms,
                                   len_tag_key, frame_len_tag_key, num_tag_key,
                                   bits_per_header_sym, bits_per_payload_sym,
                                   scramble_header));
    }

    packet_formatter_ofdm::packet_formatter_ofdm(const std::vector<std::vector<int> > &occupied_carriers,
                                                 int n_syms,
                                                 const std::string &len_tag_key,
                                                 const std::string &frame_len_tag_key,
                                                 const std::string &num_tag_key,
                                                 int bits_per_header_sym,
                                                 int bits_per_payload_sym,
                                                 bool scramble_header)
      : packet_formatter_1(_get_header_len_from_occupied_carriers(occupied_carriers, n_syms),
                           len_tag_key, num_tag_key, bits_per_header_sym),
      d_frame_len_tag_key(pmt::string_to_symbol(frame_len_tag_key)),
      d_occupied_carriers(occupied_carriers),
      d_syms_per_set(0),
      d_bits_per_payload_sym(bits_per_payload_sym),
      d_scramble_mask(d_header_len, 0)
    {
      for(unsigned i = 0; i < d_occupied_carriers.size(); i++) {
	d_syms_per_set += d_occupied_carriers[i].size();
      }

      // Init scrambler mask
      if(scramble_header) {
	// These are just random values which already have OK PAPR:
	gr::digital::lfsr shift_reg(0x8a, 0x6f, 7);
	for(int i = 0; i < d_header_len; i++) {
	  for(int k = 0; k < bits_per_header_sym; k++) {
	    d_scramble_mask[i] ^= shift_reg.next_bit() << k;
	  }
	}
      }
    }

    packet_formatter_ofdm::~packet_formatter_ofdm()
    {
    }

    bool
    packet_formatter_ofdm::format(int nbytes_in,
                                  const unsigned char *input,
                                  pmt::pmt_t &output,
                                  pmt::pmt_t &info)
    {
      bool ret_val = packet_formatter_1::format(nbytes_in, input, output, info);

      size_t pkt_len;
      uint8_t* bytes = pmt::u8vector_writable_elements(output, pkt_len);
      for(int i = 0; i < d_header_len; i++) {
      	bytes[i] ^= d_scramble_mask[i];
      }
      return ret_val;
    }

    bool
    packet_formatter_ofdm::parse(int nbits_in,
                                 const unsigned char *input,
                                 std::vector<pmt::pmt_t> &info)
   {
      std::vector<unsigned char> in_descrambled(d_header_len, 0);
      for(int i = 0; i < d_header_len; i++) {
      	in_descrambled[i] = input[i] ^ d_scramble_mask[i];
      }

      if(!packet_formatter_1::parse(in_descrambled.size(), &in_descrambled[0], info)) {
	return false;
      }

      int packet_len = 0; // # of bytes in this frame
      if(pmt::dict_has_key(info[0], d_len_tag_key)) {
        long len = pmt::to_long(pmt::dict_ref(info[0], d_len_tag_key, pmt::PMT_NIL));
        packet_len = len * 8 / d_bits_per_payload_sym;
        if(len * 8 % d_bits_per_payload_sym) {
          packet_len++;
        }
        info[0] = pmt::dict_delete(info[0], d_len_tag_key);
        info[0] = pmt::dict_add(info[0], d_len_tag_key,
                                pmt::from_long(packet_len));
      }

      // frame_len == # of OFDM symbols in this frame
      int frame_len = packet_len / d_syms_per_set;
      int k = 0;
      int i = frame_len * d_syms_per_set;
      while(i < packet_len) {
	frame_len++;
	i += d_occupied_carriers[k].size();
      }
      info[0] = pmt::dict_add(info[0],  d_frame_len_tag_key,
                              pmt::from_long(frame_len));
      return true;
    }

  } /* namespace digital */
} /* namespace gr */
