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
typedef struct _node_t  _node_t;
typedef struct _path_t  _path_t;
typedef struct _graph_t _graph_t;

static PCSTREAM_BOOL    check_dominance(_node_t *v, _path_t *P);
static PCSTREAM_BOOL
check_feasibility(_node_t *v, _path_t *P, float *T);
static PCSTREAM_BOOL check_bounds(_node_t *v, _path_t *P, float *C);

struct _node_t
{
  int            version;
  float          cost;
  float          value;
  float          minimum_cost;
  float          maximum_value;

  _pair_float_t *dominance_label;
  int            index_dl;
  int            num_dl;

  void (*set)(_node_t *, _pair_float_t, int, int);
  void (*update_dominance)(_node_t *, float, float);
};
static void _node_set(_node_t *this,
                      _pair_float_t cost_val,
                      int           version,
                      int           num_dl)
{
  this->cost            = cost_val.cost;
  this->value           = cost_val.value;
  this->version         = version;
  this->num_dl          = num_dl;

  this->dominance_label = (_pair_float_t *)malloc(
      sizeof(_pair_float_t) * (unsigned int)this->num_dl);

  for (int i = 0; i < this->num_dl; i++)
    this->dominance_label[i] = (_pair_float_t){0, 0};

  this->index_dl = 0;
}
static void
_node_update_dominance(_node_t *this, float cost, float value)
{
  int   lowc      = 0;
  int   highv     = 0;
  int   idl       = 0;
  float max_value = 0;
  float min_cost  = 0;
  int   random    = 0;

  if (this->index_dl < this->num_dl - 1)
  {
    idl                        = this->index_dl++;
    this->dominance_label[idl] = (_pair_float_t){cost, value};
    return;
  }
  max_value = this->dominance_label[0].value;
  min_cost  = this->dominance_label[0].cost;

  for (int i = 1; i < this->index_dl; i++)
  {
    if (this->dominance_label[i].cost < min_cost)
    {
      min_cost = this->dominance_label[i].cost;
      lowc     = i;
    }
    if (this->dominance_label[i].value > max_value)
    {
      max_value = this->dominance_label[i].value;
      highv     = i;
    }
  }
  random = rand() % this->index_dl;
  while (random == lowc || random == highv)
    random = rand() % this->index_dl;
  this->dominance_label[random] = (_pair_float_t){cost, value};
}
static PCSTREAM_RET _node_init(_node_t *this)
{
  this->version          = 0;
  this->cost             = 0;
  this->minimum_cost     = 0;
  this->maximum_value    = 0;
  this->dominance_label  = PCSTREAM_NULL;
  this->index_dl         = 0;
  this->num_dl           = 0;

  this->set              = _node_set;
  this->update_dominance = _node_update_dominance;

  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _node_destroy(_node_t *this)
{
  if (this->dominance_label != PCSTREAM_NULL)
    free(this->dominance_label);

  this->version          = 0;
  this->cost             = 0;
  this->minimum_cost     = 0;
  this->maximum_value    = 0;
  this->dominance_label  = PCSTREAM_NULL;
  this->index_dl         = 0;
  this->num_dl           = 0;

  this->set              = PCSTREAM_NULL;
  this->update_dominance = PCSTREAM_NULL;

  return PCSTREAM_RET_SUCCESS;
}

struct _path_t
{
  float cost;
  float value;

  int  *version_choose;
  int   index;
  int   n_mod;

  _path_t (*addition_assign)(_path_t *, _node_t);
  _path_t (*assign)(_path_t *, _path_t);
};
static _path_t _path_addition_assign(_path_t *this, _node_t n)
{
  this->cost += n.cost;
  this->value += n.value;
  this->version_choose[this->index++] = n.version;
  return *this;
}
static _path_t _path_assign(_path_t *this, _path_t other)
{
  this->value = other.value;
  this->cost  = other.cost;
  this->index = other.index;
  this->n_mod = other.n_mod;

  if (this->version_choose != PCSTREAM_NULL)
  {
    free(this->version_choose);
    this->version_choose = PCSTREAM_NULL;
  }
  this->version_choose =
      (int *)malloc(sizeof(int) * (unsigned int)this->n_mod);
  for (int i = 0; i < this->n_mod; i++)
    this->version_choose[i] = other.version_choose[i];
  return *this;
}

static PCSTREAM_RET _path_init(_path_t *this,
                               float          cost,
                               float          value,
                               PCSTREAM_COUNT n_mod)
{
  this->cost  = cost;
  this->value = value;
  this->n_mod = (int)n_mod;
  this->index = 0;

  this->version_choose =
      (int *)malloc(sizeof(int) * (unsigned int)this->n_mod);

  for (int i = 0; i < this->n_mod; i++)
    this->version_choose[i] = -1;

  this->addition_assign = _path_addition_assign;
  this->assign          = _path_assign;

  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _path_destroy(_path_t *this)
{
  if (this->version_choose != PCSTREAM_NULL)
    free(this->version_choose);
  this->cost            = 0;
  this->value           = 0;
  this->n_mod           = 0;
  this->index           = 0;

  this->addition_assign = PCSTREAM_NULL;
  this->assign          = PCSTREAM_NULL;

  return PCSTREAM_RET_SUCCESS;
}

struct _graph_t
{
  _node_t **matrix;
  int       n_ver;
  int       n_mod;
  float     Rc;
  float     Q_;
  void (*pulse)(_graph_t *, int, int, _path_t, _path_t *);
};
static void
_graph_pulse(_graph_t *this, int i, int j, _path_t v, _path_t *v_s)
{
  int check = 0;
  if (i == -1)
  {
    for (int k = 0; k < this->n_ver; k++)
      this->pulse(this, i + 1, k, v, v_s);
    return;
  }
  check = PCSTREAM_FALSE;
  if (check_dominance(&this->matrix[i][j], &v) == PCSTREAM_TRUE)
    check = PCSTREAM_TRUE;

  if (check_feasibility(&this->matrix[i][j], &v, &this->Rc) ==
      PCSTREAM_TRUE)
    check = PCSTREAM_TRUE;

  if (check_bounds(&this->matrix[i][j], &v, &this->Q_) ==
      PCSTREAM_FALSE)
    check = PCSTREAM_TRUE;

  if (check == PCSTREAM_TRUE)
    return;

  v.addition_assign(&v, this->matrix[i][j]);
  if (i == this->n_mod - 1)
  {
    *v_s     = v;
    this->Q_ = v_s->value;
    return;
  }
  for (int k = 0; k < this->n_ver; k++)
    this->pulse(this, i + 1, k, v, v_s);
}

static PCSTREAM_RET _graph_init(_graph_t *this,
                                void  *cv_buff,
                                size_t cv_size,
                                int    n_ver,
                                int    n_mod,
                                float  Rc,
                                float  Q_,
                                int    number_labels,
                                float *weights)
{
  float          c                       = 0;
  float          v                       = 0;
  char           line[PCSTREAM_MAX_BUFF] = {0};
  FILE          *data                    = PCSTREAM_NULL;
  _pair_float_t *cost_val                = PCSTREAM_NULL;
  float          sum_min                 = 0;
  float          sum_max                 = 0;

  this->matrix                           = PCSTREAM_NULL;
  this->n_ver                            = n_ver;
  this->n_mod                            = n_mod;
  this->Rc                               = Rc;
  this->Q_                               = Q_;
  this->pulse                            = _graph_pulse;

  cost_val                               = (_pair_float_t *)malloc(
      sizeof(_pair_float_t) *
      (unsigned int)(this->n_ver * this->n_mod));
  for (int i = 0; i < this->n_ver * this->n_mod; i++)
    cost_val[i] = (_pair_float_t){0, 0};

  data = fmemopen((void *)cv_buff, cv_size, "r");
  fgets(line, sizeof(line), data);

  for (int t = 0; fgets(line, sizeof(line), data); t++)
  {
    if (sscanf(line, "%f %f", &c, &v) != 2)
      continue;
    v *= weights[t / n_ver];

    cost_val[t].cost  = c;
    cost_val[t].value = v;
  }

  this->matrix = (_node_t **)malloc(sizeof(_node_t *) *
                                    (unsigned int)this->n_mod);
  for (int i = 0; i < this->n_mod; i++)
  {
    this->matrix[i] = (_node_t *)malloc(sizeof(_node_t) *
                                        (unsigned int)this->n_ver);
    for (int j = 0; j < this->n_ver; j++)
      _node_init(&(this->matrix[i][j]));
  }

  for (int i = 0; i < this->n_mod; i++)
    for (int j = 0; j < this->n_ver; j++)
      this->matrix[i][j].set(&this->matrix[i][j],
                             cost_val[i * this->n_ver + j],
                             j,
                             number_labels);
  sum_min = 0;
  for (int i = 0; i < this->n_mod; i++)
    sum_min += this->matrix[i][0].cost;

  for (int i = 0; i < this->n_mod; i++)
  {
    sum_min -= this->matrix[i][0].cost;
    for (int j = 0; j < this->n_ver; j++)
    {
      float tmp = this->matrix[i][j].cost + sum_min;
      this->matrix[i][j].minimum_cost = tmp;
    }
  }

  sum_max = 0;
  for (int i = 0; i < this->n_mod; i++)
    sum_max += this->matrix[i][this->n_ver - 1].value;

  for (int i = 0; i < this->n_mod; i++)
  {
    sum_max -= this->matrix[i][this->n_ver - 1].value;
    for (int j = 0; j < this->n_ver; j++)
    {
      float tmp = this->matrix[i][j].value + sum_max;
      this->matrix[i][j].maximum_value = tmp;
    }
  }

  free(cost_val);
  fclose(data);
  return PCSTREAM_RET_SUCCESS;
}
static PCSTREAM_RET _graph_destroy(_graph_t *this)
{
  if (this->matrix != PCSTREAM_NULL)
  {
    for (int i = 0; i < this->n_mod; i++)
    {
      for (int j = 0; j < this->n_ver; j++)
        _node_destroy(&(this->matrix[i][j]));
      free(this->matrix[i]);
    }
    free(this->matrix);
  }
  this->matrix = PCSTREAM_NULL;
  this->n_ver  = 0;
  this->n_mod  = 0;
  this->Rc     = 0;
  this->Q_     = 0;
  this->pulse  = PCSTREAM_NULL;
  return PCSTREAM_RET_SUCCESS;
}

static PCSTREAM_BOOL check_dominance(_node_t *v, _path_t *P)
{
  for (int i = 0; i < v->index_dl; i++)
  {
    if (v->dominance_label[i].cost <= P->cost + v->cost &&
        v->dominance_label[i].value > P->value + v->value)
      return PCSTREAM_TRUE;
    if (v->dominance_label[i].cost < P->cost + v->cost &&
        v->dominance_label[i].value >= P->value + v->value)
      return PCSTREAM_TRUE;
  }
  v->update_dominance(v, P->cost + v->cost, P->value + v->value);
  return PCSTREAM_FALSE;
}
static PCSTREAM_BOOL
check_feasibility(_node_t *v, _path_t *P, float *T)
{
  if (P->cost + v->minimum_cost > *T)
    return PCSTREAM_FALSE;
  return PCSTREAM_TRUE;
}
static PCSTREAM_BOOL check_bounds(_node_t *v, _path_t *P, float *C)
{
  if (P->value + v->maximum_value <= *C)
    return PCSTREAM_FALSE;
  return PCSTREAM_TRUE;
}

PCSTREAM_RET pcs_dp_based_solution(PCSTREAM_COUNT  n_mod,
                                   PCSTREAM_COUNT  n_ver,
                                   void           *metadata,
                                   size_t          metadata_size,
                                   PCSTREAM_RATIO *screen_ratio,
                                   PCSTREAM_BW     bw,
                                   PCSTREAM_LOD_VERSION *selection)
{
  float    Q_            = 0;
  _path_t  v             = {0};
  _path_t  v_s           = {0};
  _graph_t G             = {0};
  int      number_labels = 0;

  number_labels          = PCSTREAM_DP_BASED_NUMBER_LABEL;

  _path_init(&v, 0, 0, n_mod);
  _path_init(&v_s, 0, 0, n_mod);

  _graph_init(&G,
              metadata,
              metadata_size,
              (int)n_ver,
              (int)n_mod,
              (float)bw,
              Q_,
              number_labels,
              screen_ratio);
  G.pulse(&G, -1, 0, v, &v_s);

  for (PCSTREAM_COUNT i = 0; i < n_mod; i++)
  {
    int ver = v_s.version_choose[i];
    if (ver == -1)
      selection[i] = 0;
    else
      selection[i] = (PCSTREAM_LOD_VERSION)ver;
  }

  _graph_destroy(&G);
  _path_destroy(&v);
  _path_destroy(&v_s);

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
