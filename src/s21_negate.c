#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  result->bits[3] = ((get_sign(*result) ^ 1) << 31) | (get_exp(*result) << 16);
  return 0;
}
