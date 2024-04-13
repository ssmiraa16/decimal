#include "s21_decimal.h"

void create_decimal_from_arr(s21_decimal *dec, unsigned int sign,
                             unsigned int scale, unsigned int high,
                             unsigned int mid, unsigned int low) {
  dec->bits[0] = low;
  dec->bits[1] = mid;
  dec->bits[2] = high;
  dec->bits[3] = (sign << 31) + (scale << 16);
}

//--------BITES FuNCS--------------
int get_bit(s21_decimal dec,
            int pos) {  // возвращает значение бита по позиции /pos/. 0 или 1
  return ((dec.bits[pos / 32] >> (pos % 32)) & 1u);
}

void set_bit(s21_decimal *dec, int bit, int val) {  // val = 1 or 0
  if (val) {
    dec->bits[bit / 32] |= (1 << (bit % 32));
  } else {
    dec->bits[bit / 32] &= ~(1 << (bit % 32));
  }
}

void div_by_ten(s21_decimal *value) {
  long initial = 0;
  int rest = 0;
  for (int i = 2; i >= 0; i--) {
    initial = rest * 4294967296 + value->bits[i];
    value->bits[i] = initial / 10;
    rest = initial - value->bits[i] * 10;
  }
}

int get_sign(s21_decimal dec) { return get_bit(dec, 127); }

void set_sign(s21_decimal *dec, int sign) { set_bit(dec, 127, sign); }

int get_exp(s21_decimal d) {
  return (d.bits[3] & 0x00FF0000) >> 16;  // ислоьзование маски и смещения влево
                                          // для получения инта экспоненты
}

void set_exp(s21_decimal *dec, int power) {
  dec->bits[3] = (get_sign(*dec) << 31 | (power << 16));
}

//--------ADDITIONAL FuNCS--------------

int copy_to_buffer(s21_decimal src, s21_decimal *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[2];
  dst->bits[3] = src.bits[3];
  return 0;
}

void init_decimal(s21_decimal *d) {
  d->bits[0] = 0u;
  d->bits[1] = 0u;
  d->bits[2] = 0u;
  d->bits[3] = 0u;
}

int get_float_exp_from_string(char *str) {
  int result = 0;
  char *ptr = str;
  while (*ptr && *ptr != 'E') ptr++;
  result += strtol(++ptr, NULL, 10);
  return result;
}

// проверка граничных условий для s21_from_int_to_decimal
int check_F2D(float src, s21_decimal *dst) {
  int codeError = 0;
  unsigned int mask = 0xFFFE0001;  // 0x0000FFFF for bits 0 to 15 and 0x7F000000
                                   // for bits 24 to 30
                                   // проверки на предельные значения
  if (DECIMAL_MINIMUM > fabsf(src) || DECIMAL_MAX < fabsf(src)) {
    if (0.0f == fabsf(src)) {
      init_decimal(dst);
      if (signbit(src) != 0) {
        set_sign(dst, 1);
      }
    } else {
      codeError = 1;
      if (DECIMAL_MINIMUM > fabsf(src)) init_decimal(dst);
    }
  }
  // проверка на нан, инф, -инф
  if (isinf(src) || isnan(src)) {
    codeError = 1;
    if (signbit(src) != 0) set_sign(dst, 1);
  }
  // проверка на правильность конечной точки
  if ((dst->bits[3] & mask) != 0) codeError = 1;
  return codeError;
}

void get_pow_10_multiDec(int exp, s21_decimal *dst) {
  // printf("\n<<< get_pow_10_multiDec started >>>\n\n");
  int tempExp = exp;
  s21_decimal inti = {{10u, 0u, 0u, 0u}};
  // printf("десятичный дец bits[0] = %d\n", inti.bits[0]);  // Вывод значения
  // экспоненты
  s21_decimal buff = {{0u, 0u, 0u, 0u}};
  copy_to_buffer(*dst, &buff);
  for (int i = 0; i < tempExp; i++, exp--) {
    s21_mul(inti, buff, dst);
    // printf("обновленное значение Е = %d\n", get_exp(*dst));
    copy_to_buffer(*dst, &buff);
    // printf("увечили в десять раз? dec.bits[0] = %d\n", dst->bits[0]);  //
    // Вывод значения экспоненты printf(">>> значение Е в цикле <<<< = %d\n",
    // exp);
  }
  // printf("\n<<< get_pow_10_multiDec ended >>>\n\n");
}

unsigned int get_rid_ofZeros(char *sci_str, int *exp) {
  unsigned int newNumber = 0u;
  int posE = 0, hasDot = 0;
  while (sci_str[posE] != 'E') posE++;
  hasDot = (sci_str[1] == '.' || sci_str[1] == ',') ? 1 : 0;
  int pos_noZero = posE;
  while ((sci_str[pos_noZero] == 'E' || sci_str[pos_noZero] == '0') &&
         pos_noZero >= 0)
    pos_noZero--;
  int tempExp = pos_noZero - hasDot;
  char new_str[8] = {'\0'};
  memcpy(new_str, sci_str, sizeof(char) * (++pos_noZero));
  *exp -= tempExp;
  double tmp = round(strtod(new_str, NULL) * pow(10, tempExp));
  newNumber = (unsigned int)tmp;
  return newNumber;
}

//--------SHIFTS FuNCS--------------
int shift_left(s21_decimal *dec) {
  int owerflow = get_bit(*dec, 95);
  for (int i = 95; i >= 0; i--) {
    if (i != 0) {
      set_bit(dec, i, get_bit(*dec, i - 1));
    } else {
      set_bit(dec, i, 0);
    }
  }
  return owerflow;
}

void shift_right(s21_decimal *dec) {
  for (int i = 0; i < 96; i++) {
    if (i != 95) {
      set_bit(dec, i, get_bit(*dec, i + 1));
    } else {
      set_bit(dec, i, 0);
    }
  }
}

//--------BINARY HELPERS FuNCS--------------

int binary_add(s21_decimal dec1, s21_decimal dec2, s21_decimal *res) {
  init_decimal(res);
  int owerflow = 0, reserv = 0;
  for (int i = 0; i < 96; i++) {
    int bit1 = get_bit(dec1, i);
    int bit2 = get_bit(dec2, i);
    set_bit(res, i, bit1 ^ bit2 ^ reserv);
    reserv = (bit1 && bit2) || (bit1 && reserv) || (bit2 && reserv);
    if (i == 95 && reserv) {
      owerflow = 1;
    }
  }
  return owerflow;
}

void binary_sub(s21_decimal dec1, s21_decimal dec2,
                s21_decimal *res) {  // dec1 > dec2
  init_decimal(res);
  int next_bit1 = 0;
  for (int i = 0; i < 96; i++) {
    int bit1 = get_bit(dec1, i);
    int bit2 = get_bit(dec2, i);
    set_bit(res, i, bit1 ^ bit2);
    if (!bit1 && bit2) {
      next_bit1 = i + 1;
      while ((bit1 = get_bit(dec1, next_bit1)) != 1) {
        set_bit(&dec1, next_bit1, 1);
        next_bit1++;
      }
      set_bit(&dec1, next_bit1, 0);
    }
  }
}

int binary_mul(s21_decimal dec1, s21_decimal dec2, s21_decimal *res) {
  int owerflow = 0;
  s21_decimal result = {0};
  s21_decimal shift_binary_number = {0};
  for (int i = 0; i < 96 && !owerflow; i++) {
    if (get_bit(dec2, i) == 1) {
      shift_binary_number = dec1;
      for (int j = 0; j < i; j++) {
        if (shift_left(&shift_binary_number)) {
          owerflow = 1;
          break;
        }
      }
      if (owerflow || binary_add(shift_binary_number, result, &result)) {
        owerflow = 1;
        break;
      }
    }
  }
  if (!owerflow) *res = result;
  return owerflow;
}

s21_decimal binary_div(s21_decimal dec1, s21_decimal dec2,
                       s21_decimal *res) {  // mod
  init_decimal(res);                        // return remainder
  s21_decimal remainder = {0};              // res integer part
  s21_decimal extra_dec = {0};
  set_bit(&extra_dec, 0, 1);
  if (is_more_simple(dec1, dec2)) {
    while (is_more_or_equal_simple(dec1, dec2)) {
      s21_decimal copy_dec2 = dec2;
      init_decimal(&extra_dec);
      set_bit(&extra_dec, 0, 1);
      while (is_more_or_equal_simple(dec1, copy_dec2) &&
             !(get_bit(dec1, 95) && get_bit(copy_dec2, 95))) {
        shift_left(&copy_dec2);
        shift_left(&extra_dec);
      }
      if (!(get_bit(dec1, 95) && get_bit(copy_dec2, 95)) ||
          (is_more_or_equal_simple(copy_dec2, dec1))) {
        shift_right(&copy_dec2);
        shift_right(&extra_dec);
      }
      binary_sub(dec1, copy_dec2, &dec1);
      binary_add(*res, extra_dec, res);
    }
  } else if (is_equal_simple(dec1, dec2)) {
    *res = extra_dec;
    init_decimal(&dec1);
  }
  remainder = dec1;
  return remainder;
}

// conditions

int is_less_simple(s21_decimal dec1, s21_decimal dec2) {
  int total = 0;
  for (int i = 95; i >= 0; i--) {
    if (get_bit(dec1, i) ^ get_bit(dec2, i)) {
      total = get_bit(dec2, i);
      break;
    }
  }
  return total;
}

int is_equal_simple(s21_decimal dec1, s21_decimal dec2) {
  int total = 1;
  for (int i = 95; i >= 0; i--) {
    if (get_bit(dec1, i) != get_bit(dec2, i)) {
      total = 0;
      break;
    }
  }
  return total;
}

int is_less_or_equal_simple(s21_decimal dec1, s21_decimal dec2) {
  return is_less_simple(dec1, dec2) || is_equal_simple(dec1, dec2);
}

int is_more_simple(s21_decimal dec1, s21_decimal dec2) {
  return !is_less_or_equal_simple(dec1, dec2);
}

int is_more_or_equal_simple(s21_decimal dec1, s21_decimal dec2) {
  return is_more_simple(dec1, dec2) || is_equal_simple(dec1, dec2);
}

int is_not_equal_simple(s21_decimal dec1, s21_decimal dec2) {
  return !is_equal_simple(dec1, dec2);
}

void normalization(s21_decimal *dec1, s21_decimal *dec2) {
  s21_decimal ten = {{10, 0, 0, 0}};
  int sign_dec1 = get_sign(*dec1);
  int sign_dec2 = get_sign(*dec2);
  if (get_exp(*dec1) > 28) {
    int count_div = get_exp(*dec1) - 28;
    for (int i = 0; i < count_div; i++) {
      binary_div(*dec1, ten, dec1);
    }
    set_exp(dec1, 28);
  }
  if (get_exp(*dec2) > 28) {
    int count_div = get_exp(*dec2) - 28;
    for (int i = 0; i < count_div; i++) {
      binary_div(*dec2, ten, dec2);
    }
    set_exp(dec2, 28);
  }
  int exp_dec1 = get_exp(*dec1);
  int exp_dec2 = get_exp(*dec2);

  if (exp_dec1 != exp_dec2) {
    int min_exp = exp_dec1 > exp_dec2 ? exp_dec2 : exp_dec1;   // 0
    int max_exp = exp_dec1 < exp_dec2 ? exp_dec2 : exp_dec1;   // 1
    s21_decimal *min_dec = exp_dec1 > exp_dec2 ? dec2 : dec1;  // max_dec
    s21_decimal *max_dec = exp_dec1 < exp_dec2 ? dec2 : dec1;  // dec2

    while (min_exp != max_exp) {
      int total = binary_mul(*min_dec, ten, min_dec);
      if (!total && min_exp < 29) {
        set_exp(min_dec, min_exp);
        min_exp++;
      } else {
        bank_rounding_max(max_dec, min_exp);
        break;
      }
    }
    set_exp(min_dec, min_exp);
  }
  set_sign(dec1, sign_dec1);
  set_sign(dec2, sign_dec2);
}

void bank_rounding_max(s21_decimal *dec1, int exp_dec2) {
  int diff = get_exp(*dec1) - exp_dec2;
  int sign_dec1 = get_sign(*dec1);
  set_sign(dec1, 0);
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal remainder = {0};
  s21_decimal previous_digit_number = {0};
  for (int i = 0; i < diff; i++) {
    remainder = binary_div(*dec1, ten, dec1);
  }
  if (remainder.bits[0] > 5) {
    binary_add(*dec1, one, dec1);
  } else if (remainder.bits[0] == 5) {
    s21_decimal buff = {0};
    previous_digit_number = binary_div(*dec1, ten, &buff);
    if (!(previous_digit_number.bits[0] % 2)) {
      binary_add(*dec1, one, dec1);
    }
  }
  set_exp(dec1, exp_dec2);
  set_sign(dec1, sign_dec1);
}

void bank_rounding(s21_decimal *dec1, s21_decimal dec2, s21_decimal remainder) {
  int sign_dec1 = get_sign(*dec1);
  set_sign(dec1, 0);
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal five = {{5, 0, 0, 0}};
  s21_decimal digit_number = {0};
  s21_decimal extra_rem = {0};
  s21_decimal previous_digit_number = {0};
  extra_rem = binary_div(remainder, dec2, &digit_number);
  binary_mul(extra_rem, ten, &extra_rem);
  binary_div(extra_rem, dec2, &digit_number);
  if (is_more_simple(digit_number, five)) {
    binary_add(*dec1, one, dec1);
  } else if (is_equal_simple(digit_number, five)) {
    s21_decimal buff = {0};
    previous_digit_number = binary_div(*dec1, ten, &buff);
    if ((previous_digit_number.bits[0] % 2)) {
      binary_add(*dec1, one, dec1);
    }
  }
  set_sign(dec1, sign_dec1);
}
