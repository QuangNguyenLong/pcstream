#include <pcstream/mesh.h>
#include <pcstream/vec2f.h>
#include <pcstream/vec3f.h>
#include <ply.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
static pcs_ret_t pcs_mesh_free_s(pcs_mesh_t *mesh)
{
  if (mesh->pos != PCSTREAM_NULL)
  {
    free(mesh->pos);
  }
  if (mesh->indices != PCSTREAM_NULL)
  {
    free(mesh->indices);
  }

  mesh->pos         = PCSTREAM_NULL;
  mesh->num_verts   = 0;
  mesh->indices     = PCSTREAM_NULL;
  mesh->num_indices = 0;
  return PCSTREAM_RET_SUCCESS;
}

static pcs_ret_t pcs_mesh_alloc_s(pcs_mesh_t *mesh,
                                  uint32_t    num_verts,
                                  uint32_t    num_indices)
{
  if (mesh != PCSTREAM_NULL)
  {
    pcs_mesh_free_s(mesh);
  }

  mesh->pos       = (float *)malloc(sizeof(float) * 3 * num_verts);
  mesh->num_verts = num_verts;
  mesh->indices = (uint32_t *)malloc(sizeof(uint32_t) * num_indices);
  mesh->num_indices = num_indices;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_mesh_init(pcs_mesh_t *self)
{
  self->pos                   = PCSTREAM_NULL;
  self->num_verts             = 0;
  self->indices               = PCSTREAM_NULL;
  self->num_indices           = 0;

  self->read_from_buff_serial = pcs_mesh_read_from_buff_serial;
  self->write_to_buff_serial  = pcs_mesh_write_to_buff_serial;

  self->write_to_file_ply     = pcs_mesh_write_to_file_ply;
  self->read_from_file_ply    = pcs_mesh_read_from_file_ply;

  self->screen_ratio          = pcs_mesh_screen_ratio;

  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_mesh_destroy(pcs_mesh_t *self)
{
  pcs_mesh_free_s(self);
  *self = (pcs_mesh_t){0};
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_mesh_read_from_buff_serial(pcs_mesh_t *self,
                                         const char *data,
                                         pcs_count_t size)
{
  const uint8_t *curr     = (const uint8_t *)data;
  const uint8_t *end      = curr + size;
  uint32_t       numv     = 0;
  uint32_t       numi     = 0;
  size_t         pos_size = 0;
  size_t         idx_size = 0;

  if (self == PCSTREAM_NULL || data == PCSTREAM_NULL)
  {
    return PCSTREAM_RET_FAIL;
  }

  pcs_mesh_free_s(self);

  /* ---------- read vertex count ----------------------------- */
  if ((end - curr) < (ptrdiff_t)sizeof(uint32_t))
  {
    return PCSTREAM_RET_FAIL;
  }

  memcpy(&numv, curr, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  if (numv == 0)
  {
    return PCSTREAM_RET_FAIL;
  }

  self->num_verts = numv;

  /* ---------- read vertex positions ------------------------- */
  pos_size        = (size_t)numv * 3U * sizeof(float);

  if ((end - curr) < (ptrdiff_t)pos_size)
  {
    return PCSTREAM_RET_FAIL;
  }

  self->pos = (float *)malloc(pos_size);
  if (self->pos == PCSTREAM_NULL)
  {
    return PCSTREAM_RET_FAIL;
  }
  memcpy(self->pos, curr, pos_size);
  curr += pos_size;

  /* ---------- read index count ------------------------------ */
  if ((end - curr) < (ptrdiff_t)sizeof(uint32_t))
  {
    pcs_mesh_free_s(self);
    return PCSTREAM_RET_FAIL;
  }

  memcpy(&numi, curr, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  if (numi == 0)
  {
    pcs_mesh_free_s(self);
    return PCSTREAM_RET_FAIL;
  }

  self->num_indices = numi;

  /* ---------- read indices array ---------------------------- */
  idx_size          = (size_t)numi * sizeof(uint32_t);

  if ((end - curr) < (ptrdiff_t)idx_size)
  {
    pcs_mesh_free_s(self);
    return PCSTREAM_RET_FAIL;
  }

  self->indices = (uint32_t *)malloc(idx_size);
  if (self->indices == PCSTREAM_NULL)
  {
    pcs_mesh_free_s(self);
    return PCSTREAM_RET_FAIL;
  }

  memcpy(self->indices, curr, idx_size);

  return PCSTREAM_RET_SUCCESS;
}

static inline pcs_count_t
pcs_mesh_serial_size(const pcs_mesh_t *mesh)
{
  return sizeof(uint32_t) +                     /* num verts  */
         3U * mesh->num_verts * sizeof(float) + /* positions  */
         sizeof(uint32_t) +                     /* num idx    */
         mesh->num_indices * sizeof(uint32_t);  /* indices    */
}
pcs_ret_t pcs_mesh_write_to_buff_serial(pcs_mesh_t  *self,
                                        char       **data_out,
                                        pcs_count_t *size_out)
{
  pcs_count_t need     = 0;
  size_t      pos_size = 0;
  size_t      idx_size = 0;
  uint8_t    *buf      = PCSTREAM_NULL;
  uint8_t    *curr     = PCSTREAM_NULL;

  if (!self || !data_out || !size_out)
  {
    return PCSTREAM_RET_FAIL;
  }

  if (self->num_verts == 0 || self->num_indices == 0 || !self->pos ||
      !self->indices)
  {
    return PCSTREAM_RET_FAIL;
  }

  need     = pcs_mesh_serial_size(self);
  pos_size = (size_t)self->num_verts * 3U * sizeof(float);
  idx_size = (size_t)self->num_indices * sizeof(uint32_t);

  buf      = (uint8_t *)malloc((size_t)need);
  if (!buf)
  {
    return PCSTREAM_RET_FAIL;
  }

  curr = buf;

  memcpy(curr, &self->num_verts, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  memcpy(curr, self->pos, pos_size);
  curr += pos_size;

  memcpy(curr, &self->num_indices, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  memcpy(curr, self->indices, idx_size);

  *data_out = (char *)buf;
  *size_out = need;
  return PCSTREAM_RET_SUCCESS;
}

pcs_ret_t pcs_mesh_read_from_file_ply(pcs_mesh_t *self,
                                      const char *filepath)
{
  uint32_t numv = 0;
  uint32_t numf = 0;
  int      ret  = 0;
  if (self != PCSTREAM_NULL)
  {
    pcs_mesh_free_s(self);
  }

  numv = (uint32_t)ply_count_vertex(filepath);
  numf = (uint32_t)ply_count_face(filepath);
  pcs_mesh_alloc_s(self, numv, numf * 3);
  ret = ply_mesh_loader(filepath, self->pos, self->indices);
  if (ret == 0)
  {
    return PCSTREAM_RET_FAIL;
  }
  return PCSTREAM_RET_SUCCESS;
}
pcs_ret_t pcs_mesh_write_to_file_ply(pcs_mesh_t *self,
                                     const char *filepath,
                                     pcs_bool_t  binary)
{
  int ret = 0;
  if (!self || !filepath)
  {
    return PCSTREAM_RET_FAIL;
  }
  ret = ply_mesh_writer(filepath,
                        self->pos,
                        self->num_verts,
                        self->indices,
                        self->num_indices,
                        binary == PCSTREAM_TRUE ? 1 : 0);
  if (ret == 0)
  {
    return PCSTREAM_RET_FAIL;
  }
  return PCSTREAM_RET_SUCCESS;
}
static int
is_toward_s(pcs_vec2f_t veca, pcs_vec2f_t vecb, pcs_vec2f_t vecc)
{
  return (vecb.x - veca.x) * (vecc.y - veca.y) >
         (vecc.x - veca.x) * (vecb.y - veca.y);
}

static float polygon_area_s(pcs_vec2f_t *points, int num)
{
  float area = 0.0F;

  if (num < 3)
  {
    return 0.0F; // No valid area if less than 3 points
  }
  for (int i = 0, j = 0; i < num; i++)
  {
    j = (i + 1) % num;
    area += points[i].x * points[j].y - points[j].x * points[i].y;
  }
  return PCSTREAM_FLOAT_HALF * fabsf(area);
}
static int inside_s(pcs_vec2f_t vecp, int edge)
{
  switch (edge)
  {
  case 0:
    return vecp.x >= -1; // Left edge
  case 1:
    return vecp.x <= 1; // Right edge
  case 2:
    return vecp.y >= -1; // Bottom edge
  case 3:
    return vecp.y <= 1; // Top edge
  }
  return 0;
}

static pcs_vec2f_t
intersection_s(pcs_vec2f_t vec1, pcs_vec2f_t vec2, int edge)
{
  pcs_vec2f_t inter = {0};
  float       mesh  = 0.0F;
  if (float_equal(vec1.x, vec2.x))
  {
    mesh = PCSTREAM_FLOAT_ERROR; // Avoid division by zero for
                                 // vertical lines
  }
  else
  {
    mesh = (vec2.y - vec1.y) / (vec2.x - vec1.x);
  }
  switch (edge)
  {
  case 0: // Left edge (x = -1)
    inter.x = -1;
    inter.y = vec1.y + mesh * (-1 - vec1.x);
    break;
  case 1: // Right edge (x = 1)
    inter.x = 1;
    inter.y = vec1.y + mesh * (1 - vec1.x);
    break;
  case 2: // Bottom edge (y = -1)
    inter.y = -1;
    inter.x = vec1.x + (inter.y - vec1.y) / mesh;
    break;
  case 3: // Top edge (y = 1)
    inter.y = 1;
    inter.x = vec1.x + (inter.y - vec1.y) / mesh;
    break;
  }
  return inter;
}

static int clip_polygon_s(pcs_vec2f_t *vec_in,
                          int          in_len,
                          pcs_vec2f_t *vec_out,
                          int          edge)
{
  int         out_len     = 0;
  pcs_vec2f_t prev        = {0};
  int         prev_inside = 0;
  int         idx         = 0;
  pcs_vec2f_t curr        = {0};
  int         curr_inside = 0;

  if (in_len == 0)
  {
    return 0;
  }
  prev        = vec_in[in_len - 1];
  prev_inside = inside_s(prev, edge);

  for (idx = 0; idx < in_len; idx++)
  {
    curr        = vec_in[idx];
    curr_inside = inside_s(curr, edge);

    if (curr_inside)
    {
      if (!prev_inside)
      {
        vec_out[out_len++] = intersection_s(
            prev, curr, edge); // Entering intersection
      }
      vec_out[out_len++] = curr; // Current point is inside
    }
    else if (prev_inside)
    {
      vec_out[out_len++] =
          intersection_s(prev, curr, edge); // Exiting intersection
    }
    prev        = curr;
    prev_inside = curr_inside;
  }
  return out_len;
}

static float clipped_triangle_area_s(pcs_vec2f_t vec1,
                                     pcs_vec2f_t vec2,
                                     pcs_vec2f_t vec3)
{
  pcs_vec2f_t polygon[10]  = {0};
  int         polygon_size = 0;
  pcs_vec2f_t temp[10]     = {0};
  int         edge         = 0;
  int         idx          = 0;
  polygon[0]               = vec1;
  polygon[1]               = vec2;
  polygon[2]               = vec3;
  polygon_size             = 3;

  for (edge = 0; edge < 4; edge++)
  {
    polygon_size = clip_polygon_s(polygon, polygon_size, temp, edge);
    for (idx = 0; idx < polygon_size; idx++)
    {
      polygon[idx] = temp[idx]; // Copy back
    }
  }
  return polygon_area_s(polygon, polygon_size);
}

pcs_ret_t pcs_mesh_screen_ratio(pcs_mesh_t  *self,
                                const float *mvp,
                                float       *screen_ratio)
{
  pcs_vec3f_t *vertices = PCSTREAM_NULL;
  // need to be freed
  pcs_vec3f_t *ndcs     = PCSTREAM_NULL;

  *screen_ratio         = 0;
  vertices              = (pcs_vec3f_t *)self->pos;
  ndcs =
      (pcs_vec3f_t *)malloc(sizeof(pcs_vec3f_t) * self->num_verts);
  for (uint32_t i = 0; i < self->num_verts; i++)
  {
    ndcs[i] = pcs_vec3f_mvp_mul(vertices[i], mvp);
  }

  for (uint32_t i = 0; i < self->num_indices / 3; i++)
  {
    uint32_t idx0 = self->indices[i * 3];
    uint32_t idx1 = self->indices[i * 3 + 1];
    uint32_t idx2 = self->indices[i * 3 + 2];
    if (ndcs[idx0].z >= 0 && ndcs[idx0].z <= 1 &&
        ndcs[idx1].z >= 0 && ndcs[idx1].z <= 1 &&
        ndcs[idx2].z >= 0 && ndcs[idx2].z <= 1 &&
        is_toward_s((pcs_vec2f_t){ndcs[idx0].x, ndcs[idx0].y},
                    (pcs_vec2f_t){ndcs[idx1].x, ndcs[idx1].y},
                    (pcs_vec2f_t){ndcs[idx2].x, ndcs[idx2].y}))
    {
      *screen_ratio += clipped_triangle_area_s(
          (pcs_vec2f_t){ndcs[idx0].x, ndcs[idx0].y},
          (pcs_vec2f_t){ndcs[idx1].x, ndcs[idx1].y},
          (pcs_vec2f_t){ndcs[idx2].x, ndcs[idx2].y});
    }
  }
  // since the screen in ndc is a 2 x 2 square
  *screen_ratio /= 4;
  free(ndcs);
  return PCSTREAM_RET_SUCCESS;
}
