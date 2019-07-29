/*
    Copyright (C) 2019 Julian Rüth

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "arb.h"
#include "arf.h"
#include "mag.h"

int
arb_load_str(arb_t x, const char* data)
{
  const char* split = strchr(data, ' ');
  if (split == NULL)
  {
    return 1;
  }
  split = strchr(split + 1, ' ');
  if (split == NULL)
  {
    return 1;
  }

  size_t midlen = (size_t)(split - data);
  char* mid = (char*)flint_malloc(midlen + 1);
  strncpy(mid, data, midlen);
  mid[midlen] = '\0';

  size_t maglen = strlen(data) - midlen - 1;
  char* mag = (char*)flint_malloc(maglen + 1);
  strncpy(mag, split + 1, maglen);
  mag[maglen] = '\0';

  int err = arf_load_str(arb_midref(x), mid);

  if (!err)
  {
    err = mag_load_str(arb_radref(x), mag);
  }

  flint_free(mid);
  flint_free(mag);
  return err;
}
