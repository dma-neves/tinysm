#include "tinysm.hpp"

#include <iostream>

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



int main() {

    tick_dozen td;
    tick_second ts(td);

    tsm<Digit> units(0, {
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
    });

    tsm<Digit> dozens(0, {
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
    });

    for(int i = 0; i < 13; i++) {
        std::cout << dozens.get_state() << units.get_state() << std::endl;
        ts.tick();
    }
}