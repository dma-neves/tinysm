#ifndef TINYSM_HPP
#define TINY_SM_HPP

#include <map>
#include <tuple>

#include "event.hpp"
#include "event_group.hpp"

template <typename State>
class tsm {

    using Transitions = std::map<std::tuple<State,State>, event<State>&>;

public:
    tsm(State&& initial_state, Transitions&& transitions) : state_(std::move(initial_state)), transitions_(std::move(transitions))
    {
        set_triggers();
    }

    tsm(State& initial_state, Transitions& transitions) : state_(initial_state), transitions_(transitions)
    {
        set_triggers();
    }

    const State& get_state()
    {
        return state_;
    }

    bool is_in_state(const State&& state)
    {
        return state_ == state;
    }

    bool is_in_state(const State& state) {
        return state_ == state;
    }

    void add_transition(std::tuple<State,State>&& states, event<State>& transition) {
        auto key = std::move(states);
        add_transition(key, transition);
    }

    void add_transition(std::tuple<State,State>& states, event<State>& transition) {
        const auto& [start_state, end_state] = states;
        set_triggers(transition, start_state, end_state);
        transitions_.emplace(states, transition);
    }

    void remove_transition(std::tuple<State,State>&& states) {

        auto key = std::move(states);
        remove_transition(key);
    }

    void remove_transition(std::tuple<State,State>& states) {

        const auto it = transitions_.find(states);
        if(it != transitions_.end()) {
            it->second.clear_triggers();
            transitions_.erase(states);
        }
    }

private:
    State state_;
    Transitions transitions_;

    void set_triggers() {

        for(auto& [states, transition] : transitions_) {

            const auto& [start_state, end_state] = states;
            set_triggers(transition, start_state, end_state);
        }
    }

    void set_triggers(event<State>& transition, const State& start_state, const State& end_state) {

        transition.set_trigger([this, &start_state, &end_state](){ 

            if(this->state_ == start_state) {
                this->state_ = end_state;
                return true;
            }
            return false;
        });

        transition.set_get_state([this]() {
            return this->state_;
        });
    }
};

#endif