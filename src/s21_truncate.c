#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = 0;
  int int_pow = 0;
  *result = value;

  if (result == NULL) error = 1;

  for (int i = 126; i >= 96 && !error; i--) {
    if ((i > 119 || i < 112) && get_bit(value, i))
      error = 1;
    else if ((i < 120 && i > 111)) {
      int_pow += get_bit(value, i) * pow(2, i - 112);
      if (int_pow > 28) error = 1;
    }
  }

  if (!error && int_pow) {
    for (int i = 119; i > 111; i--) set_bit(result, i, 0);

    for (int i = 0; i < int_pow; i++) div_by_ten(result);
  }

  set_exp(result, 0);
  set_sign(result, get_sign(value));
  return error;
}
