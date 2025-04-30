#ifndef _PLY_H
#define _PLY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

  uint32_t ply_count_vertex(const char *filename);
  uint32_t ply_count_face(const char *filename);
  int      ply_pointcloud_loader(const char    *filename,
                                 float         *pos,
                                 unsigned char *rgb);
  int      ply_mesh_loader(const char *filename,
                           float      *pos,
                           uint32_t   *indices);

  int      ply_mesh_writer(const char *filename,
                           float      *pos,
                           uint32_t    num_verts,
                           uint32_t   *indices,
                           uint32_t    num_indices,
                           int         binary);
#ifdef __cplusplus
}
#endif
#endif // _PLY_H
