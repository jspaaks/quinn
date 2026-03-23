#include <criterion/criterion.h>
#include <equation/equation.h>


Test(test_equation, test1) {
    double actual = eval(1.0);
    double expected = 4.0 / 2.0;
    cr_assert(actual == expected);
}

Test(test_equation, test2) {
    double actual = eval(0.5);
    double expected = 16.0 / 5.0;
    cr_assert(actual == expected);
}

Test(test_equation, test3) {
    double actual = eval(2.0);
    double expected = 4.0 / 5.0;
    cr_assert(actual == expected);
}
