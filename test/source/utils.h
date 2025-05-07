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
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *filepath, size_t *out_size)
{
  FILE *file = fopen(filepath, "rb");
  if (!file)
  {
    perror("Error opening file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = (size_t)ftell(file);
  rewind(file);

  char *buffer = malloc(file_size);
  if (!buffer)
  {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  fclose(file);

  if (out_size)
    *out_size = bytes_read;

  return buffer;
}
