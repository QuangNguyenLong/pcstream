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
#include <curl/curl.h>
#include <pcstream/buffer.h>
#include "http.h"

#include <stdlib.h> /* for realloc */
#include <string.h> /* for memcpy */

static size_t
write_callback(void *data, size_t size, size_t nmemb, void *userp)
{
  size_t        total_size = 0;
  pcs_buffer_t *buf        = PCSTREAM_NULL;
  char         *ptr        = PCSTREAM_NULL;

  total_size               = size * nmemb;
  buf                      = (pcs_buffer_t *)userp;

  ptr = realloc(buf->data, buf->size + total_size + 1);
  if (ptr == NULL)
  {
    return 0; // out of memory
  }
  buf->data = ptr;
  memcpy(&(buf->data[buf->size]), data, total_size);
  buf->size += total_size;
  buf->data[buf->size] = '\0';

  return total_size;
}

pcs_ret_t pcs_http_get_to_buffer(const char           *url,
                                 PCSTREAM_HTTP_VERSION ver,
                                 pcs_buffer_t         *des,
                                 pcs_bw_t             *dl_speed)
{
  CURL    *curl = PCSTREAM_NULL;
  CURLcode res  = 0;

  curl          = curl_easy_init();

  if (!curl)
  {
    return PCSTREAM_RET_FAIL;
  }

  curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.81.0");
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)des);

  // remove self before doing real stuff
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

  // Set HTTP version
  if (ver == PCSTREAM_HTTP_1_1)
  {
    curl_easy_setopt(
        curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
  }
  else if (ver == PCSTREAM_HTTP_2_0)
  {
    curl_easy_setopt(
        curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
  }

  res = curl_easy_perform(curl);

  if (dl_speed != PCSTREAM_NULL)
  {
    curl_easy_getinfo(
        curl, CURLINFO_SPEED_DOWNLOAD_T, (curl_off_t *)dl_speed);
  }
  curl_easy_cleanup(curl);

  if (res != CURLE_OK)
  {
    pcs_buffer_destroy(des);
    return PCSTREAM_RET_FAIL;
  }

  return PCSTREAM_RET_SUCCESS;
}
