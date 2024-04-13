#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int status = 0, scale = 0;
  s21_decimal valueBuff;
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal half = {{5, 0, 0, 0}};
  int sign = get_sign(value);
  init_decimal(&valueBuff);
  set_sign(&value, 0);
  if (result == NULL || scale > 28) {
    status = 1;
  } else {
    init_decimal(result);
    scale = get_exp(value);
    if (scale > 0) {
      s21_decimal remainder;
      s21_truncate(value, &remainder);
      s21_sub(remainder, value, &remainder);
      set_sign(&remainder, 0);
      set_exp(&remainder, get_exp(remainder) - 1);
      s21_truncate(remainder, &remainder);
      if (s21_is_greater_or_equal(remainder, half) == 1) {
        status = s21_truncate(value, result);
        result->bits[3] = 0;
        s21_add(*result, one, &valueBuff);
        copy_to_buffer(valueBuff, result);
        set_sign(result, sign);
      } else {
        status = s21_truncate(value, result);
      }
    } else {
      for (int i = 0; i < 4; i++) {
        result->bits[i] = value.bits[i];
      }
    }
    set_sign(result, sign);
  }
  return status;
}