#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = 0;
  int zero_value = 1;

  for (int i = 95; i >= 0 && zero_value; i--)
    if (get_bit(value, i)) zero_value = 0;
  error = s21_truncate(value, result);
  if (!error && !zero_value && get_bit(*result, 127) && get_exp(value)) {
    int i = 0;
    while (get_bit(*result, i)) {
      set_bit(result, i, 0);
      i++;
    }
    set_bit(result, i, 1);
  }

  if (zero_value) {
    set_sign(result, 0);
    set_exp(result, 0);
  }

  return error;
}
