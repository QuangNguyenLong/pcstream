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
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genmpd.h"

const char *argp_program_version     = "genmpd 0.1";
const char *argp_program_bug_address = "<quang.nglong@gmail.com>";
static char doc[]      = "Generate a DASH MPD manifest using GPAC.";
static char args_doc[] = "";

static struct argp_option options[] = {
    {"duration", 'd', "INT", 0, "Total video duration in ms"},
    {"rep-count", 'r', "INT", 0, "Number of representations"},
    {"seq-count", 's', "INT", 0, "Number of adaptation sets"},
    {"profile", 'p', "STRING", 0, "MPD profile"},
    {"min-buffer-time", 30, "INT", 0, "Minimum buffer time in ms"},
    {"mime-type", 31, "STRING", 0, "mimeType of the adaptation sets"},
    {"fps", 'f', "INT", 0, "Frames per second"},
    {"segment-duration",
     't', "INT",
     0, "Segment duration (in timescale units)"},
    {"codec", 'c', "STRING", 0, "Codec used for video compression"},
    {"output", 'o', "FILE", 0, "Output MPD file"},
    {"media-template", 'm', "FILE", 0, "Segment media file template"},
    {"init-template",
     'i', "FILE",
     0, "Initialization segment template"},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *args = state->input;
  switch (key)
  {
  case 'd':
    args->duration = atoi(arg);
    break;
  case 'r':
    args->rep_count = atoi(arg);
    break;
  case 's':
    args->seq_count = atoi(arg);
    break;
  case 'f':
    args->fps = atoi(arg);
    break;
  case 't':
    args->segment_duration = atoi(arg);
    break;
  case 'o':
    args->output_file = arg;
    break;
  case 'm':
    args->media_template = arg;
    break;
  case 'i':
    args->init_template = arg;
    break;
  case 'c':
    args->video_codec = arg;
    break;
  case 'p':
    args->profile = arg;
    break;
  case 30:
    args->min_buffer_time = atoi(arg);
    break;
  case 31:
    args->mime_type = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

// ==== MAIN ====
int                main(int argc, char **argv)
{
  struct arguments args = {
      .duration         = DEFAULT_DURATION,
      .rep_count        = DEFAULT_REP_COUNT,
      .seq_count        = DEFAULT_SEQ_COUNT,
      .min_buffer_time  = DEFAULT_MIN_BUFFER_TIME,
      .fps              = DEFAULT_FPS,
      .segment_duration = DEFAULT_SEGMENT_DURATION,
      .output_file      = DEFAULT_OUTPUT_FILE,
      .media_template =
          "$AdaptationSetID$.seg$Number%05d$.r$RepresentationID$.bin",
      .init_template =
          "$AdaptationSetID$.init.r$RepresentationID$.bin",
      .video_codec = "vpcc",
      .profile     = "urn:mpeg:dash:profile:full:2011",
      .mime_type   = "video/mp5"};

  argp_parse(&argp, argc, argv, 0, 0, &args);

  genmpd(args);

  return 0;
}
