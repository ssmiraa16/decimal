#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int error = 0;
  if (dst) {
    init_decimal(dst);
    if (src < 0) {
      set_sign(dst, 1);
    }
    dst->bits[0] = src > 0 ? src : ~(src - 1);
  } else
    error++;
  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = 0;
  if (dst && src.bits[1] == 0 && src.bits[2] == 0 && get_bit(src, 31) == 0) {
    int expo = get_exp(src);
    if (expo <= 28) {
      int divisor = ipow(10, expo);
      *dst = src.bits[0] / divisor;
      *dst *= get_sign(src) ? -1 : 1;
    } else {
      error = 1;
    }
  } else {
    error = 1;
  }
  return error;
}

int ipow(int base, int exponent) {
  int result = 1;
  while (exponent != 0) {
    if ((exponent & 1) == 1) {
      result *= base;
    }
    base *= base;
    exponent >>= 1;
  }
  return result;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error = 0, expo = get_exp(src);
  // s21_decimal mx_d = {{4294967295u, 4294967295u, 4294967295u, 0}};
  s21_decimal mn_d = {{4294967295u, 4294967295u, 4294967295u, 0}};
  set_sign(&mn_d, 1);
  // if (!(s21_is_less_or_equal(src, mx_d) || s21_is_greater_or_equal(src,
  // mn_d))) error = 1;
  if (expo > 28) error = 1;
  if (dst && !error) {
    int sign = (get_sign(src) ? -1 : 1);
    int mantiss = get_exp(src);
    double_t temp = 0.0;
    for (int a = 0; a <= 95; a++) {
      if (get_bit(src, a) == 1) temp += pow(2, a);
    }
    *dst = (float)(temp / pow(10, mantiss) * sign);
  } else
    error = 1;
  return error;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int codeError = check_F2D(src, dst);
  if (!codeError && dst) {
    init_decimal(dst);
    char sci_src[64];
    int sign = src < 0.f ? 1 : 0;
    sprintf(sci_src, "%.6E", fabsf(src));
    int exp = get_float_exp_from_string(sci_src);
    if (exp <= -23) {
      int float_precision = exp + 28;
      sprintf(sci_src, "%.*E", float_precision, fabsf(src));
    }
    if (abs(exp) > 28) codeError = 1;
    if (!codeError) {
      init_decimal(dst);
      unsigned int newDec = get_rid_ofZeros(sci_src, &exp);
      dst->bits[0] = newDec;
      if (exp > 0)
        get_pow_10_multiDec(exp, dst);
      else
        set_exp(dst, abs(exp));
      if (sign) set_sign(dst, 1);
    }
  } else
    codeError = 1;
  return codeError;
}
