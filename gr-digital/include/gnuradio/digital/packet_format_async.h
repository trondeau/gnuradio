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


#ifndef INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_H
#define INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/packet_formatter_default.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Uses a header formatter to append a header onto a PDU.
     * \ingroup packet_operators_blk
     *
     * \details

     * This block takes in PDUs and creates a header, generally for
     * MAC-level processing. Each received PDU is assumed to be its
     * own frame, so any fragmentation would be done upstream in or
     * before the flowgraph.
     *
     * The header that is created and transmitted out of the 'header'
     * message port as a PDU. The header is based entirely on the \p
     * formatter, which is a packet_formatter_default or a child
     * class. All of these packet header formatters operate the same:
     * they take in the payload data as well as possible extra
     * metadata info about the PDU; the formatter then returns the
     * output PDU as a PMT argument along with any changes to the
     * metadata info PMT.
     *
     * For different packet header formatting needs, we can define new
     * classes that inherit from the packet_formatter_default block
     * and which overload the packet_formatter_default::format
     * function.
     *
     * \sa packet_formatter_default
     * \sa packet_formatter_counter
     *
     * This block only uses asynchronous message passing interfaces to
     * receiver and emit PDUs. The message ports are:
     *
     * \li in: receives PDUs for the frame payload
     * \li header: the header formatted for the given frame
     * \li payload: the payload
     */
    class DIGITAL_API packet_format_async : virtual public block
    {
     public:
      typedef boost::shared_ptr<packet_format_async> sptr;

      /*!
       * Make a packet header block using a given \p formatter.
       *
       * \param formatter The formatter class to use when creating the
       *        header for the packet. The default formatter is
       *        packet_formatter_default and just applies an access
       *        code and a header that repeats the PDU's frame length
       *        twice.
       */
      static sptr make(const packet_formatter_default::sptr &formatter);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMAT_ASYNC_H */
