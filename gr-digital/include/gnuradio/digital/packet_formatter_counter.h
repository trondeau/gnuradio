/* -*- c++ -*- */
/* Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_PACKET_FORMATTER_COUNTER_H
#define INCLUDED_DIGITAL_PACKET_FORMATTER_COUNTER_H

#include <pmt/pmt.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/packet_formatter_default.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Header formatter that adds the payload bits/symbol
     * format and a packet number counter.
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * Child class of packet_formatter_default. This version adds two
     * fields to the header:
     *
     * \li bps (16 bits): bits/symbol used when modulating the payload.
     * \li count (16 bits): a counter for the packet number.
     *
     * Like the default packet formatter, the length is encoded as a
     * 16-bit value repeated twice. The full packet looks like:
     * \verbatim
         | access code | hdr | payload |
       \endverbatim
     *
     * Where the access code is <= 64 bits and hdr is:
     * \verbatim
         |  0 -- 15 | 16 -- 31 |
         | pkt len  | pkt len  |
         | bits/sym | counter  |
       \endverbatim
     *
     * The access code and header are formatted for network byte order.
     *
     * \sa packet_formatter_default
     */
    class DIGITAL_API packet_formatter_counter
      : public packet_formatter_default
    {
     public:
      packet_formatter_counter(const std::string &access_code, int bps);
      virtual ~packet_formatter_counter();

      /*!
       * Creates a header from the access code and packet length to
       * build an output packet in the form:
       *
       * \verbatim
           | access code | pkt len | pkt len | bps | counter |
         \endverbatim
       *
       * \param nbytes_in The length (in bytes) of the \p input payload
       * \param input An array of unsigned chars of the packet payload
       * \param output A pmt::u8vector with the new header prepended
       *        onto the input data.
       * \param info A pmt::dict containing meta data and info about
       *        the PDU (generally from the metadata portion of the
       *        input PDU). Data can be extracted from this for the
       *        header formatting or inserted.
       */
      virtual bool format(int nbytes_in,
                          const unsigned char *input,
                          pmt::pmt_t &output,
                          pmt::pmt_t &info);

      /*!
       * Returns the length of the formatted header in bits.
       */
      virtual size_t header_nbits() const;

      /*!
       * Returns the length of the formatted header in bytes.
       */
      virtual size_t header_nbytes() const;

      /*!
       * Factory to create an async packet header formatter; returns
       * an sptr to the object.
       */
      static sptr make(const std::string &access_code, int bps);

    protected:
      uint16_t d_counter;    //!< keeps track of the number of packets transmitted

      //! Verify that the header is valid
      bool header_ok();

      /*! Get info from the header; return payload length and package
       *  rest of data in d_info dictionary.
       *
       * Extracts the header of the form:
       *
       * \verbatim
           | access code | pkt len | pkt len | bps | counter | payload |
         \endverbatim
       */
      int header_payload();
  };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMATTER_COUNTER_H */
