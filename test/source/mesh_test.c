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
#include <pcstream/mesh.h>

int main(int argc, char **argv)
{
  pcs_mesh_t  mesh = {0};
  char       *buff = PCSTREAM_NULL;
  pcs_count_t size = 0;
  PCSTREAM_CHECK_FATAL(pcs_mesh_init(&mesh));

  PCSTREAM_CHECK_FATAL(mesh.read_from_file_ply(&mesh, argv[1]));
  PCSTREAM_CHECK_FATAL(
      mesh.write_to_file_ply(&mesh, "test.ply", PCSTREAM_FALSE));

  PCSTREAM_CHECK_FATAL(
      mesh.write_to_buff_serial(&mesh, &buff, &size));
  PCSTREAM_CHECK_FATAL(
      mesh.read_from_buff_serial(&mesh, buff, size));

  free(buff);
  PCSTREAM_CHECK_FATAL(pcs_mesh_destroy(&mesh));
}
