#ifndef PCSTREAM_MESH_H
#define PCSTREAM_MESH_H

#include <pcstream/def.h>
#include <stdint.h>
typedef struct pcs_mesh_t pcs_mesh_t;

struct pcs_mesh_t
{
  float    *pos;
  uint32_t  num_verts;
  uint32_t *indices;
  uint32_t  num_indices;

  PCSTREAM_RET(*read_from_buff_serial)
  (pcs_mesh_t *, const char *, PCSTREAM_COUNT);
  PCSTREAM_RET(*write_to_buff_serial)
  (pcs_mesh_t *, char **, PCSTREAM_COUNT *);

  PCSTREAM_RET(*write_to_file_ply)
  (pcs_mesh_t *, const char *, PCSTREAM_BOOL);
  PCSTREAM_RET(*read_from_file_ply)
  (pcs_mesh_t *, const char *);

  PCSTREAM_RET(*screen_ratio)
  (pcs_mesh_t *, const float *, float *);
};

PCSTREAM_RET pcs_mesh_init(pcs_mesh_t *self);
PCSTREAM_RET pcs_mesh_destroy(pcs_mesh_t *self);

PCSTREAM_RET
pcs_mesh_read_from_buff_serial(pcs_mesh_t *self,
                               const char    *data,
                               PCSTREAM_COUNT size);
PCSTREAM_RET
pcs_mesh_write_to_buff_serial(pcs_mesh_t *self,
                              char          **data_out,
                              PCSTREAM_COUNT *size_out);

PCSTREAM_RET
pcs_mesh_read_from_file_ply(pcs_mesh_t *self, const char *filepath);
PCSTREAM_RET
pcs_mesh_write_to_file_ply(pcs_mesh_t *self,
                           const char   *filepath,
                           PCSTREAM_BOOL binary);

PCSTREAM_RET
pcs_mesh_screen_ratio(pcs_mesh_t *self,
                      const float *mvp,
                      float       *screen_ratio);

#endif
