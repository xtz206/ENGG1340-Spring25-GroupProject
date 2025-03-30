#ifndef ECONOMY_H
#define ECONOMY_H
#include<vector>
#include "game.h"
class City;
class Loader;

class Economy {
friend class Renderer;
friend class Game;
private:
    std::vector<City> cities;
    int totalProductivity;
    int totalDeposit;

public:

    Economy(Loader &ldr);
    int getDepositByCity (City &city) {return city.deposit;}
    int getIncomeByCity (City &city) {return city.productivity;}
    void setCityProductivity (City &city);
    void setCityDeposit (City &city) {city.deposit += city.productivity;}

    int calTotalDeposit();
    int calTotalProductivity();
};
#endif