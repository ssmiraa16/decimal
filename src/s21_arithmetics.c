#include "s21_decimal.h"
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  init_decimal(result);
  s21_decimal remainder = {0};
  s21_decimal extra_dec = {0};
  s21_decimal zero = {0};
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal max_dec = {{4294967295u, 4294967295u, 4294967295u, 0}};
  int digit_degree = 0;
  int sign = 1;
  int total = 0;
  int flag_full = 0;
  int flag_more = 0;
  int degree = get_exp(value_1) - get_exp(value_2);
  if (get_sign(value_1) == get_sign(value_2)) sign = 0;
  set_sign(&value_1, 0);
  set_sign(&value_2, 0);
  if (is_equal_simple(value_2, zero)) {
    total = 3;
  } else {
    remainder = binary_div(value_1, value_2, &extra_dec);
    binary_add(*result, extra_dec, result);
    int degree_remainder = 0;
    binary_div(extra_dec, ten, &extra_dec);
    while (!is_equal_simple(extra_dec, zero)) {
      binary_div(extra_dec, ten, &extra_dec);
      digit_degree++;
    }
    while (is_not_equal_simple(remainder, zero) &&
           degree_remainder < 28 - digit_degree) {
      if (is_more_simple(*result, max_dec)) break;
      binary_mul(remainder, ten, &remainder);
      remainder = binary_div(remainder, value_2, &extra_dec);
      binary_mul(*result, ten, result);
      binary_add(*result, extra_dec, result);
      degree_remainder++;
      if (is_not_equal_simple(remainder, zero) &&
          (degree_remainder == 28 - digit_degree)) {
        flag_full = 1;
      }
    }
    int total_degree = degree + degree_remainder;
    if (flag_full && (total_degree < 29)) {
      bank_rounding(result, value_2, remainder);
    }
    while (total_degree > 28) {
      flag_more = 1;
      remainder = binary_div(*result, ten, result);
      total_degree--;
    }
    if (flag_full && flag_more) {
      bank_rounding(result, ten, remainder);
    }
    while (total_degree < 0) {
      binary_mul(*result, ten, result);
      total_degree++;
    }
    set_sign(result, sign);
    set_exp(result, total_degree);
  }
  return total;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  init_decimal(result);
  int total = 0;
  s21_decimal zero = {0};
  s21_decimal digit_num = {0};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal ten = {{10, 0, 0, 0}};
  int sign = 1;

  int degree = get_exp(value_1) + get_exp(value_2);
  if (get_sign(value_1) == get_sign(value_2)) sign = 0;
  set_sign(&value_1, 0);
  set_sign(&value_2, 0);
  if (!is_equal_simple(value_2, zero) && !is_equal_simple(value_1, zero)) {
    total = binary_mul(value_1, value_2, result);
    if (!total) {
      if (degree > 28) {
        while (degree != 29) {
          binary_div(*result, ten, result);
          degree--;
        }
        degree--;
        digit_num = binary_div(*result, ten, result);
        if (digit_num.bits[0] > 5) {
          binary_add(*result, one, result);
        } else if (digit_num.bits[0] == 5) {
          s21_decimal buff = {0};
          digit_num = binary_div(*result, ten, &buff);
          if (digit_num.bits[0] % 2 == 1) {
            binary_add(*result, one, result);
          }
        }
      }
      set_sign(result, sign);
      set_exp(result, degree);
    }
  }
  if (total == 1 && sign) total = 2;
  return total;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int total = 0;
  init_decimal(result);
  int sign_dec1 = get_sign(value_1);
  int sign_dec2 = get_sign(value_2);
  if (get_exp(value_1) != get_exp(value_2)) {
    normalization(&value_1, &value_2);
  }
  if (sign_dec1 == sign_dec2) {
    if (!sign_dec1) {  // a + b
      total = binary_add(value_1, value_2, result);
      set_sign(result, 0);
    } else {  // -a + -b
      if (binary_add(value_1, value_2, result)) {
        total = 2;
      }
      set_sign(result, 1);
    }
  } else {
    if (!sign_dec1 && sign_dec2) {  // a + -b
      if (is_less_simple(value_1, value_2)) {
        binary_sub(value_2, value_1, result);
        set_sign(result, 1);
      } else {
        binary_sub(value_1, value_2, result);
        set_sign(result, 0);
      }
    } else {  // -a + b
      if (is_less_simple(value_1, value_2)) {
        binary_sub(value_2, value_1, result);
        set_sign(result, 0);
      } else {
        binary_sub(value_1, value_2, result);
        set_sign(result, 1);
      }
    }
  }
  set_exp(result, get_exp(value_1));
  return total;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int total = 0;
  init_decimal(result);
  int sign_dec2 = get_sign(value_2);
  if (!sign_dec2) {  // a - b -> a + -b
    set_sign(&value_2, 1);
    total = s21_add(value_1, value_2, result);
  } else {  // a - (-b) - > a + b
    set_sign(&value_2, 0);
    total = s21_add(value_1, value_2, result);
  }
  return total;
}
