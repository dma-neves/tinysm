#include "tinysm.hpp"

#include <iostream>

using namespace tinysm;

using Digit = int;

class tick_dozen : public event<Digit> {
public:
    tick_dozen() {}

    void tick() {
        trigger();
    }
};

class tick_second : public event<Digit> {
public:

    tick_second(tick_dozen& td) : tm_(td) {}

    void tick() {

        if(get_state() == 9) {
            tm_.tick();
        }

        trigger();
    }

private:
    tick_dozen& tm_;
};

class clock_driver {
public:
    void set_unit_digit(int d) { std::cout << "set units to " << d << std::endl; }
    void set_dozen_digit(int d) { std::cout << "set dozens to " << d << std::endl; }
};

int main() {

    tick_dozen td;
    tick_second ts(td);
    clock_driver cd;

    tsm<Digit> units(0, 
        {
            {{0,1}, ts},
            {{1,2}, ts},
            {{2,3}, ts},
            {{3,4}, ts},
            {{4,5}, ts},
            {{5,6}, ts},
            {{6,7}, ts},
            {{7,8}, ts},
            {{8,9}, ts},
            {{9,0}, ts}
        },
        {
            {0, [&cd](){ cd.set_unit_digit(0); } },
            {1, [&cd](){ cd.set_unit_digit(1); } },
            {2, [&cd](){ cd.set_unit_digit(2); } },
            {3, [&cd](){ cd.set_unit_digit(3); } },
            {4, [&cd](){ cd.set_unit_digit(4); } },
            {5, [&cd](){ cd.set_unit_digit(5); } },
            {6, [&cd](){ cd.set_unit_digit(6); } },
            {7, [&cd](){ cd.set_unit_digit(7); } },
            {8, [&cd](){ cd.set_unit_digit(8); } },
            {9, [&cd](){ cd.set_unit_digit(9); } },
        },
        {}
    );

    tsm<Digit> dozens(0, 
        {
            {{0,1}, td},
            {{1,2}, td},
            {{2,3}, td},
            {{3,4}, td},
            {{4,5}, td},
            {{5,6}, td},
            {{6,7}, td},
            {{7,8}, td},
            {{8,9}, td},
            {{9,0}, td}
        },
        {
            {0, [&cd](){ cd.set_dozen_digit(0); } },
            {1, [&cd](){ cd.set_dozen_digit(1); } },
            {2, [&cd](){ cd.set_dozen_digit(2); } },
            {3, [&cd](){ cd.set_dozen_digit(3); } },
            {4, [&cd](){ cd.set_dozen_digit(4); } },
            {5, [&cd](){ cd.set_dozen_digit(5); } },
            {6, [&cd](){ cd.set_dozen_digit(6); } },
            {7, [&cd](){ cd.set_dozen_digit(7); } },
            {8, [&cd](){ cd.set_dozen_digit(8); } },
            {9, [&cd](){ cd.set_dozen_digit(9); } },
        },
        {}
    );

    for(int i = 0; i < 13; i++) {
        std::cout << dozens.get_state() << units.get_state() << std::endl;
        ts.tick();
    }
}