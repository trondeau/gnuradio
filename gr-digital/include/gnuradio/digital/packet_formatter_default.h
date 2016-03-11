/* -*- c++ -*- */
/* Copyright 2014-2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_PACKET_FORMATTER_DEFAULT_H
#define INCLUDED_DIGITAL_PACKET_FORMATTER_DEFAULT_H

#include <pmt/pmt.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/logger.h>
#include <boost/enable_shared_from_this.hpp>

namespace gr {
  namespace digital {

    /*!
     * \brief Default header formatter for PDU formatting.
     * \ingroup packet_operators_blk
     *
     * \details

     * Used to handle the default packet header. The formatters manage
     * two functions:
     *
     * \li packet_formatter_default::format: takes in a
     * payload and creates a header from it.
     *
     * \li packet_formatter_default::parse receive bits and
     * extract the header info.
     *
     * Another version of the parser is
     * packet_formatter_default::parse_soft that receives soft
     * decisions instead of hard bits.
     *
     * This class is specifically designed to work with packets/frames
     * in the asynchronous PDU architecture of GNU Radio. See the
     * packet_format_async block for formatting the headers onto
     * payloads and packet_parse_b and packet_parse_f blocks for
     * parsing headers in a receiver.
     *
     * The Packet Format block takes in a PDU and uses a formatter
     * class derived from this class to add a header onto the
     * packet. The Packet Format blocks takes in the PDU, unpacks the
     * message, and passes it to a formatter class' format function,
     * which builds a header based on the payload. The header is
     * passed back and emitted from formatter block as a separate
     * output. The async format block, packet_format_async, has two
     * message output ports. The 'header' port passes the header out
     * as a PDU and the 'payload' passes the payload out as a PDU. The
     * flowgraph can then separately modulate and combine these two
     * pieces in the follow-on processing.
     *
     * The packet_sync_b and packet_sync_f blocks use the same
     * formatter class by calling the 'parse' or 'parse_soft' to parse
     * the received packet headers. These parser blocks are sinks for
     * the data stream and emit a message from an 'info' port that
     * contains an PMT dictionary of the information in the
     * header. The formatter class determines the dictionary keys.
     *
     * This is the base class for dealing with formatting headers for
     * different protocols and purposes. For other header formatting
     * behaviors, create a child class from here and overload the
     * format, parse, parse_soft, and parsing state machine functions
     * as necessary.
     *
     * The default header created in this base class consists of an
     * access code and the packet length. The length is encoded as a
     * 16-bit value repeated twice:
     *
     * \verbatim
         | access code | hdr | payload |
       \endverbatim
     *
     * Where the access code is <= 64 bits and hdr is:
     *
     * \verbatim
         |  0 -- 15 | 16 -- 31 |
         | pkt len  | pkt len  |
       \endverbatim
     *
     * The access code and header are formatted for network byte order.
     *
     * This header generator does not calculate or append a CRC to the
     * packet. Use the CRC32 Async block for that before adding the
     * header. The header's length will then measure the payload plus
     * the CRC length (4 bytes for a CRC32).
     *
     * The default header parser produces a PMT dictionary that
     * contains the following keys. All formatter blocks MUST produce
     * these two values in any dictionary.
     *
     * \li "skip samps": the number of samples between the end of the
     * last payload and the beginning of this payload. This is used by
     * a decoder block to know where in the payload to begin
     * demodulating the payload.
     *
     * \li "payload bits": the number of bits in the payload. The
     * payload decoder will have to know how this relates to the
     * number of symbols received.
     *
     * \sa packet_formatter_counter
     * \sa packet_formatter_1
     * \sa packet_formatter_ofdm
     */
    class DIGITAL_API packet_formatter_default
      : public boost::enable_shared_from_this<gr::digital::packet_formatter_default>
    {
     public:
      typedef boost::shared_ptr<packet_formatter_default> sptr;

      packet_formatter_default(const std::string &access_code);
      virtual ~packet_formatter_default();

      sptr base() { return shared_from_this(); };
      sptr formatter() { return shared_from_this(); };

      /*!
       * Creates a header from the access code and packet length and
       * creates an output header as a PMT vector in the form:
       *
       * \verbatim
           | access code | pkt len | pkt len |
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
       * Parses a header of the form:
       *
       * \verbatim
           | access code | pkt len | pkt len | payload |
         \endverbatim
       *
       * This is implemented as a state machine that starts off
       * searching for the access code. Once found, the access code is
       * used to find the start of the packet and the following
       * header. This default header encodes the length of the payload
       * a 16 bit integer twice. The state machine finds the header
       * and checks that both payload length values are the same. It
       * then goes into its final state that reads in the payload
       * (based on the payload length) and produces a payload as a PMT
       * u8 vector of packed bytes.
       *
       * \param nbits_in The number of bits in the input array.
       * \param input The input as hard decision bits.
       * \param info A vector of pmt::dicts to hold any meta data or
       *        info about the PDU. When parsing the header, the
       *        formatter can add info from the header into this dict.
       *        Each packet has a single PMT dictionary of info, so
       *        the vector length is the number of packets received
       *        extracted during one call to this parser function.
       * \param nbits_processed Number of input bits actually
       *        processed; If all goes well, this is nbits_in. A
       *        premature return after a bad header could be less than
       *        this.
       */
      virtual bool parse(int nbits_in,
                         const unsigned char *input,
                         std::vector<pmt::pmt_t> &info,
                         int &nbits_processed);

      /*!
       * Parses a header of the form:
       *
       * \verbatim
           | access code | pkt len | pkt len | payload |
         \endverbatim
       *
       * This is implemented as a state machine that starts off
       * searching for the access code. Once found, the access code is
       * used to find the start of the packet and the following
       * header. This default header encodes the length of the payload
       * a 16 bit integer twice. The state machine finds the header
       * and checks that both payload length values are the same. It
       * then goes into its final state that reads in the payload
       * (based on the payload length) and produces a payload as a PMT
       * f32 vector of soft decision bits.
       *
       * This form of the header parser is specifically intended for
       * use with FEC decoders, in particular the
       * gr::fec::async_decoder.
       *
       * \param nbits_in The number of soft decisions in the input array.
       * \param input The input as soft decision floats.
       * \param info A vector of pmt::dicts to hold any meta data or
       *        info about the PDU. When parsing the header, the
       *        formatter can add info from the header into this dict.
       *        Each packet has a single PMT dictionary of info, so
       *        the vector length is the number of packets received
       *        extracted during one call to this parser function.
       * \param nbits_processed Number of input bits actually
       *        processed; If all goes well, this is nbits_in. A
       *        premature return after a bad header could be less than
       *        this.
       */
      virtual bool parse_soft(int nbits_in,
                              const float *input,
                              std::vector<pmt::pmt_t> &info,
                              int &nbits_processed);

      /*!
       * Returns the length of the formatted header in bits.
       */
      virtual size_t header_nbits() const;

      /*!
       * Returns the length of the formatted header in bytes.
       */
      virtual size_t header_nbytes() const;

      /*!
       * Updates the access code. Must be a string of 1's and 0's and
       * <= 64 bits.
       */
      bool set_access_code(const std::string &access_code);

      /*!
       * Returns the formatted access code as a 64-bit register.
       */
      unsigned long long access_code() const;

      /*!
       * Sets the threshold for number of access code bits can be in
       * error before detection. Defaults to 0.
       */
      void set_threshold(unsigned int thresh=0);

      /*!
       * Returns threshold value for access code detection.
       */
      unsigned int threshold() const;

      /*!
       * Factory to create an async packet header formatter; returns
       * an sptr to the object.
       */
      static sptr make(const std::string &access_code);

    protected:
      enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC};

      uint64_t d_access_code;        //!< register to hold the access code
      size_t d_access_code_len;      //!< length in bits of the access code

      state_t d_state;               //!< state of the state machine
      unsigned long long d_data_reg; //!< used to look for access_code
      unsigned long long d_mask;     /*!< masks access_code bits (top N bits are set where
                                       N is the number of bits in the access code) */
      unsigned int d_threshold;      //!< how many bits may be wrong in sync vector

      header_buffer d_hdr_reg;

      int d_pkt_len;                 //!< Length of the packet to put into the output buffer
      int d_pkt_count;               //!< Number of bytes/soft bits already received
      pmt::pmt_t d_info;             //!< info captured from the header

      uint16_t d_bps;                //!< bits/sec of payload modulation

      int d_nbits;                   //!< num bits processed since reset

      //! Enter Search state of the state machine to find the access code.
      virtual void enter_search();

      //! Access code found, start getting the header
      virtual void enter_have_sync();

      //! Header found, setup for pulling in the hard decision bits
      virtual void enter_have_header(int payload_len);

      //! Verify that the header is valid
      virtual bool header_ok();

      /*! Get info from the header; return payload length and package
       *  rest of data in d_info dictionary.
       */
      virtual int header_payload();

      /*! Used by blocks to access the logger system.
       */
      gr::logger_ptr d_logger;
      gr::logger_ptr d_debug_logger;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_FORMATTER_DEFAULT_H */
