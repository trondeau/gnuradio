/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

%template(packet_header_default_sptr) boost::shared_ptr<gr::digital::packet_header_default>;
%pythoncode %{
packet_header_default_sptr.__repr__ = lambda self: "<packet_header_default>"
packet_header_default = packet_header_default .make;
%}

%template(packet_header_ofdm_sptr) boost::shared_ptr<gr::digital::packet_header_ofdm>;
%pythoncode %{
packet_header_ofdm_sptr.__repr__ = lambda self: "<packet_header_ofdm>"
packet_header_ofdm = packet_header_ofdm .make;
%}

%template(packet_formatter_base_sptr) boost::shared_ptr<gr::digital::packet_formatter_base>;

%template(packet_formatter_default_sptr) boost::shared_ptr<gr::digital::packet_formatter_default>;
%pythoncode %{
packet_formatter_default_sptr.__repr__ = lambda self: "<packet_formatter_default>"
packet_formatter_default = packet_formatter_default .make;
%}


%template(packet_formatter_counter_sptr) boost::shared_ptr<gr::digital::packet_formatter_counter>;
%pythoncode %{
packet_formatter_counter_sptr.__repr__ = lambda self: "<packet_formatter_counter>"
packet_formatter_counter = packet_formatter_counter .make;
%}

%template(packet_formatter_crc_sptr) boost::shared_ptr<gr::digital::packet_formatter_crc>;
%pythoncode %{
packet_formatter_crc_sptr.__repr__ = lambda self: "<packet_formatter_crc>"
packet_formatter_crc = packet_formatter_crc .make;
%}
