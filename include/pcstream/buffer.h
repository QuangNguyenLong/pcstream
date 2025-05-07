/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
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
