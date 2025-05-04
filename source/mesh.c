#include <pcstream/mesh.h>
#include <pcstream/vec2f.h>
#include <pcstream/vec3f.h>
#include <ply.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
static PCSTREAM_RET _pcs_mesh_free(pcs_mesh_t *m)
{
  if (m->pos != PCSTREAM_NULL)
    free(m->pos);
  if (m->indices != PCSTREAM_NULL)
    free(m->indices);

  m->pos         = PCSTREAM_NULL;
  m->num_verts   = 0;
  m->indices     = PCSTREAM_NULL;
  m->num_indices = 0;
  return PCSTREAM_RET_SUCCESS;
}

static PCSTREAM_RET _pcs_mesh_alloc(pcs_mesh_t *m,
                                    uint32_t    num_verts,
                                    uint32_t    num_indices)
{
  if (m != PCSTREAM_NULL)
    _pcs_mesh_free(m);

  m->pos         = (float *)malloc(sizeof(float) * 3 * num_verts);
  m->num_verts   = num_verts;
  m->indices     = (uint32_t *)malloc(sizeof(uint32_t) * num_indices);
  m->num_indices = num_indices;
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET pcs_mesh_init(pcs_mesh_t *self)
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
PCSTREAM_RET pcs_mesh_destroy(pcs_mesh_t *self)
{
  _pcs_mesh_free(self);
  *self = (pcs_mesh_t){0};
  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET
pcs_mesh_read_from_buff_serial(pcs_mesh_t *self,
                               const char    *data,
                               PCSTREAM_COUNT size)
{
  const uint8_t *curr     = (const uint8_t *)data;
  const uint8_t *end      = curr + size;
  uint32_t       nv       = 0;
  uint32_t       ni       = 0;
  size_t         pos_size = 0;
  size_t         idx_size = 0;

  if (self == PCSTREAM_NULL || data == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;

  _pcs_mesh_free(self);

  /* ---------- read vertex count ----------------------------- */
  if ((end - curr) < (ptrdiff_t)sizeof(uint32_t))
    return PCSTREAM_RET_FAIL;

  memcpy(&nv, curr, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  if (nv == 0)
    return PCSTREAM_RET_FAIL;

  self->num_verts = nv;

  /* ---------- read vertex positions ------------------------- */
  pos_size        = (size_t)nv * 3u * sizeof(float);

  if ((end - curr) < (ptrdiff_t)pos_size)
    return PCSTREAM_RET_FAIL;

  self->pos = (float *)malloc(pos_size);
  if (self->pos == PCSTREAM_NULL)
    return PCSTREAM_RET_FAIL;

  memcpy(self->pos, curr, pos_size);
  curr += pos_size;

  /* ---------- read index count ------------------------------ */
  if ((end - curr) < (ptrdiff_t)sizeof(uint32_t))
  {
    _pcs_mesh_free(self);
    return PCSTREAM_RET_FAIL;
  }

  memcpy(&ni, curr, sizeof(uint32_t));
  curr += sizeof(uint32_t);

  if (ni == 0)
  {
    _pcs_mesh_free(self);
    return PCSTREAM_RET_FAIL;
  }

  self->num_indices = ni;

  /* ---------- read indices array ---------------------------- */
  idx_size          = (size_t)ni * sizeof(uint32_t);

  if ((end - curr) < (ptrdiff_t)idx_size)
  {
    _pcs_mesh_free(self);
    return PCSTREAM_RET_FAIL;
  }

  self->indices = (uint32_t *)malloc(idx_size);
  if (self->indices == PCSTREAM_NULL)
  {
    _pcs_mesh_free(self);
    return PCSTREAM_RET_FAIL;
  }

  memcpy(self->indices, curr, idx_size);

  return PCSTREAM_RET_SUCCESS;
}

static inline PCSTREAM_COUNT pcs_mesh_serial_size(const pcs_mesh_t *m)
{
  return sizeof(uint32_t) +                  /* num verts  */
         3u * m->num_verts * sizeof(float) + /* positions  */
         sizeof(uint32_t) +                  /* num idx    */
         m->num_indices * sizeof(uint32_t);  /* indices    */
}
PCSTREAM_RET
pcs_mesh_write_to_buff_serial(pcs_mesh_t *self,
                              char          **data_out,
                              PCSTREAM_COUNT *size_out)
{
  PCSTREAM_COUNT need     = 0;
  size_t         pos_size = 0;
  size_t         idx_size = 0;
  uint8_t       *buf      = PCSTREAM_NULL;
  uint8_t       *curr     = PCSTREAM_NULL;

  if (!self || !data_out || !size_out)
    return PCSTREAM_RET_FAIL;

  if (self->num_verts == 0 || self->num_indices == 0 || !self->pos ||
      !self->indices)
    return PCSTREAM_RET_FAIL;

  need     = pcs_mesh_serial_size(self);
  pos_size = (size_t)self->num_verts * 3u * sizeof(float);
  idx_size = (size_t)self->num_indices * sizeof(uint32_t);

  buf      = (uint8_t *)malloc((size_t)need);
  if (!buf)
    return PCSTREAM_RET_FAIL;

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

PCSTREAM_RET pcs_mesh_read_from_file_ply(pcs_mesh_t *self,
                                         const char *filepath)
{
  uint32_t nv  = 0;
  uint32_t nf  = 0;
  int      ret = 0;
  if (self != PCSTREAM_NULL)
    _pcs_mesh_free(self);

  nv = (uint32_t)ply_count_vertex(filepath);
  nf = (uint32_t)ply_count_face(filepath);
  _pcs_mesh_alloc(self, nv, nf * 3);
  ret = ply_mesh_loader(filepath, self->pos, self->indices);
  if (ret == 0)
    return PCSTREAM_RET_FAIL;
  return PCSTREAM_RET_SUCCESS;
}
PCSTREAM_RET pcs_mesh_write_to_file_ply(pcs_mesh_t *self,
                                        const char   *filepath,
                                        PCSTREAM_BOOL binary)
{
  int ret = 0;
  if (!self || !filepath)
    return PCSTREAM_RET_FAIL;
  ret = ply_mesh_writer(filepath,
                        self->pos,
                        self->num_verts,
                        self->indices,
                        self->num_indices,
                        binary == PCSTREAM_TRUE ? 1 : 0);
  if (ret == 0)
    return PCSTREAM_RET_FAIL;
  return PCSTREAM_RET_SUCCESS;
}
static int _is_toward(pcs_vec2f_t a, pcs_vec2f_t b, pcs_vec2f_t c)
{
  return (b.x - a.x) * (c.y - a.y) > (c.x - a.x) * (b.y - a.y);
}

static float _polygon_area(pcs_vec2f_t *points, int n)
{
  float area = 0.0;
  int   i    = 0;
  int   j    = 0;

  if (n < 3)
    return 0.0; // No valid area if less than 3 points

  for (i = 0; i < n; i++)
  {
    j = (i + 1) % n;
    area += points[i].x * points[j].y - points[j].x * points[i].y;
  }
  return 0.5f * fabsf(area);
}
static int _inside(pcs_vec2f_t p, int edge)
{
  switch (edge)
  {
  case 0:
    return p.x >= -1; // Left edge
  case 1:
    return p.x <= 1; // Right edge
  case 2:
    return p.y >= -1; // Bottom edge
  case 3:
    return p.y <= 1; // Top edge
  }
  return 0;
}

static pcs_vec2f_t
_intersection(pcs_vec2f_t p1, pcs_vec2f_t p2, int edge)
{
  pcs_vec2f_t inter = {0};
  float       m     = 0;
  if (float_equal(p1.x, p2.x))
    m = 1e9; // Avoid division by zero for vertical lines
  else
    m = (p2.y - p1.y) / (p2.x - p1.x);
  switch (edge)
  {
  case 0: // Left edge (x = -1)
    inter.x = -1;
    inter.y = p1.y + m * (-1 - p1.x);
    break;
  case 1: // Right edge (x = 1)
    inter.x = 1;
    inter.y = p1.y + m * (1 - p1.x);
    break;
  case 2: // Bottom edge (y = -1)
    inter.y = -1;
    inter.x = p1.x + (inter.y - p1.y) / m;
    break;
  case 3: // Top edge (y = 1)
    inter.y = 1;
    inter.x = p1.x + (inter.y - p1.y) / m;
    break;
  }
  return inter;
}

static int
_clip_polygon(pcs_vec2f_t *in, int in_len, pcs_vec2f_t *out, int edge)
{
  int         out_len     = 0;
  pcs_vec2f_t prev        = {0};
  int         prev_inside = 0;
  int         i           = 0;
  pcs_vec2f_t curr        = {0};
  int         curr_inside = 0;

  if (in_len == 0)
    return 0;

  prev        = in[in_len - 1];
  prev_inside = _inside(prev, edge);

  for (i = 0; i < in_len; i++)
  {
    curr        = in[i];
    curr_inside = _inside(curr, edge);

    if (curr_inside)
    {
      if (!prev_inside)
      {
        out[out_len++] =
            _intersection(prev, curr, edge); // Entering intersection
      }
      out[out_len++] = curr; // Current point is inside
    }
    else if (prev_inside)
    {
      out[out_len++] =
          _intersection(prev, curr, edge); // Exiting intersection
    }
    prev        = curr;
    prev_inside = curr_inside;
  }
  return out_len;
}

static float
_clipped_triangle_area(pcs_vec2f_t p1, pcs_vec2f_t p2, pcs_vec2f_t p3)
{
  pcs_vec2f_t polygon[10]  = {0};
  int         polygon_size = 0;
  pcs_vec2f_t temp[10]     = {0};
  int         edge         = 0;
  int         i            = 0;
  polygon[0]               = p1;
  polygon[1]               = p2;
  polygon[2]               = p3;
  polygon_size             = 3;

  for (edge = 0; edge < 4; edge++)
  {
    polygon_size = _clip_polygon(polygon, polygon_size, temp, edge);
    for (i = 0; i < polygon_size; i++)
      polygon[i] = temp[i]; // Copy back
  }
  return _polygon_area(polygon, polygon_size);
}

PCSTREAM_RET pcs_mesh_screen_ratio(pcs_mesh_t *self,
                                   const float *mvp,
                                   float       *screen_ratio)
{
  pcs_vec3f_t *vertices = PCSTREAM_NULL;
  // need to be freed
  pcs_vec3f_t *ndcs     = PCSTREAM_NULL;

  *screen_ratio         = 0;
  vertices              = (pcs_vec3f_t *)self->pos;
  ndcs = (pcs_vec3f_t *)malloc(sizeof(pcs_vec3f_t) * self->num_verts);
  for (uint32_t i = 0; i < self->num_verts; i++)
  {
    ndcs[i] = pcs_vec3f_mvp_mul(vertices[i], mvp);
  }

  for (uint32_t i = 0; i < self->num_indices / 3; i++)
  {
    uint32_t idx0 = self->indices[i * 3];
    uint32_t idx1 = self->indices[i * 3 + 1];
    uint32_t idx2 = self->indices[i * 3 + 2];
    if (ndcs[idx0].z >= 0 && ndcs[idx0].z <= 1 && ndcs[idx1].z >= 0 &&
        ndcs[idx1].z <= 1 && ndcs[idx2].z >= 0 && ndcs[idx2].z <= 1 &&
        _is_toward((pcs_vec2f_t){ndcs[idx0].x, ndcs[idx0].y},
                   (pcs_vec2f_t){ndcs[idx1].x, ndcs[idx1].y},
                   (pcs_vec2f_t){ndcs[idx2].x, ndcs[idx2].y}))
    {
      *screen_ratio += _clipped_triangle_area(
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
