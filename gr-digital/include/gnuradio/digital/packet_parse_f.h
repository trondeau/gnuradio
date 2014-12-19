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


#ifndef INCLUDED_DIGITAL_PACKET_PARSE_F_H
#define INCLUDED_DIGITAL_PACKET_PARSE_F_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/packet_formatter_default.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Block that synchronizes to a header based on a header
     * formatter class. Designed to accept soft decisions and produce
     * PDUs with soft decisions (pmt::f32vector).
     *
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * A packet synchronizer block. This block takes in soft decision
     * bits (floats) and finds the access code as a sync word to find
     * the start of a frame.
     *
     * The block uses a formatter derived from a
     * packet_formatter_default formatter class. The only thing
     * this class is required to have in the header is an access code
     * of up to 64 bits. This block uses the formatter and access code
     * to do the frame synchronization. An access code is found if the
     * received bits have fewer than \p threshold errors detected.
     *
     * As soft decisions are received, the header is generally uncoded
     * or the formatter will understand the code and handle it
     * itself. Mostly, the header_parser will just slice the decisions
     * at 0 and convert to hard 1's and 0's.
     *
     * Once the frame access code is detected, the block uses the
     * formatter's parse_header to decode the remaining
     * header. Generally, as in the default header case, the header
     * will contain the length of the frame's payload. That and
     * anything else in the header will generally go into the PDU's
     * meta-data dictionary.
     *
     * The block will output a PDU that contains frame's header info
     * in the meta-data portion of the PDU and the payload itself. The
     * payload will be the input soft decision values, so the payload
     * will be a float vector.
     *
     * \sa packet_sync_bb for a hard decision version.
     */
    class DIGITAL_API packet_parse_f : virtual public sync_block
    {
     public:
      typedef boost::shared_ptr<packet_parse_f> sptr;

      /*!
       * Make a packet header block using a given \p formatter.
       *
       * \param formatter The formatter class to use when reading the
       *        header.
       * \param threshold How many bits in the access code the
       *        \p formatter can miss and still trigger a hit.
       */
      static sptr make(const packet_formatter_default::sptr &formatter,
                       unsigned int threshold=0);

      /*!
       * Sets the threshold level for the formatter when finding the
       * access code. See packet_formatter_default::set_threshold
       * for more details.
       */
      virtual void set_threshold(unsigned int thresh) = 0;

      /*!
       * Gets the access code detection threshold from the formatter.
       */
      virtual unsigned int threshold() const = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_PARSE_F_H */
