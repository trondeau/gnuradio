/* -*- c++ -*- */
/* Copyright 2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_PACKET_FORMATTER_1_H
#define INCLUDED_DIGITAL_PACKET_FORMATTER_1_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/packet_formatter_default.h>
#include <boost/crc.hpp>

namespace gr {
  namespace digital {

    /*!
     * \brief Default header formatter for digital packet transmission.
     * \ingroup packet_operators_blk
     * \sa packet_formatter_default
     *
     * \details
     * For bursty/packetized digital transmission, packets are usually prepended
     * with a packet header, containing the number of bytes etc.
     * This class is not a block, but a tool to create these packet header.
     *
     * This is a default packet header (see header_formatter()) for a description
     * on the header format). To create other header, derive packet header creator
     * classes from this function.
     *
     * gr::digital::packet_headergenerator_bb uses header generators derived from
     * this class to create packet headers from data streams.
     */
    class DIGITAL_API packet_formatter_1
      : public packet_formatter_default
    {
     public:
      packet_formatter_1(long header_len,
                         const std::string &len_tag_key="packet_len",
                         const std::string &num_tag_key="packet_num",
                         int bits_per_byte=1);
      virtual ~packet_formatter_1();

      void set_header_num(unsigned header_num) { d_header_number = header_num; };

      /*!
       * \brief Encodes the header information in the given tags into bits and places them into \p out
       *
       * Uses the following header format:
       * Bits 0-11: The packet length (what was stored in the tag with key \p len_tag_key)
       * Bits 12-23: The header number (counts up everytime this function is called)
       * Bit 24-31: 8-Bit CRC
       * All other bits: Are set to zero
       *
       * If the header length is smaller than 32, bits are simply left out. For this
       * reason, they always start with the LSB.
       *
       * However, it is recommended to stay above 32 Bits, in order to have a working
       * CRC.
       */
      virtual bool format(int nbytes_in,
                          const unsigned char *input,
                          pmt::pmt_t &output,
                          pmt::pmt_t &info);

      /*!
       * \brief Inverse function to format().
       *
       */
      virtual bool parse(int nbits_in,
                         const unsigned char *input,
                         std::vector<pmt::pmt_t> &info);

      /*!
       * Returns the length of the formatted header in bits.
       */
      virtual size_t header_nbits() const;

      /*!
       * Returns the length of the formatted header in bytes.
       */
      virtual size_t header_nbytes() const;

      static sptr make(long header_len,
                       const std::string &len_tag_key="packet_len",
                       const std::string &num_tag_key="packet_num",
                       int bits_per_byte=1);

    protected:
      long d_header_len;
      pmt::pmt_t d_len_tag_key;
      pmt::pmt_t d_num_tag_key;
      int d_bits_per_byte;
      unsigned d_header_number;
      unsigned d_mask;
      boost::crc_optimal<8, 0x07, 0xFF, 0x00, false, false>  d_crc_impl;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMATTER_1_H */
