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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "packet_gateway_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace digital {

    packet_gateway::sptr
    packet_gateway::make(size_t itemsize, int mtu, int bps)
    {
      return gnuradio::get_initial_sptr
        (new packet_gateway_impl(itemsize, mtu, bps));
    }

    packet_gateway_impl::packet_gateway_impl(size_t itemsize, int mtu, int bps)
      : block("packet_gateway",
              io_signature::make(1, 1, itemsize),
              io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_mtu(mtu)
    {
      set_bps(bps);

      d_mode = ST_WAITING;
      d_skipped = 0;
      d_avail = 0;
      d_written = 0;

      message_port_register_in(pmt::mp("info"));
      set_msg_handler(pmt::mp("info"),
                      boost::bind(&packet_gateway_impl::handle_info, this, _1));

      set_min_output_buffer(4*d_mtu*itemsize);
      //set_output_multiple(d_mtu);
    }

    packet_gateway_impl::~packet_gateway_impl()
    {
    }

    void
    packet_gateway_impl::handle_info(pmt::pmt_t msg)
    {
      if(pmt::is_dict(msg)) {
        pmt::pmt_t p_pkt_len = pmt::dict_ref(msg, pmt::intern("payload bits"), pmt::PMT_NIL);
        if(!pmt::eq(p_pkt_len, pmt::PMT_NIL)) {
          d_pkt_len = pmt::to_long(p_pkt_len) / d_bps;
        }

        pmt::pmt_t p_to_skip = pmt::dict_ref(msg, pmt::intern("skip samps"), pmt::PMT_NIL);
        if(!pmt::eq(p_to_skip, pmt::PMT_NIL)) {
          d_to_skip = pmt::to_long(p_to_skip);
        }
        d_mode = ST_PROCESSING;
        d_written = 0;
        GR_LOG_DEBUG(d_logger, boost::format("received info;  pkt_len: %1%  skip: %2%") \
                     % d_pkt_len % d_to_skip);
      }
    }

    int
    packet_gateway_impl::bps() const
    {
      return d_bps;
    }

    void
    packet_gateway_impl::set_bps(int bps)
    {
      if(bps > 0) {
        d_bps = bps;
      }
      else {
        throw std::runtime_error("packet_gateway: bps must be > 0");
      }
    }

    int
    packet_gateway_impl::general_work(int noutput_items,
                                      gr_vector_int &ninput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t*)input_items[0];
      uint8_t *out = (uint8_t*)output_items[0];

      uint64_t skip, to_write;
      int to_consume = 0, ret = 0;

      switch(d_mode) {
      case(ST_WAITING):
        d_avail = noutput_items;
        if(d_avail > d_mtu) {
          to_consume = d_avail - d_mtu;
          d_avail = d_mtu;
          d_skipped += to_consume;
        }
        ret = 0;
        //GR_LOG_DEBUG(d_logger, boost::format("skipping: %1%    avail: %2%   nout: %3%") \
        //             % d_skipped % d_avail % noutput_items);
        break;

      case(ST_PROCESSING):
        skip = d_to_skip - d_skipped;
        d_skipped += std::min(skip, (uint64_t)noutput_items);
        //GR_LOG_DEBUG(d_logger, boost::format("skipped: %1%  to skip: %2%  skip: %3%  nout: %4% -> %5%") \
        //             % d_skipped % d_to_skip % skip % noutput_items % (noutput_items-skip));
        if(d_skipped == d_to_skip) {
          to_write = std::min(d_pkt_len - d_written, (uint64_t)(noutput_items-skip));

          memcpy(out, (in + (skip*d_itemsize)), to_write*d_itemsize);
          d_written += to_write;
          if(d_written == d_pkt_len) {
            d_mode = ST_WAITING;
            d_skipped = 0;
            d_avail = 0;
          }

          to_consume = skip + to_write;
          ret = to_write;

          //GR_LOG_DEBUG(d_logger, boost::format("processing;  skipped: %1%  to_write: %2%  written: %3%") \
          //             % d_skipped % to_write % d_written);
          //GR_LOG_DEBUG(d_logger, boost::format("             to consume: %1%  returning: %2%") \
          //             % to_consume % ret);
        }

        if(d_skipped > d_to_skip)
          throw std::runtime_error("EXIT");
        break;
      default:
        throw std::runtime_error("packet_gateway: in unknown state.");
      }

      consume_each(to_consume);
      return ret;
    }

  } /* namespace digital */
} /* namespace gr */
