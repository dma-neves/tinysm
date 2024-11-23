# Tiny State Machine

This is a tiny C++ template library to define state machines declaratively using event-based transitions. Given tha lack o standard C++ state machine library, I created this library as a very simple and light-weight solution do define state machines in my projects. This is a header-only library, meaning that it can easily be copied into an existing project.

Note: This library was inspired by a similar solution developed by [tobyselway](github.com/tobyselway).

## Build

```bash
git clone git@github.com:dma-neves/tinysm.git
cd tinysm
mkdir build
cd build
cmake  ..
make
examples/elevator
```

## Usage

tinysm is composed of four concepts:

- The `State` type, which can be a primitive data type, an enum, etc. See examples below.
- The `event`, which describes an event that can trigger a transition between states. We can define such events using a class that inherits from `event`. In this class we can implement any logic we desire, and whenever we want to trigger a transition, we call the `trigger` method from the parent class. If no specific logic is required, we simply instantiate an `event` object, and call `trigger` whenever necessary.
- The `event_group`. If multiple events should trigger the same state transition, we group them using an `event_group`. The `event_group` receives a vector of `event` addresses.
- The `tsm`, which allows us to declaratively define the relation between state transitions and events or event groups. The `tsm` constructor receives a default state, and a map whose keys are tuples `<start_state, end_state>`, and values are `event`s or `event_group`s. If needed, we can also dynamically add and remove transitions.

## Elevator Example

![alt text](other/elevator_resize.png)


```c++
#include "tinysm.hpp"

enum ElevatorState {
    MovingUp,
    MovingDown,
    DoorOpen,
    DoorClosed,
};

class door_timer : public event<ElevatorState> {
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

class keep_door_open : public event<ElevatorState> {
public:
    keep_door_open(door_timer& dt) : door_timer_(dt) {}
    
    void trigger() {
        door_timer_.reset();
        event<ElevatorState>::trigger();
    }
}

int main() {

    door_timer door_timer_;

    keep_door_open press_up_at_elevator_floor_(door_timer_);
    keep_door_open press_down_at_elevator_floor_(door_timer_);

    event<ElevatorState> 
        press_up_above_elevator_floor_,
        press_up_below_elevator_floor_,
        press_down_above_elevator_floor_,
        press_down_below_elevator_floor_,
        press_open_door_,
        select_floor_above_elevator_floor_,
        select_floor_below_elevator_floor_,
        reached_selected_floor_;

    event_group<ElevatorState> open_door({&press_up_at_elevator_floor_, &press_down_at_elevator_floor_, &press_open_door_});
    event_group<ElevatorState> go_down({&select_floor_below_elevator_floor_, &press_up_below_elevator_floor_, &press_down_below_elevator_floor_});
    event_group<ElevatorState> go_up({&select_floor_above_elevator_floor_, &press_up_above_elevator_floor_, &press_down_above_elevator_floor_});

    tsm<ElevatorState> elevator_sm(ElevatorState::DoorClosed, 
        {
            {{ElevatorState::DoorOpen,      ElevatorState::DoorClosed}, door_timer_},
            {{ElevatorState::DoorClosed,    ElevatorState::DoorOpen},   open_door},
            {{ElevatorState::DoorOpen,      ElevatorState::DoorOpen},   open_door},
            {{ElevatorState::DoorClosed,    ElevatorState::MovingUp},   go_up},
            {{ElevatorState::MovingUp,      ElevatorState::DoorOpen},   reached_selected_floor_},
            {{ElevatorState::DoorClosed,    ElevatorState::MovingDown}, go_down},
            {{ElevatorState::MovingDown,    ElevatorState::DoorOpen},   reached_selected_floor_},
        });

    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    press_up_at_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    door_timer_.update(3); 
    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    select_floor_above_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // MovingUp

    reached_selected_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // DoorOpen

    door_timer_.update(3);
    print_elevator_state(elevator_sm.get_state()); // DoorClosed

    press_down_below_elevator_floor_.trigger();
    print_elevator_state(elevator_sm.get_state()); // MovingDown
}

```