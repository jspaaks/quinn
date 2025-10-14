#include <criterion/criterion.h>
#include <equation/equation.h>

Test(sample, test) {
    double actual = eval(1.0);
    double expected = 2.0;
    cr_assert(actual == expected);
}
