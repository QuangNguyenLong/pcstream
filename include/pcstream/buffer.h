#ifndef PCSTREAM_BUFFER_H
#define PCSTREAM_BUFFER_H

#include <pcstream/def.h>
#include <stdlib.h>

typedef struct pcs_buffer_t pcs_buffer_t;

/**
 * @struct pcs_buffer_t
 * @brief Structure representing a memory buffer.
 * @details Contains a pointer to the allocated memory and the size
 * of the buffer.
 */
struct pcs_buffer_t
{
  /**
   * @brief Pointer to the buffer data in memory.
   */
  char  *data;

  /**
   * @brief Size of the buffer in bytes.
   */
  size_t size;
};

/**
 * @brief Initializes a pcs_buffer_t instance.
 * @details Sets the internal fields of the buffer to default values
 * (NULL and 0).
 * @param self Pointer to the pcs_buffer_t instance to be
 * initialized.
 * @return PCSTREAM_RET_SUCCESS on successful initialization.
 */
PCSTREAM_EXPORT pcs_ret_t pcs_buffer_init(pcs_buffer_t *self);

/**
 * @brief Destroys a pcs_buffer_t instance.
 * @details Frees allocated memory (if any) and resets the structure
 * to default values.
 * @param self Pointer to the pcs_buffer_t instance to be destroyed.
 * @return PCSTREAM_RET_SUCCESS on successful destruction.
 */
PCSTREAM_EXPORT pcs_ret_t pcs_buffer_destroy(pcs_buffer_t *self);

#endif // PCSTREAM_BUFFER_H
