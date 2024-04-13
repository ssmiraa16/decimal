CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -D_GNU_SOURCE
GCOVFLAGS = -ftest-coverage -fprofile-arcs
TESTFLAGS = -lcheck
VALGRIND_FLAGS= --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all --verbose

OS := $(shell uname)
ifeq ($(OS), Linux)
    TESTFLAGS += -lm -lrt -lsubunit -lpthread #-coverage
endif

COMMON = sideFuncs.c s21_arithmetics.c s21_comparison.c
HEADER = s21_decimal.h

OBJ_C = $(patsubst %.c, %.o, $(COMMON))

#johnnaib part
OBJ_J = $(patsubst %.c, %.o, $(SOURCES_J))
SOURCES_J = s21_conventors_parsers.c s21_truncate.c

#vanesawo part
OBJ_V = $(patsubst %.c, %.o, $(SOURCES_V))
SOURCES_V = s21_floor.c s21_negate.c s21_round.c

#dev_justicer part
#SOURCE_dJ = COMMON

#biscepsa part
#SOURCE_B = COMMON

TESTS_SOURCE_dJ_B = $(wildcard ./tests/tests_*.c)
TESTS_OBJECTS_dJ_B = $(patsubst ./tests/%.c, ./tests/%.o, ${TESTS_SOURCE_dJ_B})
TESTS_HEADER= ./test_includes/s21_tests.h

TEST_SOURCE_V = ./tests_V/test.c

TEST_SOURCE_J = ./t_cases/test02.c

all: s21_decimal.a gcov_report

s21_decimal.a: $(COMMON) $(HEADER) $(SOURCES_J) $(SOURCES_V)
	$(CC) $(CFLAGS) -c $(COMMON) $(HEADER) $(SOURCES_J) $(SOURCES_V)
	ar rc s21_decimal.a $(OBJ_C) $(OBJ_J) $(OBJ_V)
	ranlib s21_decimal.a
	rm -rf *.o

test: s21_decimal.a main.c ${TESTS_SOURCE_dJ_B} $(TESTS_HEADER) $(TEST_SOURCE_J) $(TEST_SOURCE_V)
	$(CC) $(CFLAGS) main.c ${TESTS_SOURCE_dJ_B} $(TEST_SOURCE_J) $(TEST_SOURCE_V) $(TESTFLAGS) s21_decimal.a -o atest
	./atest
	rm -rf *.o *.a atest

gcov_report: $(COMMON) $(HEADER) $(SOURCES_J) $(SOURCES_V)  $(TESTS_HEADER) main.c ${TESTS_SOURCE_dJ_B} $(TEST_SOURCE_J) $(TEST_SOURCE_V)
	$(CC) $(CFLAGS) $(GCOVFLAGS) main.c ${TESTS_SOURCE_dJ_B} $(TEST_SOURCE_J) $(TEST_SOURCE_V) $(TESTFLAGS) $(COMMON) $(SOURCES_J) $(SOURCES_V) -o test
	./test
	lcov -t "test" -o test_report.info -c -d .
	genhtml -o report test_report.info
	rm -rf *.o *.gcno *.gcda *.info test

open:
	open report/index.html

valgrind: s21_decimal.a main.c ${TESTS_SOURCE_dJ_B} $(TESTS_HEADER) $(TEST_SOURCE_J) $(TEST_SOURCE_V)
	$(CC) $(CFLAGS) main.c ${TESTS_SOURCE_dJ_B} $(TEST_SOURCE_J) $(TEST_SOURCE_V) $(TESTFLAGS) s21_decimal.a -o test
	valgrind $(VALGRIND_FLAGS) ./test

style: clean
	cp ../materials/linters/.clang-format .clang-format
	clang-format -n $(COMMON) $(HEADER) $(SOURCES_J) $(SOURCES_V) $(TESTS_SOURCE_dJ_B) $(TESTS_HEADER) $(TEST_SOURCE_V) $(TEST_SOURCE_J) main.c

leaks: s21_decimal.a main.c ${TESTS_SOURCE_dJ_B} $(TESTS_HEADER) $(TEST_SOURCE_J) $(TEST_SOURCE_V)
	$(CC) $(CFLAGS) main.c ${TESTS_SOURCE_dJ_B} $(TEST_SOURCE_J) $(TEST_SOURCE_V) $(TESTFLAGS) s21_decimal.a -o test
	leaks -atExit -- ./test

clean:
	rm -rf *.o *.gcno *.a *.gcda *.gch
	rm -rf ./report/ *.info
	rm -rf test_report test

rebuild: clean all
