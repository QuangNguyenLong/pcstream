#ifndef PCSTREAM_VEC3F_H
#define PCSTREAM_VEC3F_H
#include <math.h>
#include <pcstream/def.h>

#ifdef __cplusplus
extern "C"
{
#endif
  static inline int float_error(float left, float right, float error)
  {
    return left - right < error && left - right > -error;
  }
  static inline int float_equal(float left, float right)
  {
    return float_error(left, right, PCSTREAM_FLOAT_ERROR);
  }
  static float quantize(float var, float qsize)
  {
    return qsize * (float)floor(var / qsize + PCSTREAM_FLOAT_HALF);
  }

  typedef struct pcs_vec3f_t
  {
    float x;
    float y;
    float z;
  } pcs_vec3f_t;
  static inline pcs_vec3f_t
  pcs_vec3f_set(float vecx, float vecy, float vecz)
  {
    pcs_vec3f_t vec = {vecx, vecy, vecz};
    return vec;
  }
  static inline pcs_vec3f_t pcs_vec3f_int(pcs_vec3f_t vec)
  {
    return (pcs_vec3f_t){
        (float)(int)vec.x, (float)(int)vec.y, (float)(int)vec.z};
  }
  static inline pcs_vec3f_t pcs_vec3f_mul_scalar(pcs_vec3f_t vec,
                                                 float       scalar)
  {
    return (pcs_vec3f_t){
        vec.x * scalar, vec.y * scalar, vec.z * scalar};
  }
  static inline pcs_vec3f_t pcs_vec3f_inverse(pcs_vec3f_t vec)
  {
    return (pcs_vec3f_t){1.0F / vec.x, 1.0F / vec.y, 1.0F / vec.z};
  }
  static inline float pcs_vec3f_magnitude(pcs_vec3f_t vec)
  {
    return (float)sqrt(vec.x * vec.x + vec.y * vec.y +
                       vec.z * vec.z);
  }
  static inline pcs_vec3f_t pcs_vec3f_normalize(pcs_vec3f_t vec)
  {
    float mag = pcs_vec3f_magnitude(vec);
    if (mag > 0.0F)
    {
      return pcs_vec3f_mul_scalar(vec, 1.0F / mag);
    }
    return (pcs_vec3f_t){0};
  }
  static inline pcs_vec3f_t pcs_vec3f_add(pcs_vec3f_t left,
                                          pcs_vec3f_t right)
  {
    return (pcs_vec3f_t){
        left.x + right.x, left.y + right.y, left.z + right.z};
  }
  static inline pcs_vec3f_t pcs_vec3f_sub(pcs_vec3f_t left,
                                          pcs_vec3f_t right)
  {
    return (pcs_vec3f_t){
        left.x - right.x, left.y - right.y, left.z - right.z};
  }
  static inline pcs_vec3f_t pcs_vec3f_mul(pcs_vec3f_t left,
                                          pcs_vec3f_t right)
  {
    return (pcs_vec3f_t){
        left.x * right.x, left.y * right.y, left.z * right.z};
  }
  static inline float pcs_vec3f_dot(pcs_vec3f_t left,
                                    pcs_vec3f_t right)
  {
    return left.x * right.x + left.y * right.y + left.z * right.z;
  }
  static inline pcs_vec3f_t pcs_vec3f_cross(pcs_vec3f_t left,
                                            pcs_vec3f_t right)
  {
    return (pcs_vec3f_t){left.y * right.z - left.z * right.y,
                         left.z * right.x - left.x * right.z,
                         left.x * right.y - left.y * right.x};
  }
  static inline float pcs_vec3f_angle_between(pcs_vec3f_t left,
                                              pcs_vec3f_t right)
  {
    float dot  = pcs_vec3f_dot(left, right);
    float magA = pcs_vec3f_magnitude(left);
    float magB = pcs_vec3f_magnitude(right);
    return (float)acos(dot / (magA * magB));
  }
  static inline int pcs_vec3f_eq(pcs_vec3f_t left, pcs_vec3f_t right)
  {
    return float_equal(left.x, right.x) &&
           float_equal(left.y, right.y) &&
           float_equal(left.z, right.z);
  }

  static inline int pcs_vec3f_g(pcs_vec3f_t left, pcs_vec3f_t right)
  {
    if (left.x < right.x)
    {
      return 0;
    }
    if (left.x > right.x)
    {
      return 1;
    }
    if (left.y < right.y)
    {
      return 0;
    }
    if (left.y > right.y)
    {
      return 1;
    }
    return left.z > right.z;
  }
  static inline int pcs_vec3f_l(pcs_vec3f_t left, pcs_vec3f_t right)
  {
    if (left.x < right.x)
    {
      return 1;
    }
    if (left.x > right.x)
    {
      return 0;
    }
    if (left.y < right.y)
    {
      return 1;
    }
    if (left.y > right.y)
    {
      return 0;
    }
    return left.z < right.z;
  }
  static inline int pcs_vec3f_geq(pcs_vec3f_t left,
                                  pcs_vec3f_t right)
  {
    return pcs_vec3f_g(left, right) || pcs_vec3f_eq(left, right);
  }
  static inline int pcs_vec3f_leq(pcs_vec3f_t left,
                                  pcs_vec3f_t right)
  {
    return pcs_vec3f_l(left, right) || pcs_vec3f_eq(left, right);
  }
  static inline pcs_vec3f_t pcs_vec3f_reflect(pcs_vec3f_t vec,
                                              pcs_vec3f_t other)
  {
    float dot = pcs_vec3f_dot(vec, other);
    return pcs_vec3f_sub(vec, pcs_vec3f_mul_scalar(other, 2 * dot));
  }

  static inline pcs_vec3f_t pcs_vec3f_quantize(pcs_vec3f_t vec,
                                               float       qsize)
  {
    return (pcs_vec3f_t){quantize(vec.x, qsize),
                         quantize(vec.y, qsize),
                         quantize(vec.z, qsize)};
  }
  static inline pcs_vec3f_t pcs_vec3f_mvp_mul(pcs_vec3f_t  vec,
                                              const float *mvp)
  {
    float temp_x =
        mvp[0] * vec.x + mvp[4] * vec.y + mvp[8] * vec.z + mvp[12];
    float temp_y =
        mvp[1] * vec.x + mvp[5] * vec.y + mvp[9] * vec.z + mvp[13];
    float temp_z =
        mvp[2] * vec.x + mvp[6] * vec.y + mvp[10] * vec.z + mvp[14];
    float temp_w =
        mvp[3] * vec.x + mvp[7] * vec.y + mvp[11] * vec.z + mvp[15];
    temp_x /= temp_w;
    temp_y /= temp_w;
    temp_z /= temp_w;
    return (pcs_vec3f_t){temp_x, temp_y, temp_z};
  }
  static inline pcs_vec3f_t
  pcs_vec3f_rotate(pcs_vec3f_t vec, float angle, pcs_vec3f_t axis)
  {
    float cosa        = cosf(angle);
    float sina        = sinf(angle);
    float one_minus_c = 1.0F - cosa;

    float len =
        sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    axis.x /= len;
    axis.y /= len;
    axis.z /= len;

    float tmp[3][3] = {
        {         axis.x * axis.x * one_minus_c + cosa,
         axis.x * axis.y * one_minus_c - axis.z * sina,
         axis.x * axis.z * one_minus_c + axis.y * sina},
        {axis.y * axis.x * one_minus_c + axis.z * sina,
         axis.y * axis.y * one_minus_c + cosa,
         axis.y * axis.z * one_minus_c - axis.x * sina},
        {axis.z * axis.x * one_minus_c - axis.y * sina,
         axis.z * axis.y * one_minus_c + axis.x * sina,
         axis.z * axis.z * one_minus_c + cosa         }
    };

    pcs_vec3f_t result;
    result.x =
        tmp[0][0] * vec.x + tmp[0][1] * vec.y + tmp[0][2] * vec.z;
    result.y =
        tmp[1][0] * vec.x + tmp[1][1] * vec.y + tmp[1][2] * vec.z;
    result.z =
        tmp[2][0] * vec.x + tmp[2][1] * vec.y + tmp[2][2] * vec.z;

    return result;
  }

#ifdef __cplusplus
}
#endif

#endif
