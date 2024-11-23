#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <iostream>
#include <vector>

template <typename State>
class tsm;

template <typename State>
class event_group;

template <typename State>
class event {

public:

    friend class tsm<State>;
    friend class event_group<State>;

    void trigger() {

        for(const auto& trigger_callback : trigger_) {
            if(trigger_callback())
                break;
        }
    }

    State get_state() {
        return get_state_();
    }

protected:

    using TriggerFunction = std::function<bool(void)>;
    using GetStateFunction = std::function<State(void)>;

    void set_trigger(TriggerFunction&& trigger) {

        trigger_.emplace_back(std::move(trigger));
    }

    void clear_triggers() {
        trigger_.clear();
    }

    void set_get_state(GetStateFunction&& get_state) {

        get_state_ = std::move(get_state);
    }

    std::vector<TriggerFunction> trigger_;
    GetStateFunction get_state_;
};

#endif