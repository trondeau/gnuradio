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


#ifndef INCLUDED_DIGITAL_PACKET_FORMAT_BB_H
#define INCLUDED_DIGITAL_PACKET_FORMAT_BB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/digital/packet_formatter_default.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Uses a header formatter to append a header onto a PDU.
     * \ingroup packet_operators_blk
     *
     * \details

     * This is a tagged-stream block takes in a byte stream and
     * creates a header, generally for MAC-level processing.
     *
     * This creates and streams out the built header but does not
     * change the payload or pass it on.
     *
     * For different packet header formatting needs, we can define new
     * classes that inherit from the packet_formatter_default block
     * and which overload the packet_formatter_default::format
     * function.
     *
     * \sa packet_formatter_default
     * \sa packet_formatter_counter
     *
     */
    class DIGITAL_API packet_format_bb : virtual public tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<packet_format_bb> sptr;

      /*!
       * Make a packet header block using a given \p formatter.
       *
       * \param formatter The formatter class to use when creating the
       *        header for the packet. The default formatter is
       *        packet_formatter_default and just applies an access
       *        code and a header that repeats the PDU's frame length
       *        twice.
       * \param padding Adds trailing 0's. See set_padding.
       */
      static sptr make(const packet_formatter_default::sptr &formatter,
                       const std::string &len_tag_key = "packet_len",
                       unsigned int padding=0);

      /*!
       * Sets the amount of padding of 0's on the end of the
       * packet. See packet_formatter_default::set_padding for
       * details of how this works in the header formatter.
       */
      virtual void set_padding(unsigned int padding=0) = 0;

      /*!
       * Returns how much padding (trailing zeros) is applied to each
       * packet.
       */
      virtual unsigned int padding() const = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMAT_BB_H */
