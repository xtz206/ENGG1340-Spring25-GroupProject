#include "Economy.h"
#include "loader.h"
#define BASE_PRODUCTIVITY 50

Economy::Economy(Loader &ldr): cities(ldr.load_cities()) {
    totalProductivity = calTotalProductivity();
    totalDeposit = 0;
    totalDeposit = calTotalDeposit();
}
void Economy::setCityProductivity (City &city) {
    int hp = city.hitpoint;
    if (hp > 0) {
        city.productivity = BASE_PRODUCTIVITY + (hp / 10);
    } else {
        city.productivity = 0;
    }
}

int Economy::calTotalProductivity() {
    totalProductivity = 0;
    for (auto &city : cities) {
        if (city.hitpoint > 0) {
           totalDeposit += city.productivity;
        }
    }
    return totalProductivity;
}

int Economy::calTotalDeposit() {
    totalDeposit = 0;
    for (auto &city : cities) {
        totalDeposit += city.deposit;
    }
    return totalDeposit;
}