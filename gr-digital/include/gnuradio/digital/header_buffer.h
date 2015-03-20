/* -*- c++ -*- */
/* Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_BUFFER_H
#define INCLUDED_DIGITAL_HEADER_BUFFER_H

#include <gnuradio/digital/api.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Helper class for creating payload headers for OTA transmission.
     * \ingroup packet_operators_blk
     *
     * \details
     *
     * This class is used by the packet formatter blocks (e.g.,
     * packet_formatter_default) to make crafting payload headers
     * easier to manage. The code that uses this should prepare a
     * buffer of the appropriate size and pass the pointer to the
     * buffer to the header_buffer constructor (\p bytes_out). This
     * buffer is then filled with data by calling add_field with a
     * length and an item of information to pack into the buffer.
     *
     * Each field is a specific length of 8, 16, 32, or 64 bits that
     * are to be transmitted in network byte order (bit
     * endian). Generally, data passed to the add_field calls is
     * formatted little endian, so the data is first converted to big
     * endian and then added to the back of the buffer. If the data is
     * already big endian, set the \p be flag to true.
     *
     * The length argument for all add_field calls is the number of
     * bytes actually accounted for in the data structure. Generally,
     * we can just set this to sizeof(dtype), but occassionally we may
     * need to use fewer bytes than actually represented by the data
     * type. An example would be the access code used in the
     * packet_formatter_default, which is a uint64_t type but may have
     * fewer bytes used in the actual access code.
     *
     * The function that class this class is expected to handle the
     * memory handling of the buffer -- both allocating and
     * deallocating.
     */
    class DIGITAL_API header_buffer
    {
    private:
      int d_offset;
      uint8_t *d_buffer;

    public:
      /*!
       * Create a header buffer object with a pre-allocated buffer, \p
       * buffer, to hold the formatted header data.
       */
      header_buffer(uint8_t *buffer);

      /*!
       * Class destructor.
       */
      ~header_buffer();

      /*!
       * Returns the length of the data in the buffer (not the
       * allocated buffer length).
       */
      int length() const;

      /*!
       * Returns a constant pointer to the buffer.
       */
      const uint8_t* header() const;

      /*!
       * Add an 8-bit field to the header.
       *
       * \param len Length (in bytes) of \p data (here, always 1).
       * \param data The 8-bit data item.
       * \param be Set to 'true' if data is already big endian.
       */
      void add_field(int len, uint8_t data, bool be=false);

      /*!
       * Add an 16-bit field to the header.
       *
       * \param len Length (in bytes) of \p data.
       * \param data The 16-bit data item.
       * \param be Set to 'true' if data is already big endian.
       */
      void add_field(int len, uint16_t data, bool be=false);

      /*!
       * Add an 32-bit field to the header.
       *
       * \param len Length (in bytes) of \p data.
       * \param data The 32-bit data item.
       * \param be Set to 'true' if data is already big endian.
       */
      void add_field(int len, uint32_t data, bool be=false);

      /*!
       * Add an 64-bit field to the header.
       *
       * \param len Length (in bytes) of \p data.
       * \param data The 64-bit data item.
       * \param be Set to 'true' if data is already big endian.
       */
      void add_field(int len, uint64_t data, bool be=false);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_BUFFER_H */
