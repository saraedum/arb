/*
    Copyright (C) 2019 Julian Rüth

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "arf.h"
#include "mag.h"

char *
mag_dump_str(const mag_t x)
{
  arf_t y;
  arf_init(y);
  arf_set_mag(y, x);
  char* res = arf_dump_str(y);
  arf_clear(y);
  return res;
}
