#ifndef _GLU_SOURCE
#define _GNU_SOURCE
#endif
#include <pcstream/algorithm.h>
#include <stdio.h>
#include <stdlib.h>

#define PCSTREAM_DP_BASED_NUMBER_LABEL 20

#define DEFINE_PAIR(TYPE) \
  typedef struct          \
  {                       \
    TYPE cost;            \
    TYPE value;           \
  } _pair_##TYPE##_t;

DEFINE_PAIR(float)
typedef struct _pcs_node_t  _pcs_node_t;
typedef struct _pcs_path_t  _pcs_path_t;
typedef struct _pcs_graph_t _pcs_graph_t;

static PCSTREAM_BOOL        check_dominance(_pcs_node_t *node_v,
                                            _pcs_path_t *path_P);
static PCSTREAM_BOOL        check_feasibility(_pcs_node_t *node_v,
                                              _pcs_path_t *path_P,
                                              float       *Tval);
static PCSTREAM_BOOL
check_bounds(_pcs_node_t *node_v, _pcs_path_t *path_P, float *Cval);

struct _pcs_node_t
{
  int            version;
  float          cost;
  float          value;
  float          minimum_cost;
  float          maximum_value;

  _pair_float_t *dominance_label;
  int            index_dl;
  int            num_dl;

  void (*set)(_pcs_node_t *, _pair_float_t, int, int);
  void (*update_dominance)(_pcs_node_t *, float, float);
};
static void _pcs_node_set(_pcs_node_t  *self,
                          _pair_float_t cost_val,
                          int           version,
                          int           num_dl)
{
  self->cost            = cost_val.cost;
  self->value           = cost_val.value;
  self->version         = version;
  self->num_dl          = num_dl;

  self->dominance_label = (_pair_float_t *)malloc(
      sizeof(_pair_float_t) * (unsigned int)self->num_dl);

  for (int i = 0; i < self->num_dl; i++)
    self->dominance_label[i] = (_pair_float_t){0, 0};

  self->index_dl = 0;
}
static void _pcs_node_update_dominance(_pcs_node_t *self,
                                       float        cost,
                                       float        value)
{
  int   lowc      = 0;
  int   highv     = 0;
  int   idl       = 0;
  float max_value = 0;
  float min_cost  = 0;
  int   random    = 0;

  if (self->index_dl < self->num_dl - 1)
  {
    idl                        = self->index_dl++;
    self->dominance_label[idl] = (_pair_float_t){cost, value};
    return;
  }
  max_value = self->dominance_label[0].value;
  min_cost  = self->dominance_label[0].cost;

  for (int i = 1; i < self->index_dl; i++)
  {
    if (self->dominance_label[i].cost < min_cost)
    {
      min_cost = self->dominance_label[i].cost;
      lowc     = i;
    }
    if (self->dominance_label[i].value > max_value)
    {
      max_value = self->dominance_label[i].value;
      highv     = i;
    }
  }
  random = rand() % self->index_dl;
  while (random == lowc || random == highv)
    random = rand() % self->index_dl;
  self->dominance_label[random] = (_pair_float_t){cost, value};
}
static PCSTREAM_RET _pcs_node_init(_pcs_node_t *self)
{
  self->version          = 0;
  self->cost             = 0;
  self->minimum_cost     = 0;
  self->maximum_value    = 0;
  self->dominance_label  = PCSTREAM_NULL;
  self->index_dl         = 0;
  self->num_dl           = 0;

  self->set              = _pcs_node_set;
  self->update_dominance = _pcs_node_update_dominance;

  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _pcs_node_destroy(_pcs_node_t *self)
{
  if (self->dominance_label != PCSTREAM_NULL)
    free(self->dominance_label);

  self->version          = 0;
  self->cost             = 0;
  self->minimum_cost     = 0;
  self->maximum_value    = 0;
  self->dominance_label  = PCSTREAM_NULL;
  self->index_dl         = 0;
  self->num_dl           = 0;

  self->set              = PCSTREAM_NULL;
  self->update_dominance = PCSTREAM_NULL;

  return PCSTREAM_RET_SUCCESS;
}

struct _pcs_path_t
{
  float cost;
  float value;

  int  *version_choose;
  int   index;
  int   n_mod;

  _pcs_path_t (*addition_assign)(_pcs_path_t *, _pcs_node_t);
  _pcs_path_t (*assign)(_pcs_path_t *, _pcs_path_t);
};
static _pcs_path_t _pcs_path_addition_assign(_pcs_path_t *self,
                                             _pcs_node_t  n)
{
  self->cost += n.cost;
  self->value += n.value;
  self->version_choose[self->index++] = n.version;
  return *self;
}
static _pcs_path_t _pcs_path_assign(_pcs_path_t *self,
                                    _pcs_path_t  other)
{
  self->value = other.value;
  self->cost  = other.cost;
  self->index = other.index;
  self->n_mod = other.n_mod;

  if (self->version_choose != PCSTREAM_NULL)
  {
    free(self->version_choose);
    self->version_choose = PCSTREAM_NULL;
  }
  self->version_choose =
      (int *)malloc(sizeof(int) * (unsigned int)self->n_mod);
  for (int i = 0; i < self->n_mod; i++)
    self->version_choose[i] = other.version_choose[i];
  return *self;
}

static PCSTREAM_RET _pcs_path_init(_pcs_path_t   *self,
                                   float          cost,
                                   float          value,
                                   PCSTREAM_COUNT n_mod)
{
  self->cost  = cost;
  self->value = value;
  self->n_mod = (int)n_mod;
  self->index = 0;

  self->version_choose =
      (int *)malloc(sizeof(int) * (unsigned int)self->n_mod);

  for (int i = 0; i < self->n_mod; i++)
    self->version_choose[i] = -1;

  self->addition_assign = _pcs_path_addition_assign;
  self->assign          = _pcs_path_assign;

  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _pcs_path_destroy(_pcs_path_t *self)
{
  if (self->version_choose != PCSTREAM_NULL)
    free(self->version_choose);
  self->cost            = 0;
  self->value           = 0;
  self->n_mod           = 0;
  self->index           = 0;

  self->addition_assign = PCSTREAM_NULL;
  self->assign          = PCSTREAM_NULL;

  return PCSTREAM_RET_SUCCESS;
}

struct _pcs_graph_t
{
  _pcs_node_t **matrix;
  int           n_ver;
  int           n_mod;
  float         Rcons;
  float         Q_val;
  void (*pulse)(
      _pcs_graph_t *, int, int, _pcs_path_t, _pcs_path_t *);
};
static void _pcs_graph_pulse(_pcs_graph_t *self,
                             int           i,
                             int           j,
                             _pcs_path_t   node_v,
                             _pcs_path_t  *v_s)
{
  int check = 0;
  if (i == -1)
  {
    for (int k = 0; k < self->n_ver; k++)
      self->pulse(self, i + 1, k, node_v, v_s);
    return;
  }
  check = PCSTREAM_FALSE;
  if (check_dominance(&self->matrix[i][j], &node_v) == PCSTREAM_TRUE)
    check = PCSTREAM_TRUE;

  if (check_feasibility(&self->matrix[i][j],
                        &node_v,
                        &self->Rcons) == PCSTREAM_TRUE)
    check = PCSTREAM_TRUE;

  if (check_bounds(&self->matrix[i][j], &node_v, &self->Q_val) ==
      PCSTREAM_FALSE)
    check = PCSTREAM_TRUE;

  if (check == PCSTREAM_TRUE)
    return;

  node_v.addition_assign(&node_v, self->matrix[i][j]);
  if (i == self->n_mod - 1)
  {
    *v_s        = node_v;
    self->Q_val = v_s->value;
    return;
  }
  for (int k = 0; k < self->n_ver; k++)
    self->pulse(self, i + 1, k, node_v, v_s);
}

static PCSTREAM_RET _pcs_graph_init(_pcs_graph_t *self,
                                    void         *cv_buff,
                                    size_t        cv_size,
                                    int           n_ver,
                                    int           n_mod,
                                    float         Rcons,
                                    float         Q_val,
                                    int           number_labels,
                                    float        *weights)
{
  float          node_c                  = 0;
  float          node_v                  = 0;
  char           line[PCSTREAM_MAX_BUFF] = {0};
  FILE          *data                    = PCSTREAM_NULL;
  _pair_float_t *cost_val                = PCSTREAM_NULL;
  float          sum_min                 = 0;
  float          sum_max                 = 0;

  self->matrix                           = PCSTREAM_NULL;
  self->n_ver                            = n_ver;
  self->n_mod                            = n_mod;
  self->Rcons                            = Rcons;
  self->Q_val                            = Q_val;
  self->pulse                            = _pcs_graph_pulse;

  cost_val                               = (_pair_float_t *)malloc(
      sizeof(_pair_float_t) *
      (unsigned int)(self->n_ver * self->n_mod));
  for (int i = 0; i < self->n_ver * self->n_mod; i++)
    cost_val[i] = (_pair_float_t){0, 0};

  data = fmemopen((void *)cv_buff, cv_size, "r");
  fgets(line, sizeof(line), data);

  for (int t = 0; fgets(line, sizeof(line), data); t++)
  {
    if (sscanf(line, "%f %f", &node_c, &node_v) != 2)
      continue;
    node_v *= weights[t / n_ver];

    cost_val[t].cost  = node_c;
    cost_val[t].value = node_v;
  }

  self->matrix = (_pcs_node_t **)malloc(sizeof(_pcs_node_t *) *
                                        (unsigned int)self->n_mod);
  for (int i = 0; i < self->n_mod; i++)
  {
    self->matrix[i] = (_pcs_node_t *)malloc(
        sizeof(_pcs_node_t) * (unsigned int)self->n_ver);
    for (int j = 0; j < self->n_ver; j++)
      _pcs_node_init(&(self->matrix[i][j]));
  }

  for (int i = 0; i < self->n_mod; i++)
    for (int j = 0; j < self->n_ver; j++)
      self->matrix[i][j].set(&self->matrix[i][j],
                             cost_val[i * self->n_ver + j],
                             j,
                             number_labels);
  sum_min = 0;
  for (int i = 0; i < self->n_mod; i++)
    sum_min += self->matrix[i][0].cost;

  for (int i = 0; i < self->n_mod; i++)
  {
    sum_min -= self->matrix[i][0].cost;
    for (int j = 0; j < self->n_ver; j++)
    {
      float tmp = self->matrix[i][j].cost + sum_min;
      self->matrix[i][j].minimum_cost = tmp;
    }
  }

  sum_max = 0;
  for (int i = 0; i < self->n_mod; i++)
    sum_max += self->matrix[i][self->n_ver - 1].value;

  for (int i = 0; i < self->n_mod; i++)
  {
    sum_max -= self->matrix[i][self->n_ver - 1].value;
    for (int j = 0; j < self->n_ver; j++)
    {
      float tmp = self->matrix[i][j].value + sum_max;
      self->matrix[i][j].maximum_value = tmp;
    }
  }

  free(cost_val);
  fclose(data);
  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _graph_destroy(_pcs_graph_t *self)
{
  if (self->matrix != PCSTREAM_NULL)
  {
    for (int i = 0; i < self->n_mod; i++)
    {
      for (int j = 0; j < self->n_ver; j++)
        _pcs_node_destroy(&(self->matrix[i][j]));
      free(self->matrix[i]);
    }
    free(self->matrix);
  }
  self->matrix = PCSTREAM_NULL;
  self->n_ver  = 0;
  self->n_mod  = 0;
  self->Rcons  = 0;
  self->Q_val  = 0;
  self->pulse  = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

static PCSTREAM_BOOL check_dominance(_pcs_node_t *node_v,
                                     _pcs_path_t *path_P)
{
  for (int i = 0; i < node_v->index_dl; i++)
  {
    if (node_v->dominance_label[i].cost <=
            path_P->cost + node_v->cost &&
        node_v->dominance_label[i].value >
            path_P->value + node_v->value)
      return PCSTREAM_TRUE;
    if (node_v->dominance_label[i].cost <
            path_P->cost + node_v->cost &&
        node_v->dominance_label[i].value >=
            path_P->value + node_v->value)
      return PCSTREAM_TRUE;
  }
  node_v->update_dominance(node_v,
                           path_P->cost + node_v->cost,
                           path_P->value + node_v->value);
  return PCSTREAM_FALSE;
}
static PCSTREAM_BOOL check_feasibility(_pcs_node_t *node_v,
                                       _pcs_path_t *path_P,
                                       float       *Tval)
{
  if (path_P->cost + node_v->minimum_cost > *Tval)
    return PCSTREAM_FALSE;
  return PCSTREAM_TRUE;
}
static PCSTREAM_BOOL
check_bounds(_pcs_node_t *node_v, _pcs_path_t *path_P, float *Cval)
{
  if (path_P->value + node_v->maximum_value <= *Cval)
    return PCSTREAM_FALSE;
  return PCSTREAM_TRUE;
}

PCSTREAM_RET pcs_dp_based_solution(PCSTREAM_COUNT  n_mod,
                                   PCSTREAM_COUNT  n_ver,
                                   void           *metadata,
                                   size_t          metadata_size,
                                   PCSTREAM_RATIO *screen_ratio,
                                   PCSTREAM_BW     bandwidth,
                                   PCSTREAM_LOD_VERSION *selection)
{
  float        Q_val         = 0;
  _pcs_path_t  node_v        = {0};
  _pcs_path_t  v_s           = {0};
  _pcs_graph_t G             = {0};
  int          number_labels = 0;

  number_labels              = PCSTREAM_DP_BASED_NUMBER_LABEL;

  _pcs_path_init(&node_v, 0, 0, n_mod);
  _pcs_path_init(&v_s, 0, 0, n_mod);

  _pcs_graph_init(&G,
                  metadata,
                  metadata_size,
                  (int)n_ver,
                  (int)n_mod,
                  (float)bandwidth,
                  Q_val,
                  number_labels,
                  screen_ratio);
  G.pulse(&G, -1, 0, node_v, &v_s);

  for (PCSTREAM_COUNT i = 0; i < n_mod; i++)
  {
    int ver = v_s.version_choose[i];
    if (ver == -1)
      selection[i] = 0;
    else
      selection[i] = (PCSTREAM_LOD_VERSION)ver;
  }

  _graph_destroy(&G);
  _pcs_path_destroy(&node_v);
  _pcs_path_destroy(&v_s);

  return PCSTREAM_RET_SUCCESS;
}

PCSTREAM_RET pcs_lm_based_solution(void)
{
  return PCSTREAM_RET_FAIL;
}

PCSTREAM_RET pcs_hybrid_solution(void)
{
  return PCSTREAM_RET_FAIL;
}

PCSTREAM_RET pcs_equal_solution(void)
{
  return PCSTREAM_RET_FAIL;
}
