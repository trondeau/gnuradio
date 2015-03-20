/* -*- c++ -*- */
/* Copyright 2015 Free Software Foundation, Inc.
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
#include <gnuradio/digital/header_buffer.h>

namespace gr {
  namespace digital {

    header_buffer::header_buffer(uint8_t *bytes_out)
    {
      d_offset = 0;
      d_buffer = bytes_out;
    }

    header_buffer::~header_buffer()
    {
    }

    int
    header_buffer::length() const
    {
      return d_offset;
    }

    const uint8_t*
    header_buffer::header() const
    {
      return d_buffer;
    }

    void
    header_buffer::add_field(int len, uint8_t data, bool be)
    {
      memcpy(&d_buffer[d_offset], &data, len);
      d_offset += len;
    }

    void
    header_buffer::add_field(int len, uint16_t data, bool be)
    {
      uint16_t x = data;
      if(!be) {
        volk_16u_byteswap(&x, 1);
        x = x >> (16-len*8);
      }
      memcpy(&d_buffer[d_offset], &x, len);
      d_offset += len;
    }

    void
    header_buffer::add_field(int len, uint32_t data, bool be)
    {
      uint32_t x = data;
      if(!be) {
        volk_32u_byteswap(&x, 1);
        x = x >> (32-len*8);
      }
      memcpy(&d_buffer[d_offset], &x, len);
      d_offset += len;
    }

    void
    header_buffer::add_field(int len, uint64_t data, bool be)
    {
      uint64_t x = data;
      if(!be) {
        volk_64u_byteswap(&x, 1);
        x = x >> (64-len*8);
      }
      memcpy(&d_buffer[d_offset], &x, len);
      d_offset += len;
    }

  } /* namespace digital */
} /* namespace gr */
