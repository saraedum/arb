/*
    Copyright (C) 2019 Julian Rüth

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <assert.h>

#include "mag.h"
#include "arf.h"

int
mag_load_str(mag_t x, const char* data)
{
    arf_t y;
    arf_init(y);

    int err = arf_load_str(y, data);
    if (err)
    {
        arf_clear(y);
        return err;
    }

    if (arf_is_special(y))
    {
        if (arf_is_zero(y))
        {
            mag_zero(x);
        }
        else if (arf_is_pos_inf(y))
        {
            mag_inf(x);
        }
        else
        {
            // a mag cannot be negative infinity or NaN
            flint_abort();
        }
    }
    else
    {
        fmpz_t mantissa, exponent;
        fmpz_init(mantissa);
        fmpz_init(exponent);

        arf_get_fmpz_2exp(mantissa, exponent, y);

        assert(fmpz_cmp_ui(mantissa, 1 << MAG_BITS) < 0);

        mag_set_ui(x, fmpz_get_ui(mantissa));

        mag_mul_2exp_fmpz(x, x, exponent);

        fmpz_clear(exponent);
        fmpz_clear(mantissa);
    }

    arf_clear(y);

    return err;
}
