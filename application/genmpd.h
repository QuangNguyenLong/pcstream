#include <errno.h>
#include <glob.h>
#include <gpac/list.h>
#include <gpac/mpd.h>
#include <gpac/tools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// ==== Defaults ====
#define DEFAULT_DURATION         10000
#define DEFAULT_REP_COUNT        5
#define DEFAULT_SEQ_COUNT        10
#define DEFAULT_FPS              30
#define DEFAULT_MIN_BUFFER_TIME  1000
#define DEFAULT_SEGMENT_DURATION 30
#define DEFAULT_OUTPUT_FILE      "output.mpd"
#define SEGMENT_SIZE_ESTIMATE \
  100000 // bytes per segment (100KB estimate)

#define MAX_PATH_LEN 512

struct arguments
{
  u64         duration;
  u32         rep_count;
  u32         seq_count;
  u32         min_buffer_time;
  u32         fps;
  u32         segment_duration;
  const char *output_file;
  const char *media_template;
  const char *init_template;
  const char *video_codec;
  const char *profile;
  const char *mime_type;
};

long get_file_size(const char *filename)
{
  struct stat st = {0};
  if (stat(filename, &st) == 0)
    return st.st_size;
  return -1;
}

void substitute_placeholders(const char *template,
                             char *output,
                             int   AdaptationSetID,
                             int   RepresentationID)
{
  const char *p   = template;
  char       *out = NULL;
  out             = output;

  while (*p)
  {
    if (strncmp(
            p, "$AdaptationSetID$", strlen("$AdaptationSetID$")) == 0)
    {
      out += sprintf(out, "%d", AdaptationSetID);
      p += strlen("$AdaptationSetID$");
    }
    else if (strncmp(p,
                     "$RepresentationID$",
                     strlen("$RepresentationID$")) == 0)
    {
      out += sprintf(out, "%d", RepresentationID);
      p += strlen("$RepresentationID$");
    }
    else
    {
      *out++ = *p++;
    }
  }
  *out = '\0';
}

int calculate_total_bytes(const char *template,
                          int  AdaptationSetID,
                          int  RepresentationID,
                          int *seg_count)
{
  char  format_string[1024] = {0};
  char  fixed_format[1024]  = {0};
  char *in                  = NULL;
  char *out                 = NULL;
  int   total_bytes         = 0;
  int   number              = 0;
  char  final_path[1024]    = {0};
  long  size                = 0;
  int   cmp                 = 0;

  substitute_placeholders(
      template, format_string, AdaptationSetID, RepresentationID);

  printf("%s\n", format_string);

  in  = format_string;
  out = fixed_format;
  while (*in)
  {
    if (strncmp(in, "$Number%05d$", strlen("$Number%05d$")) == 0)
    {
      out += sprintf(out, "%%05d");
      in += strlen("$Number%05d$");
    }
    else
    {
      *out++ = *in++;
    }
  }
  *out = '\0';

  printf("Final format string after substitution: %s\n",
         fixed_format);

  total_bytes = 0;
  number      = 1;

  while (1)
  {
    snprintf(final_path, sizeof(final_path), fixed_format, number);

    cmp  = strcmp(final_path, fixed_format);

    size = get_file_size(final_path);
    if (size > 0)
    {
      total_bytes += size;
      number++;
    }

    if (size < 0 || cmp == 0)
      break;
  }
  if (seg_count != NULL)
    *seg_count = number;

  return total_bytes;
}

unsigned int genmpd(struct arguments args)
{
  u32                     timescale     = 0;
  u32                     segments      = 0;
  u32                     last_duration = 0;
  GF_MPD                 *mpd           = NULL;
  GF_MPD_Period          *period        = NULL;
  char                    codec_buf[32] = {0};
  char                    id_buf[16]    = {0};
  int                     seg_count     = 0;
  int                     total_byte    = 0;
  // stuff need to be free
  GF_MPD_AdaptationSet  **aset          = NULL;
  GF_MPD_Representation **rep           = NULL;

  timescale                             = args.fps;
  segments                              = (args.duration / 1000.0) /
             (args.segment_duration / (float)timescale);
  last_duration        = (args.duration % 1000) * timescale / 1000;

  mpd                  = gf_mpd_new();
  mpd->profiles        = gf_strdup(args.profile);
  mpd->min_buffer_time = args.min_buffer_time;
  mpd->periods         = gf_list_new();
  mpd->media_presentation_duration = args.duration;
  period                           = gf_mpd_period_new();
  // period->duration     = args.duration;

  aset = malloc(sizeof(GF_MPD_AdaptationSet *) * args.seq_count);
  rep  = malloc(sizeof(GF_MPD_Representation *) * args.rep_count);

  for (int seq = 0; seq < args.seq_count; seq++)
  {
    aset[seq]                      = gf_mpd_adaptation_set_new();
    aset[seq]->id                  = seq;
    aset[seq]->mime_type           = gf_strdup(args.mime_type);
    // aset[seq]->content_type        = gf_strdup("video");
    aset[seq]->segment_alignment   = GF_TRUE;
    aset[seq]->bitstream_switching = GF_TRUE;
    aset[seq]->representations     = gf_list_new();

    for (int i = 0; i < args.rep_count; i++)
    {
      GF_MPD_SegmentTemplate      *templ    = NULL;
      GF_MPD_SegmentTimeline      *timeline = NULL;
      GF_MPD_SegmentTimelineEntry *entry    = NULL;

      rep[i] = gf_mpd_representation_new();
      sprintf(id_buf, "%d", i);
      rep[i]->id  = gf_strdup(id_buf);
      rep[i]->fps = args.fps;

      // Estimate bandwidth: size (bits) / duration (seconds)
      total_byte =
          calculate_total_bytes(
              args.media_template, seq, i, &seg_count) +
          calculate_total_bytes(args.init_template, seq, i, NULL);

      rep[i]->bandwidth = total_byte / (seg_count + 1) /
                          (args.segment_duration / timescale);
      // rep[i]->mime_type = gf_strdup("video/mp5");

      sprintf(codec_buf, "%s.%d", args.video_codec, i);
      rep[i]->codecs = gf_strdup(codec_buf);

      templ          = malloc(sizeof(GF_MPD_SegmentTemplate));
      memset(templ, 0, sizeof(GF_MPD_SegmentTemplate));
      templ->timescale      = timescale;
      templ->initialization = gf_strdup(args.init_template);
      templ->media          = gf_strdup(args.media_template);
      templ->start_number   = 1;

      timeline              = gf_mpd_segmentimeline_new();
      timeline->entries     = gf_list_new();

      entry             = malloc(sizeof(GF_MPD_SegmentTimelineEntry));
      entry->start_time = 0;
      entry->duration   = args.segment_duration;
      entry->repeat_count = segments - 2; // minus init and itself
      // entry->nb_parts     = 0;
      gf_list_add(timeline->entries, entry);

      if (last_duration > 0)
      {
        GF_MPD_SegmentTimelineEntry *last = NULL;

        last = malloc(sizeof(GF_MPD_SegmentTimelineEntry));
        last->start_time   = args.segment_duration * segments;
        last->duration     = last_duration;
        last->repeat_count = 0;
        gf_list_add(timeline->entries, last);
      }

      templ->segment_timeline  = timeline;
      rep[i]->segment_template = templ;

      gf_list_add(aset[seq]->representations, rep[i]);
    }

    if (!period->adaptation_sets)
      period->adaptation_sets = gf_list_new();

    gf_list_add(period->adaptation_sets, aset[seq]);
  }

  gf_list_add(mpd->periods, period);
  gf_mpd_write_file(mpd, args.output_file);

  gf_mpd_del(mpd);
  free(aset);
  free(rep);
}
