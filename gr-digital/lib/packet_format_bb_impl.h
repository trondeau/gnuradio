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

#ifndef INCLUDED_DIGITAL_PACKET_FORMAT_BB_IMPL_H
#define INCLUDED_DIGITAL_PACKET_FORMAT_BB_IMPL_H

#include <gnuradio/digital/packet_format_bb.h>

namespace gr {
  namespace digital {

    class packet_format_bb_impl
      : public packet_format_bb
    {
     private:
      packet_formatter_default::sptr d_formatter;
      unsigned int d_padding;

     public:
      packet_format_bb_impl(const packet_formatter_default::sptr &formatter,
                            const std::string &len_tag_key = "packet_len",
                            unsigned int padding=0);
      ~packet_format_bb_impl();

      void set_padding(unsigned int padding=0);
      unsigned int padding() const;

      int work(int noutput_items,
               gr_vector_int &ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMAT_BB_IMPL_H */
