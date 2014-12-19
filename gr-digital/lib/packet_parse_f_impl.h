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

#ifndef INCLUDED_DIGITAL_PACKET_PARSE_F_IMPL_H
#define INCLUDED_DIGITAL_PACKET_PARSE_F_IMPL_H

#include <gnuradio/digital/packet_parse_f.h>

namespace gr {
  namespace digital {

    class packet_parse_f_impl : public packet_parse_f
    {
     private:
      packet_formatter_default::sptr d_formatter;
      pmt::pmt_t d_out_port;

     public:
      packet_parse_f_impl(const packet_formatter_default::sptr &formatter,
                          unsigned int threshold=0);
      ~packet_parse_f_impl();

      void set_threshold(unsigned int thresh);
      unsigned int threshold() const;

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_PARSE_F_IMPL_H */
