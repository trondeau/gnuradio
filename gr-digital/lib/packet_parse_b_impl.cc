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

#include "packet_parse_b_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <volk/volk.h>
#include <iostream>

namespace gr {
  namespace digital {

    packet_parse_b::sptr
    packet_parse_b::make(const packet_formatter_default::sptr &formatter,
                         unsigned int threshold)
    {
      return gnuradio::get_initial_sptr
	(new packet_parse_b_impl(formatter, threshold));
    }


    packet_parse_b_impl::packet_parse_b_impl(const packet_formatter_default::sptr &formatter,
                                             unsigned int threshold)
      : sync_block("packet_parse_b",
                   io_signature::make(1, 1, sizeof(char)),
                   io_signature::make(0, 0, 0))
    {
      d_formatter = formatter;
      set_threshold(threshold);

      d_out_port = pmt::mp("info");
      message_port_register_out(d_out_port);
    }

    packet_parse_b_impl::~packet_parse_b_impl()
    {
    }

    void
    packet_parse_b_impl::set_threshold(unsigned int thresh)
    {
      d_formatter->set_threshold(thresh);
    }


    unsigned int
    packet_parse_b_impl::threshold() const
    {
      return d_formatter->threshold();
    }

    int
    packet_parse_b_impl::work(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];

      int count = 0;
      std::vector<pmt::pmt_t> info;
      bool ret = d_formatter->parse(noutput_items, in, info, count);

      if(ret) {
        for(size_t i = 0; i < info.size(); i++) {
          message_port_pub(d_out_port, info[i]);
        }
      }
      else {
        message_port_pub(d_out_port, pmt::PMT_F);
      }

      return count;
    }

  } /* namespace digital */
} /* namespace gr */
