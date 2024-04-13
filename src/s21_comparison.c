#include "s21_decimal.h"

int s21_is_less(s21_decimal a, s21_decimal b) {
  int res = 0;
  int signA = get_sign(a);
  int signB = get_sign(b);
  int zero = a.bits[0] == 0 && a.bits[1] == 0 && a.bits[2] == 0 &&
             b.bits[0] == 0 && b.bits[1] == 0 && b.bits[2] == 0;
  if (zero) {
    res = 0;
  } else if (signA == 1 && signB == 0) {
    res = 1;
  } else if (signA == 0 && signB == 1) {
    res = 0;
  } else if (signA == signB) {
    normalization(&a, &b);
    for (int i = 2; i >= 0; i--) {
      if (a.bits[i] < b.bits[i]) {
        res = (signA == 1) ? 0 : 1;
        break;
      } else if (a.bits[i] > b.bits[i]) {
        res = (signA == 1) ? 1 : 0;
        break;
      }
    }
  }
  return res;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  return (s21_is_greater(a, b)) ? 0 : 1;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  int res = 0;
  int signA = get_sign(a);
  int signB = get_sign(b);
  int zero = a.bits[0] == 0 && a.bits[1] == 0 && a.bits[2] == 0 &&
             b.bits[0] == 0 && b.bits[1] == 0 && b.bits[2] == 0;
  if (zero) {
    res = 0;
  } else if (signA == 0 && signB == 1) {
    res = 1;
  } else if (signA == 1 && signB == 0) {
    res = 0;
  } else if (signA == signB) {
    normalization(&a, &b);
    for (int i = 2; i >= 0; i--) {
      if (a.bits[i] > b.bits[i]) {
        res = (signA == 0) ? 1 : 0;
        break;
      } else if (a.bits[i] < b.bits[i]) {
        res = (signA == 0) ? 0 : 1;
        break;
      }
    }
  }
  return res;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  return (s21_is_less(a, b)) ? 0 : 1;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  int res = 1;  // res = 0 -- False, 1 -- True

  int zero = a.bits[0] == 0 && a.bits[1] == 0 && a.bits[2] == 0 &&
             b.bits[0] == 0 && b.bits[1] == 0 && b.bits[2] == 0;

  int equal = a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] &&
              a.bits[2] == b.bits[2];

  int signA = get_sign(a);
  int signB = get_sign(b);
  int power_1 = get_exp(a);
  int power_2 = get_exp(b);

  if (zero) {
    res = 1;
  } else if (signA != signB) {
    res = 0;
  } else {
    res = power_1 == power_2 && equal;
  }
  return res;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return (s21_is_equal(a, b)) ? 0 : 1;
}
