/* -*- c++ -*- */
/* Copyright 2014 Free Software Foundation, Inc.
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

#include <iostream>
#include <string.h>
#include <volk/volk.h>
#include <gnuradio/digital/packet_formatter_default.h>
#include <gnuradio/math.h>

namespace gr {
  namespace digital {

    packet_formatter_default::sptr
    packet_formatter_default::make(const std::string &access_code)
    {
      return packet_formatter_default::sptr
        (new packet_formatter_default(access_code));
    }

    packet_formatter_default::packet_formatter_default(const std::string &access_code)
    {
      set_access_code(access_code);
      d_count = 0;
      d_pkt_len = 0;
      enter_search();
      d_threshold = 0;
      d_data_reg = 0;
      d_bps = 1; // set in child classes that use this
    }

    packet_formatter_default::~packet_formatter_default()
    {
    }

    bool
    packet_formatter_default::set_access_code(const std::string &access_code)
    {
      d_access_code_len = access_code.length();	// # of bits in the access code

      if((access_code.size() % 8 != 0) || (access_code.size() > 64))
        return false;

      // set len top bits to 1.
      d_mask = ((~0ULL) >> (64 - d_access_code_len));

      d_access_code = 0;
      for(unsigned i = 0; i < d_access_code_len; i++) {
        d_access_code = (d_access_code << 1) | (access_code[i] & 1);
      }

      return true;
    }

    unsigned long long
    packet_formatter_default::access_code() const
    {
      return d_access_code;
    }

    void
    packet_formatter_default::set_threshold(unsigned int thresh)
    {
      if(d_threshold > d_access_code_len)
        throw std::runtime_error("packet_formatter_default: Cannot set threshold larger than the access code length.");
      d_threshold = thresh;
    }

    unsigned int
    packet_formatter_default::threshold() const
    {
      return d_threshold;
    }

    bool
    packet_formatter_default::format(int nbytes_in,
                                     const unsigned char *input,
                                     pmt::pmt_t &output,
                                     pmt::pmt_t &info)
    {
      size_t header_size = header_nbytes();
      uint8_t* bytes_out = (uint8_t*)volk_malloc(header_size*sizeof(uint8_t),
                                                 volk_get_alignment());

      uint16_t len = static_cast<uint16_t>(nbytes_in);
      volk_16u_byteswap(&len, 1);

      uint64_t ac = d_access_code;
      size_t ac_bytes = d_access_code_len/8;
      volk_64u_byteswap(&ac, 1);
      ac = ac >> (64-d_access_code_len);

      // Copy access code, header info, and input to the output buffer
      int offset = 0;
      memcpy(bytes_out, &ac, ac_bytes);
      offset += ac_bytes;
      memcpy(&bytes_out[offset], &len, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      memcpy(&bytes_out[offset], &len, sizeof(uint16_t));

      // Package output data into a PMT vector
      output = pmt::init_u8vector(header_size, bytes_out);

      // Creating the output pmt copies data; free our own here.
      volk_free(bytes_out);

      return true;
    }

    bool
    packet_formatter_default::parse(int nbits_in,
                                    const unsigned char *input,
                                    std::vector<pmt::pmt_t> &info)
    {
      int count = 0;

      while(count < nbits_in) {
        switch(d_state) {
	case STATE_SYNC_SEARCH:    // Look for the access code correlation

	  while(count < nbits_in) {
            // shift in new data
            d_data_reg = (d_data_reg << 1) | ((input[count++]) & 0x1);
            d_count++;

            // compute hamming distance between desired access code and current data
            uint64_t wrong_bits = 0;
            uint64_t nwrong = d_threshold+1;

            wrong_bits = (d_data_reg ^ d_access_code) & d_mask;
            volk_64u_popcnt(&nwrong, wrong_bits);

            if(nwrong <= d_threshold) {
              enter_have_sync();
              break;
            }
          }
          break;

	case STATE_HAVE_SYNC:
	  while(count < nbits_in) {    // Shift bits one at a time into header
            d_hdr_reg = (d_hdr_reg << 1) | (input[count++] & 0x1);
            d_hdr_count++;
            d_count++;

            if(d_hdr_count == (header_nbits()-d_access_code_len)) {
	      // we have a full header, check to see if it has been received properly
	      if(header_ok()) {
                int payload_len = header_payload();
		enter_have_header(payload_len);
                info.push_back(d_info);
              }
	      else {
		enter_search();    // bad header
              }
              break;
            }
          }
          break;

	case STATE_HAVE_HEADER:
	  while(count < nbits_in) {
            if(d_pkt_count < d_pkt_len/d_bps) {
              count++;
              d_pkt_count++;
            }
            if(d_pkt_count == d_pkt_len/d_bps) {
              enter_search();
              break;
            }
          }
          break;
        }
      }

      return true;
    }

    bool
    packet_formatter_default::parse_soft(int nbits_in,
                                         const float *input,
                                         std::vector<pmt::pmt_t> &info)
    {
      int count = 0;

      while(count < nbits_in) {
        switch(d_state) {
	case STATE_SYNC_SEARCH:    // Look for the access code correlation

	  while(count < nbits_in) {
            // shift in new data
            d_data_reg = (d_data_reg << 1) | (gr::branchless_binary_slicer(input[count++]) & 0x1);
            d_count++;

            // compute hamming distance between desired access code and current data
            uint64_t wrong_bits = 0;
            uint64_t nwrong = d_threshold+1;

            wrong_bits = (d_data_reg ^ d_access_code) & d_mask;
            volk_64u_popcnt(&nwrong, wrong_bits);

            if(nwrong <= d_threshold) {
              enter_have_sync();
              break;
            }
          }
          break;

	case STATE_HAVE_SYNC:
	  while(count < nbits_in) {    // Shift bits one at a time into header
            d_hdr_reg = (d_hdr_reg << 1) | (gr::branchless_binary_slicer(input[count++]) & 0x1);
            d_hdr_count++;
            d_count++;

            if(d_hdr_count == (header_nbits()-d_access_code_len)) {
	      // we have a full header, check to see if it has been received properly
	      if(header_ok()) {
                int payload_len = header_payload();
		enter_have_header(8*payload_len);
                info.push_back(d_info);
              }
	      else {
		enter_search();    // bad header
              }
              break;
            }
          }
          break;

	case STATE_HAVE_HEADER:
	  while(count < nbits_in) {
            if(d_pkt_count < d_pkt_len/d_bps) {
              count++;
              d_pkt_count++;
            }
            if(d_pkt_count == d_pkt_len/d_bps) {
              enter_search();
              break;
            }
          }
          break;
        }
      }

      return true;
    }


    size_t
    packet_formatter_default::header_nbits() const
    {
      return d_access_code_len + 8*2*sizeof(uint16_t);
    }

    size_t
    packet_formatter_default::header_nbytes() const
    {
      return d_access_code_len/8 + 2*sizeof(uint16_t);
    }

    inline void
    packet_formatter_default::enter_search()
    {
      d_state = STATE_SYNC_SEARCH;
    }

    inline void
    packet_formatter_default::enter_have_sync()
    {
      d_state = STATE_HAVE_SYNC;
      d_hdr_reg = 0;
      d_hdr_count = 0;
    }

    inline void
    packet_formatter_default::enter_have_header(int payload_len)
    {
      d_state = STATE_HAVE_HEADER;
      d_pkt_len = payload_len;
      d_pkt_count = 0;
      d_count = 0;
    }

    bool
    packet_formatter_default::header_ok()
    {
      // confirm that two copies of header info are identical
      return ((d_hdr_reg >> 16) ^ (d_hdr_reg & 0xffff)) == 0;
    }

    int
    packet_formatter_default::header_payload()
    {
      int len = (d_hdr_reg) & 0xffff;

      d_info = pmt::make_dict();
      d_info = pmt::dict_add(d_info, pmt::intern("skip samps"),
                             pmt::from_long(d_count));
      d_info = pmt::dict_add(d_info, pmt::intern("payload bits"),
                             pmt::from_long(8*len));
      return len;
    }

  } /* namespace digital */
} /* namespace gr */
