/*=============================================================================

    This file is part of ARB.

    ARB is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ARB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARB; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2012 Fredrik Johansson

******************************************************************************/

#include <math.h>
#include "double_extras.h"
#include "hypgeom.h"

void
fmpr_gamma_ui_lbound(fmpr_t x, ulong n, long prec)
{
    if (n == 0) abort();

    if (n < 250)
    {
        fmpz_t t;
        fmpz_init(t);
        fmpz_fac_ui(t, n - 1);
        fmpr_set_round_fmpz(x, t, prec, FMPR_RND_DOWN);
        fmpz_clear(t);
    }
    else
    {
        /* (2 pi/x)^(1/2) * (x/e)^x < Gamma(x) */
        fmpr_t t, u;

        fmpr_init(t);
        fmpr_init(u);

        /* lower bound for 2 pi */
        fmpr_set_ui_2exp_si(t, 843314855, -27);
        fmpr_div_ui(t, t, n, prec, FMPR_RND_DOWN);
        fmpr_sqrt(t, t, prec, FMPR_RND_DOWN);

        /* lower bound for 1/e */
        fmpr_set_ui_2exp_si(u, 197503771, -29);
        fmpr_mul_ui(u, u, n, prec, FMPR_RND_DOWN);

        fmpr_pow_sloppy_ui(u, u, n, prec, FMPR_RND_DOWN);

        fmpr_mul(x, t, u, prec, FMPR_RND_DOWN);

        fmpr_clear(t);
        fmpr_clear(u);
    }
}

void
fmpr_gamma_ui_ubound(fmpr_t x, ulong n, long prec)
{
    if (n == 0) abort();

    if (n < 250)
    {
        fmpz_t t;
        fmpz_init(t);
        fmpz_fac_ui(t, n - 1);
        fmpr_set_round_fmpz(x, t, prec, FMPR_RND_UP);
        fmpz_clear(t);
    }
    else
    {
        fmpr_t t, u;
        fmpr_init(t);

        /* Gamma(x) < e * (x / e)^x -- TODO: use a tighter bound */

        fmpr_init(t);
        fmpr_init(u);

        /* upper bound for 1/e */
        fmpr_set_ui_2exp_si(u, 197503773, -29);
        fmpr_mul_ui(u, u, n, prec, FMPR_RND_UP);
        fmpr_pow_sloppy_ui(u, u, n, prec, FMPR_RND_UP);

        /* upper bound for e */
        fmpr_set_ui_2exp_si(t, 364841613, -27);
        fmpr_mul(x, t, u, prec, FMPR_RND_UP);

        fmpr_clear(t);
        fmpr_clear(u);
    }
}

long
hypgeom_root_bound(const mag_t z, int r)
{
    if (r == 0)
    {
        return 0;
    }
    else
    {
        fmpr_t t;
        long v;
        fmpr_init(t);
        mag_get_fmpr(t, z);
        fmpr_root(t, t, r, MAG_BITS, FMPR_RND_UP);
        fmpr_add_ui(t, t, 1, MAG_BITS, FMPR_RND_UP);
        v = fmpr_get_si(t, FMPR_RND_UP);
        fmpr_clear(t);
        return v;
    }
}

/*
Given T(K), compute bound for T(n) z^n.

We need to multiply by

z^n * 1/rf(K+1,m)^r * (rf(K+1,m)/rf(K+1-A,m)) * (rf(K+1-B,m)/rf(K+1-2B,m))

where m = n - K. This is equal to

z^n * 

(K+A)! (K-2B)! (K-B+m)!
-----------------------    * ((K+m)! / K!)^(1-r)
(K-B)! (K-A+m)! (K-2B+m)!
*/
void
hypgeom_term_bound(mag_t Tn, const mag_t TK, long K, long A, long B, int r, const mag_t z, long n)
{
    mag_t t, u, num;
    long m;

    mag_init(t);
    mag_init(u);
    mag_init(num);

    m = n - K;

    if (m < 0)
    {
        printf("hypgeom term bound\n");
        abort();
    }

    /* TK * z^n */
    mag_pow_ui(t, z, n);
    mag_mul(num, TK, t);

    /* numerator: (K+A)! (K-2B)! (K-B+m)! */
    mag_fac_ui(t, K+A);
    mag_mul(num, num, t);

    mag_fac_ui(t, K-2*B);
    mag_mul(num, num, t);

    mag_fac_ui(t, K-B+m);
    mag_mul(num, num, t);

    /* denominator: (K-B)! (K-A+m)! (K-2B+m)! */
    mag_rfac_ui(t, K-B);
    mag_mul(num, num, t);

    mag_rfac_ui(t, K-A+m);
    mag_mul(num, num, t);

    mag_rfac_ui(t, K-2*B+m);
    mag_mul(num, num, t);

    /* ((K+m)! / K!)^(1-r) */
    if (r == 0)
    {
        mag_fac_ui(t, K+m);
        mag_mul(num, num, t);

        mag_rfac_ui(t, K);
        mag_mul(num, num, t);
    }
    else if (r != 1)
    {
        mag_fac_ui(t, K);
        mag_rfac_ui(u, K+m);
        mag_mul(t, t, u);

        mag_pow_ui(t, t, r-1);
        mag_mul(num, num, t);
    }

    mag_set(Tn, num);

    mag_clear(t);
    mag_clear(u);
    mag_clear(num);
}


/* z = max(x-y, 0), rounding down [lower bound] */
void
mag_sub_lower(mag_t z, const mag_t x, const mag_t y)
{
    if (mag_is_special(x) || mag_is_special(y))
    {
        if (mag_is_zero(y))
            mag_set(z, x);
        else if (mag_is_zero(x) || mag_is_inf(y))
            mag_zero(z);
        else
            mag_inf(z);
    }
    else
    {
        fmpr_t t, u;

        fmpr_init(t);
        fmpr_init(u);

        mag_get_fmpr(t, x);
        mag_get_fmpr(u, y);

        fmpr_sub(t, t, u, MAG_BITS, FMPR_RND_DOWN);

        if (fmpr_sgn(t) <= 0)
            mag_zero(z);
        else  /* XXX: exact? */
            mag_set_fmpz_2exp_fmpz_lower(z, fmpr_manref(t), fmpr_expref(t));

        fmpr_clear(t);
        fmpr_clear(u);
    }
}

void
mag_set_ui_2exp_si(mag_t z, ulong v, long e)
{
    mag_set_ui(z, v);
    mag_mul_2exp_si(z, z, e);
}

double
mag_get_d(const mag_t z)
{
    long exp;

    if (mag_is_zero(z))
        return 0.0;
    else if (mag_is_inf(z))
        return D_INF;

    /* XXX: overflow/underflow properly */
    exp = MAG_EXP(z);

    if (exp < -1000)
        return ldexp(1.0, -1000);
    else if (exp > 1000)
        return D_INF;
    else
        return ldexp(MAG_MAN(z), exp - MAG_BITS);
}

long
hypgeom_bound(mag_t error, int r,
    long A, long B, long K, const mag_t TK, const mag_t z, long tol_2exp)
{
    mag_t Tn, t, u, one, tol, num, den;
    long n, m;

    mag_init(Tn);
    mag_init(t);
    mag_init(u);
    mag_init(one);
    mag_init(tol);
    mag_init(num);
    mag_init(den);

    mag_one(one);
    mag_set_ui_2exp_si(tol, 1UL, -tol_2exp);

    /* approximate number of needed terms */
    n = hypgeom_estimate_terms(z, r, tol_2exp);

    /* required for 1 + O(1/k) part to be decreasing */
    n = FLINT_MAX(n, K + 1);

    /* required for z^k / (k!)^r to be decreasing */
    m = hypgeom_root_bound(z, r);
    n = FLINT_MAX(n, m);

    /*  We now have |R(k)| <= G(k) where G(k) is monotonically decreasing,
        and can bound the tail using a geometric series as soon
        as soon as G(k) < 1. */

    /* bound T(n-1) */
    hypgeom_term_bound(Tn, TK, K, A, B, r, z, n-1);

    while (1)
    {
        /* bound R(n) */
        mag_mul_ui(num, z, n);
        mag_mul_ui(num, num, n - B);

        mag_set_ui_lower(den, n - A);
        mag_mul_ui_lower(den, den, n - 2*B);

        if (r != 0)
        {
            mag_set_ui_lower(u, n);
            mag_pow_ui_lower(u, u, r);
            mag_mul_lower(den, den, u);
        }

        mag_div(t, num, den);

        /* multiply bound for T(n-1) by bound for R(n) to bound T(n) */
        mag_mul(Tn, Tn, t);

        /* geometric series termination check */
        /* u = max(1-t, 0), rounding down [lower bound] */
        mag_sub_lower(u, one, t);

        if (!mag_is_zero(u))
        {
            mag_div(u, Tn, u);

            if (mag_cmp(u, tol) < 0)
            {
                mag_set(error, u);
                break;
            }
        }

        /* move on to next term */
        n++;
    }

    mag_clear(Tn);
    mag_clear(t);
    mag_clear(u);
    mag_clear(one);
    mag_clear(tol);
    mag_clear(num);
    mag_clear(den);

    return n;
}

