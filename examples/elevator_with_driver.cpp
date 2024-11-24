#include "tinysm.hpp"

#include <iostream>

using namespace tinysm;

enum ElevatorState {

    MovingUp,
    MovingDown,
    DoorOpen,
    DoorClosed,
};

void print_elevator_state(ElevatorState state) {

    std::cout << "elevator: ";
    switch (state)
    {
        case MovingUp: std::cout << "MovingUp"; break;
        case MovingDown: std::cout << "MovingDown"; break;
        case DoorOpen: std::cout << "DoorOpen"; break;
        case DoorClosed: std::cout << "DoorClosed"; break;
    }
    std::cout << std::endl;
}

class close_door_timer : public event<ElevatorState> {

public:
    void update(int increment = 1) {

        seconds += increment;
        if(seconds >= threhsold) {
            reset();
            trigger();
        }
    }

    void reset() { seconds = 0; }

private:
    int seconds = 0;
    static constexpr int threhsold = 3;
};

class open_door : public event<ElevatorState> {
public:
    open_door(close_door_timer& dt) : close_door_timer_(dt) {}

    void trigger() {
        close_door_timer_.reset();
        event<ElevatorState>::trigger();
    }

private:
    close_door_timer& close_door_timer_;
};

class elevator_driver {

public:

    elevator_driver(event<ElevatorState>& reached_floor) : reached_floor_(reached_floor) {

    }

    void update(int increment = 1) {

        if(moving_up) {
            current_floor += increment;

            if(current_floor >= destination_floor) {
                current_floor = destination_floor;
                reached_floor_.trigger();
                moving_up = false;
                moving_down = false;
            }
        }
        else if(moving_down) {
            current_floor -= increment;

            if(current_floor <= destination_floor) {
                current_floor = destination_floor;
                reached_floor_.trigger();
                moving_up = false;
                moving_down = false;
            }
        }
     }

    void move_up() { 

        moving_up = true;
        moving_down = false;
    }

    void move_down() {

        moving_up = false;
        moving_down = true;
    }
    
    void open_door() { /* control elevator door motors to open */ }
    void close_door() { /* control elevator door motors to close */ }

    void set_destination_floor(int floor) { destination_floor = floor; }
private:
    int destination_floor = 0;
    int current_floor = 0;

    bool moving_up = false;
    bool moving_down = false;

    event<ElevatorState>& reached_floor_;
};

int main() {

    close_door_timer close_door_timer_;

    open_door press_up_at_elevator_floor_(close_door_timer_);
    open_door press_down_at_elevator_floor_(close_door_timer_);
    open_door press_open_door_(close_door_timer_);

    event<ElevatorState> 
        press_up_above_elevator_floor_,
        press_up_below_elevator_floor_,
        press_down_above_elevator_floor_,
        press_down_below_elevator_floor_,
        select_floor_above_elevator_floor_,
        select_floor_below_elevator_floor_,
        reached_selected_floor_;

    event_group<ElevatorState> open_door_({&press_up_at_elevator_floor_, &press_down_at_elevator_floor_, &press_open_door_});
    event_group<ElevatorState> go_down_({&select_floor_below_elevator_floor_, &press_up_below_elevator_floor_, &press_down_below_elevator_floor_});
    event_group<ElevatorState> go_up_({&select_floor_above_elevator_floor_, &press_up_above_elevator_floor_, &press_down_above_elevator_floor_});

    elevator_driver ed(reached_selected_floor_);

    tsm<ElevatorState> elevator_sm(ElevatorState::DoorClosed, 
        {
            {{ElevatorState::DoorOpen, ElevatorState::DoorClosed}, close_door_timer_},
            {{ElevatorState::DoorClosed, ElevatorState::DoorOpen}, open_door_},
            {{ElevatorState::DoorOpen, ElevatorState::DoorOpen}, open_door_},

            {{ElevatorState::DoorClosed, ElevatorState::MovingUp}, go_up_},
            {{ElevatorState::MovingUp, ElevatorState::DoorOpen}, reached_selected_floor_},
            {{ElevatorState::DoorClosed, ElevatorState::MovingDown}, go_down_},
            {{ElevatorState::MovingDown, ElevatorState::DoorOpen}, reached_selected_floor_},
        },
        {
            {ElevatorState::DoorClosed, [&ed](){ ed.close_door();   } },
            {ElevatorState::DoorOpen,   [&ed](){ ed.open_door();    } },
            {ElevatorState::MovingUp,   [&ed](){ ed.move_up();      } },
            {ElevatorState::MovingDown, [&ed](){ ed.move_down();    } }
        },
        {}
    );

    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    press_up_at_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    close_door_timer_.update();
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    close_door_timer_.update();
    close_door_timer_.update(); 
    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    press_down_at_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    close_door_timer_.update(3);
    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    ed.set_destination_floor(10);
    select_floor_above_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // MovingUp

    ed.update(10);
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    close_door_timer_.update(3);
    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    ed.set_destination_floor(2);
    press_down_below_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // MovingDown

    ed.update(8);
    print_elevator_state(elevator_sm.get_state()); // DoorOpen
}