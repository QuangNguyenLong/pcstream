/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef PCSTREAM_HTTP_H
#define PCSTREAM_HTTP_H

#include <pcstream/buffer.h>
#include <pcstream/def.h>

PCSTREAM_EXPORT pcs_ret_t
pcs_http_get_to_buffer(const char           *url,
                       PCSTREAM_HTTP_VERSION ver,
                       pcs_buffer_t         *des,
                       pcs_bw_t             *dl_speed);

#endif
