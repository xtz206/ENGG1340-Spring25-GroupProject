#include "Economy.h"
#include "game.h"
#include "loader.h"

Economy::Economy(Loader &ldr): cities(ldr.load_cities())
{
    income = cal_income();
    total = income;
}

int Economy::get_total(void) const
{
    return total;
}

int Economy::get_status(City* city) const
{
    return 1;
}

int Economy::cal_total(void)
{
    total = cal_income();
    return total;
}