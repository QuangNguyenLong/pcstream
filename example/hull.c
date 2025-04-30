#include <pcstream/buffer.h>
#include <pcstream/mesh.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- usage & exit helpers -------------- */
static void print_usage(const char *prog)
{
    fprintf(stderr,
        "Usage:\n"
        "  %s ply2bin <input.ply> <output.bin>\n"
        "  %s bin2ply <input.bin> <output.ply>\n",
        prog, prog);
    exit(EXIT_FAILURE);
}

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int read_bin_file(const char *path, void **bufOut, size_t *sizeOut)
{
  FILE *fp = fopen(path, "rb");
  if (!fp)
    return -1;

  /* obtain file size */
  if (fseek(fp, 0, SEEK_END) != 0)
  {
    fclose(fp);
    return -1;
  }
  long fsz = ftell(fp);
  if (fsz < 0)
  {
    fclose(fp);
    return -1;
  }
  rewind(fp);

  void *buf = malloc((size_t)fsz);
  if (!buf)
  {
    fclose(fp);
    return -1;
  }

  /* read entire file */
  if (fread(buf, 1, (size_t)fsz, fp) != (size_t)fsz)
  {
    free(buf);
    fclose(fp);
    return -1;
  }

  fclose(fp);
  *bufOut  = buf;
  *sizeOut = (size_t)fsz;
  return 0;
}

int write_bin_file(const char *path, const void *buf, size_t size)
{
  FILE *fp = fopen(path, "wb");
  if (!fp)
    return -1;

  if (fwrite(buf, 1, size, fp) != size)
  {
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0;
}


static void cmd_ply2bin(const char *in_ply, const char *out_bin)
{
      pcs_mesh_t   m        = {0};
      pcs_buffer_t out_buff = {0};

      pcs_mesh_init(&m);
      pcs_buffer_init(&out_buff);

      m.read_from_file_ply(&m, in_ply);
      m.write_to_buff_serial(&m, &out_buff.data, &out_buff.size);

      if(write_bin_file(out_bin, out_buff.data, out_buff.size) != 0)
          die("write bin file");
      pcs_buffer_destroy(&out_buff);
      pcs_mesh_destroy(&m);
}

static void cmd_bin2ply(const char *in_bin,
                        const char *out_ply,
                        int ascii_flag)
{
    pcs_mesh_t m = {0};
    pcs_buffer_t in_buff = {0};
    PCSTREAM_BOOL is_bin = 0;

    is_bin = ascii_flag == 0 ? PCSTREAM_TRUE : PCSTREAM_FALSE;

    pcs_mesh_init(&m);
    pcs_buffer_init(&in_buff);

    if (read_bin_file(in_bin, (void **)&in_buff.data, &in_buff.size) != 0)
        die("read_bin_file");
    m.read_from_buff_serial(&m, in_buff.data, in_buff.size);

    m.write_to_file_ply(&m, out_ply, is_bin);

    pcs_buffer_destroy(&in_buff);
    pcs_mesh_destroy(&m);
}

/* ------------------ main -------------------- */
int main(int argc, char **argv)
{
    if (argc < 4)                 /* minimal arg count */
        print_usage(argv[0]);

    const char *cmd = argv[1];

    if (strcmp(cmd, "ply2bin") == 0) {
        if (argc != 4) print_usage(argv[0]);
        cmd_ply2bin(argv[2], argv[3]);
    }
    else if (strcmp(cmd, "bin2ply") == 0) {
        int ascii = 0;
        const char *in_bin  = NULL;
        const char *out_ply = NULL;

        /* crude flag parsing: only --ascii is recognised */
        int argi = 2;
        for (; argi < argc; ++argi) {
            if (strcmp(argv[argi], "--ascii") == 0)
                ascii = 1;
            else if (!in_bin)
                in_bin = argv[argi];
            else if (!out_ply)
                out_ply = argv[argi];
            else
                print_usage(argv[0]);
        }
        if (!in_bin || !out_ply) print_usage(argv[0]);

        cmd_bin2ply(in_bin, out_ply, ascii);
    }
    else {
        print_usage(argv[0]);
    }

    return EXIT_SUCCESS;
}
