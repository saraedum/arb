/*
    Copyright (C) 2019 Julian Rüth

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>

#include "mag.h"

int mag_load_file(mag_t x, FILE* stream)
{
    char* pos;
    char* data = NULL;
    size_t length = 0;
    int  ret = 0;
    
    ssize_t nread = getdelim(&data, &length, ' ', stream);
    if (nread == -1)
        return nread;
    if (data[nread-1] == ' ')
        data[nread-1] = '\0';

    /* replace all '$' with ' ' */
    for (pos = data; (pos = strchr(pos, '$')) != NULL; *pos = ' ');

    ret = mag_load_str(x, data);
    free(data);
    return ret;
}



