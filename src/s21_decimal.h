#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UINT 4294967295u
#define DECIMAL_MAX 79228157791897854723898736640.0f
#define DECIMAL_MINIMUM \
  0.00000000000000000000000000010000000031710768509710513471352647538147514756461109f
#define s21_MAXDEC 2e96 - 1
#define EXP_DECIMAL_MASK 0xFF0000

typedef struct {
  unsigned int bits[4];
} s21_decimal;

void create_decimal_from_arr(s21_decimal *dec, unsigned int sign,
                             unsigned int scale, unsigned int high,
                             unsigned int mid, unsigned int low);

// ----------------Вспомогательные функции----------------------
int get_sign(s21_decimal value);
void set_sign(s21_decimal *value, int val);
int get_exp(s21_decimal d);
void set_exp(s21_decimal *dec, int power);
void init_decimal(s21_decimal *d);
int copy_to_buffer(s21_decimal value, s21_decimal *buffer);

int ipow(int base, int exponent);
void div_by_ten(s21_decimal *value);

int get_bit(s21_decimal dec, int pos);
void set_bit(s21_decimal *value, int bit, int val);
int get_float_exp_from_string(char *str);
int check_F2D(float src, s21_decimal *dst);
void get_pow_10_multiDec(int exp, s21_decimal *dst);

int shift_left(s21_decimal *dec);
void shift_right(s21_decimal *dec);
void bank_rounding(s21_decimal *dec1, s21_decimal dec2, s21_decimal remainder);
void bank_rounding_max(s21_decimal *dec1, int exp_dec2);
int binary_add(s21_decimal dec1, s21_decimal dec2, s21_decimal *res);
void binary_sub(s21_decimal dec1, s21_decimal dec2,
                s21_decimal *res);  // dec1 > dec2
int binary_mul(s21_decimal dec1, s21_decimal dec2, s21_decimal *res);
s21_decimal binary_div(s21_decimal dec1, s21_decimal dec2, s21_decimal *Res);

void normalization(s21_decimal *dec1, s21_decimal *dec2);

int is_less(s21_decimal dec1, s21_decimal dec2);
int is_equal(s21_decimal dec1, s21_decimal dec2);
int is_less_or_equal(s21_decimal dec1, s21_decimal dec2);
int is_greater(s21_decimal dec1, s21_decimal dec2);
int is_greater_or_equal(s21_decimal dec1, s21_decimal dec2);
int s21_is_not_equal(s21_decimal dec1, s21_decimal dec2);

int check_F2D(float src, s21_decimal *dst);
void init_decimal(s21_decimal *d);
int get_float_exp_from_string(char *str);
unsigned int get_rid_ofZeros(char *sci_str, int *exp);

// extra conditions

int is_less_simple(s21_decimal dec1, s21_decimal dec2);
int is_equal_simple(s21_decimal dec1, s21_decimal dec2);
int is_less_or_equal_simple(s21_decimal dec1, s21_decimal dec2);
int is_more_simple(s21_decimal dec1, s21_decimal dec2);
int is_more_or_equal_simple(s21_decimal dec1, s21_decimal dec2);
int is_not_equal_simple(s21_decimal dec1, s21_decimal dec2);

// ----------------Арифметические операторы----------------------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// ----------------Операторы сравнения--------------------------
int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal a, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);

// ----------------Преобразователи------------------------------
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// ----------------Другие функции-------------------------------
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

#endif  // SRC_S21_DECIMAL_H_
