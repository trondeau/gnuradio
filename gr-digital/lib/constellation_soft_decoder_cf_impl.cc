/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include "constellation_soft_decoder_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace digital {

    constellation_soft_decoder_cf::sptr
    constellation_soft_decoder_cf::make(constellation_sptr constellation,
                                        bool packetized)
    {
      return gnuradio::get_initial_sptr
	(new constellation_soft_decoder_cf_impl(constellation, packetized));
    }

    constellation_soft_decoder_cf_impl::
    constellation_soft_decoder_cf_impl(constellation_sptr constellation,
                                       bool packetized)
      : block("constellation_soft_decoder_cf",
              io_signature::make(1, 1, sizeof(gr_complex)),
              io_signature::make(0, 1, sizeof(float))),
	d_constellation(constellation),
	d_dim(constellation->dimensionality()),
        d_bps(constellation->bits_per_symbol())
    {
      message_port_register_in(pmt::mp("constellation"));
      set_msg_handler(pmt::mp("constellation"),
                      boost::bind(&constellation_soft_decoder_cf_impl::handle_constellation,
                                  this, _1));
      message_port_register_in(pmt::mp("info"));
      set_msg_handler(pmt::mp("info"),
                      boost::bind(&constellation_soft_decoder_cf_impl::handle_info,
                                  this, _1));
      message_port_register_out(pmt::mp("pdu"));

      d_nbits = 0;
      if(packetized) {
        d_ok = false;
        d_pkt_len = 0;
      }
      else {
        d_ok = true;
        d_pkt_len = -1;
      }

      d_output = NULL;

      set_relative_rate(d_bps);
    }

    constellation_soft_decoder_cf_impl::~constellation_soft_decoder_cf_impl()
    {
      if(d_output) {
        volk_free(d_output);
        d_output = NULL;
      }
    }

    void
    constellation_soft_decoder_cf_impl::forecast(int noutput_items,
                                                 gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size ();
      for(unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = noutput_items  / d_bps + history() - 1;
    }

    void
    constellation_soft_decoder_cf_impl::handle_constellation(pmt::pmt_t constellation_pmt)
    {
      if(pmt::is_any(constellation_pmt)) {
        boost::any constellation_any = pmt::any_ref(constellation_pmt);
        constellation_sptr constellation =
          boost::any_cast<constellation_sptr>(constellation_any);

        d_constellation = constellation;
	d_dim = constellation->dimensionality();
        d_bps = constellation->bits_per_symbol();
      }
    }

    void
    constellation_soft_decoder_cf_impl::handle_info(pmt::pmt_t msg)
    {
      // set signal to start processing
      d_ok = true;

      // get packet length to process
      d_nbits = 0;
      if(pmt::is_dict(msg)) {
        pmt::pmt_t p_pkt_len = pmt::dict_ref(msg, pmt::intern("payload bits"), pmt::PMT_NIL);
        if(!pmt::eq(p_pkt_len, pmt::PMT_NIL)) {
          d_pkt_len = pmt::to_long(p_pkt_len);
        }

        pmt::pmt_t p_hdr_len = pmt::dict_ref(msg, pmt::intern("skip samps"), pmt::PMT_NIL);
        if(!pmt::eq(p_hdr_len, pmt::PMT_NIL)) {
          d_hdr_len = pmt::to_long(p_hdr_len);
        }
      }

      if(d_output) {
        volk_free(d_output);
        d_output = NULL;
      }
      d_output = (float*)volk_malloc(d_pkt_len*sizeof(float),
                                     volk_get_alignment());
      memset(d_output, 0, d_pkt_len*sizeof(float));
    }

    int
    constellation_soft_decoder_cf_impl::general_work(int noutput_items,
                                                     gr_vector_int &ninput_items,
                                                     gr_vector_const_void_star &input_items,
                                                     gr_vector_void_star &output_items)
    {
      // FIXME: figure out how to manage d_dim

      std::vector<float> bits;
      int nbits = 0, nsamps = 0;
      gr_complex const *in = (const gr_complex*)input_items[0];

      // Process as an infinite stream and produce on the output stream port.
      if(d_pkt_len == -1) {
        float *out = (float*)output_items[0];
        nbits = noutput_items;
        nsamps = nbits / d_bps;
        for(int i = 0; i < nsamps; i++) {
          bits = d_constellation->soft_decision_maker(in[i]);
          for(size_t j = 0; j < bits.size(); j++) {
            out[d_bps*i+j] = bits[j];
          }
        }
        consume_each(nsamps);
        return nbits;
      }

      // Process as a finite packet and produce a PDU
      else {
        if(d_ok) {
          // Eat all skipped samples that the packet parser told us
          // about. After this, we are at the payload samples.
          if(d_hdr_len > 0) {
            nbits = std::min(noutput_items, d_hdr_len);
            d_hdr_len -= nbits;
            consume_each(nbits);
            return 0;
          }

          nbits = std::min(noutput_items, d_pkt_len-d_nbits);
          nsamps = nbits / d_bps;
          for(int i = 0; i < nsamps; i++) {
            bits = d_constellation->soft_decision_maker(in[i]);
            //gr_complex sym = in[i]*gr_complex(0, -1);
            //bits = d_constellation->soft_decision_maker(sym);
            for(size_t j = 0; j < bits.size(); j++) {
              d_output[d_nbits] = bits[j];
              d_nbits++;
            }
          }

          if(d_pkt_len == d_nbits) {
            pmt::pmt_t output = pmt::init_f32vector(d_pkt_len, d_output);
            pmt::pmt_t info = pmt::make_dict();
            info = pmt::dict_add(info, pmt::intern("payload len"),
                                 pmt::from_long(d_pkt_len));
            pmt::pmt_t msg_pair = pmt::cons(info, output);
            message_port_pub(pmt::mp("pdu"), msg_pair);

            // so we don't go again until a new info message arrives
            d_ok = false;
          }
          consume_each(nsamps);
          return 0;
        }
        else {
          consume_each(0);
          return 0;
        }
      }
    }

  } /* namespace digital */
} /* namespace gr */
