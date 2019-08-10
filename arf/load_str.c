/*
    Copyright (C) 2019 Julian Rüth

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "arf.h"

static const char* arf_serialized_zero = "0 0";
static const char* arf_serialized_pos_inf = "0 -1";
static const char* arf_serialized_neg_inf = "0 -2";
static const char* arf_serialized_nan = "0 -3";

char *
arf_dump_str(const arf_t x)
{
    size_t res_len;
    char * res;

    if (arf_is_special(x))
    {
        const char* ret;
        if (arf_is_zero(x))
        {
            ret = arf_serialized_zero;
        }
        else if (arf_is_pos_inf(x))
        {
            ret = arf_serialized_pos_inf;
        }
        else if (arf_is_neg_inf(x))
        {
            ret = arf_serialized_neg_inf;
        }
        else if (arf_is_nan(x))
        {
            ret = arf_serialized_nan;
        }
        else
        {
            /* Impossible to happen; all the special values have been treated above. */
            flint_abort();
        }

        res_len = strlen(ret);
        res = (char*)flint_malloc(res_len + 1);
        strcpy(res, ret);
    }
    else
    {
        fmpz_t mantissa, exponent;

        fmpz_init(mantissa);
        fmpz_init(exponent);

        arf_get_fmpz_2exp(mantissa, exponent, x);

        res_len = (fmpz_sgn(mantissa) < 0) + fmpz_sizeinbase(mantissa, 16) + 1
            + (fmpz_sgn(exponent) < 0) + fmpz_sizeinbase(exponent, 16);
        res = (char*)flint_malloc(res_len + 1);

        fmpz_get_str(res, 16, mantissa);
        strcat(res, " ");
        fmpz_get_str(res + strlen(res), 16, exponent);

        fmpz_clear(mantissa);
        fmpz_clear(exponent);
    }

    if(strlen(res) != res_len) flint_abort(); /* assert */

    return res;
}

int
arf_load_str(arf_t x, const char* data)
{
    int err = 0;

    if (strcmp(data, arf_serialized_zero) == 0)
    {
        arf_zero(x);
    }
    else if (strcmp(data, arf_serialized_neg_inf) == 0)
    {
        arf_neg_inf(x);
    }
    else if (strcmp(data, arf_serialized_pos_inf) == 0)
    {
        arf_pos_inf(x);
    }
    else if (strcmp(data, arf_serialized_nan) == 0)
    {
        arf_nan(x);
    }
    else
    {
        fmpz_t mantissa, exponent;
        char * e_str;
        char * m_str;

        fmpz_init(mantissa);
        fmpz_init(exponent);

        e_str = strchr(data, ' ');
        if (e_str == NULL)
        {
            return 1;
        }

        m_str = (char*)flint_malloc(e_str - data + 1);
        strncpy(m_str, data, e_str - data);
        m_str[e_str - data] = '\0';

        err = fmpz_set_str(mantissa, m_str, 16);

        flint_free(m_str);

        if (err)
        {
            fmpz_clear(exponent);
            fmpz_clear(mantissa);
            return err;
        }

        err = fmpz_set_str(exponent, e_str + 1, 16);

        if (err)
        {
            fmpz_clear(exponent);
            fmpz_clear(mantissa);
            return err;
        }

        arf_set_fmpz_2exp(x, mantissa, exponent);

        fmpz_clear(exponent);
        fmpz_clear(mantissa);
    }

    return err;
}
