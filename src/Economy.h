#ifndef ECONOMY_H
#define ECONOMY_H
#include<vector>


class City;
class Game;
class Loader;

class Economy
{
    friend class Renderer;

private:
    int total;
    int income;
    std::vector<City> cities;

public:
    Economy(Loader& ldr);
    int get_total(void) const;
    int get_status(City* city) const;

    int cal_total(void);
    int cal_productivity(void);
    void set_state(int p);
    int cal_income(void);

};

#endif