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

#ifndef INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_IMPL_H
#define INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_IMPL_H

#include <gnuradio/digital/packet_format_async.h>

namespace gr {
  namespace digital {

    class packet_format_async_impl
      : public packet_format_async
    {
     private:
      packet_formatter_base::sptr d_formatter;

      pmt::pmt_t d_in_port;
      pmt::pmt_t d_hdr_port, d_pld_port;

      void append(pmt::pmt_t msg);

     public:
      packet_format_async_impl(const packet_formatter_base::sptr &formatter);
      ~packet_format_async_impl();
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_IMPL_H */
