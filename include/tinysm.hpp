#ifndef TINYSM_HPP
#define TINY_SM_HPP

#include <map>
#include <tuple>
#include <vector>
#include <functional>

namespace tinysm {

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

        for(const auto& trigger_ : triggers_) {
            if(trigger_())
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

        triggers_.emplace_back(std::move(trigger));
    }

    void clear_triggers() {
        triggers_.clear();
    }

    void set_get_state(GetStateFunction&& get_state) {

        get_state_ = std::move(get_state);
    }

    std::vector<TriggerFunction> triggers_;
    GetStateFunction get_state_;
};

template <typename State>
class event_group : public event<State> {

public:

    void trigger() {
        event<State>::trigger();
    }

    event_group(std::vector<event<State>*>&& events) {

        events_ = std::move(events);

        for(const auto& event_ : events_) {

            event_->set_trigger([this](){

                this->trigger();
                return true;
            });
        }
    }

private:

    std::vector<event<State>*> events_;
};

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

        for(auto& [states, event_] : transitions_) {

            const auto& [start_state, end_state] = states;
            set_triggers(event_, start_state, end_state);
        }
    }

    void set_triggers(event<State>& event_, const State& start_state, const State& end_state) {

        event_.set_trigger([this, &start_state, &end_state](){ 

            if(this->state_ == start_state) {
                this->state_ = end_state;
                return true;
            }
            return false;
        });

        event_.set_get_state([this]() {
            return this->state_;
        });
    }
};

}

#endif