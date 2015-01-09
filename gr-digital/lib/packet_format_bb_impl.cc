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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <volk/volk.h>
#include <gnuradio/io_signature.h>
#include "packet_format_bb_impl.h"
#include <stdio.h>

namespace gr {
  namespace digital {

    packet_format_bb::sptr
    packet_format_bb::make(const packet_formatter_default::sptr &formatter,
                           const std::string &len_tag_key,
                           unsigned int padding)
    {
      return gnuradio::get_initial_sptr
        (new packet_format_bb_impl(formatter, len_tag_key, padding));
    }

    packet_format_bb_impl::packet_format_bb_impl(const packet_formatter_default::sptr &formatter,
                                                 const std::string &len_tag_key,
                                                 unsigned int padding)
      : tagged_stream_block("packet_format_bb",
                            io_signature::make(1, 1, sizeof (char)),
                            io_signature::make(1, 1, sizeof (char)),
                            len_tag_key),
        d_formatter(formatter)
    {
      set_output_multiple(d_formatter->header_nbytes());
      // This is the worst case rate, because we don't know the true value, of course
      set_relative_rate(d_formatter->header_nbytes());
      set_tag_propagation_policy(TPP_DONT);

      set_padding(padding);
    }

    packet_format_bb_impl::~packet_format_bb_impl()
    {
    }

    void
    packet_format_bb_impl::set_padding(unsigned int padding)
    {
      d_padding = padding;
    }

    unsigned int
    packet_format_bb_impl::padding() const
    {
      return d_padding;
    }

    int
    packet_format_bb_impl::work(int noutput_items,
                                gr_vector_int &ninput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      std::vector<tag_t> tags;
      pmt::pmt_t info = pmt::make_dict();
      get_tags_in_window(tags, 0, 0, ninput_items[0]);
      for (unsigned i = 0; i < tags.size(); i++) {
        info = pmt::dict_add(info, tags[i].key, tags[i].value);
      }

      pmt::pmt_t output_pmt;
      if(!d_formatter->format(ninput_items[0], in, output_pmt, info)) {
        GR_LOG_FATAL(d_logger,
                     boost::format("header formatter returned false at position %1%") \
                     % nitems_read(0));
        throw std::runtime_error("header formatter returned false.");
      }

      size_t pkt_len;
      const uint8_t* output_bytes = pmt::u8vector_elements(output_pmt, pkt_len);
      memcpy(out, output_bytes, pkt_len*sizeof(char));

      return d_formatter->header_nbytes();
    }

  } /* namespace digital */
} /* namespace gr */
