/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PACKET_GATEWAY_IMPL_H
#define INCLUDED_GR_PACKET_GATEWAY_IMPL_H

#include <gnuradio/digital/packet_gateway.h>

namespace gr {
  namespace digital {

    class packet_gateway_impl : public packet_gateway
    {
    private:
      size_t d_itemsize;
      int d_mtu, d_avail;

      uint64_t d_to_skip, d_skipped;
      uint64_t d_pkt_len, d_written;

      enum packet_gateway_mode_t {
        ST_WAITING = 0,
        ST_PROCESSING
      };

      packet_gateway_mode_t d_mode;

    public:
      packet_gateway_impl(size_t itemsize, int mtu=1500);
      ~packet_gateway_impl();

      void handle_info(pmt::pmt_t msg);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PACKET_GATEWAY_IMPL_H */
