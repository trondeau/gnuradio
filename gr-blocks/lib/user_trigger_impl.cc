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

#include "user_trigger_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace blocks {

    user_trigger::sptr
    user_trigger::make()
    {
      return gnuradio::get_initial_sptr
        (new user_trigger_impl());
    }

    user_trigger_impl::user_trigger_impl()
      : block("user_trigger",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0))
    {
      message_port_register_out(pmt::mp("trig"));
    }

    user_trigger_impl::~user_trigger_impl()
    {
    }

    void
    user_trigger_impl::trigger(pmt::pmt_t trig)
    {
      message_port_pub(pmt::mp("trig"), trig);
    }

  } /* namespace blocks */
} /* namespace gr */
