#include "tinysm.hpp"

#include <iostream>

class turn_on_switch : public event<bool> {
public:
    void press() {

        trigger();
    }
};

class turn_off_switch : public event<bool> {
public:
    void press() {

        trigger();
    }
};

int main() {

    turn_on_switch ton;
    turn_off_switch toff;

    tsm<bool> led_sm({false,
        {
            {{false, true}, ton},
            {{true, false}, toff}
        }
    });

    std::cout << "default state: " << led_sm.get_state() << std::endl;

    ton.press();
    std::cout << "state after [press on swtich]: " << led_sm.get_state() << std::endl;

    ton.press();
    std::cout << "state after [press on swtich]: " << led_sm.get_state() << std::endl;
    
    toff.press();
    std::cout << "state after [press off swtich]: " << led_sm.get_state() << std::endl;

    led_sm.remove_transition({false, true});
    std::cout << "removed turn on switch transition" << std::endl;
    ton.press();
    std::cout << "state after [press on swtich]: " << led_sm.get_state() << std::endl;

    led_sm.add_transition({false, true}, ton);
     std::cout << "added turn on switch transition" << std::endl;
    ton.press();
    std::cout << "state after [press on swtich]: " << led_sm.get_state() << std::endl;
}