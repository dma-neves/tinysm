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
    using EnterExistCallbacks = std::map<State, std::function<void(void)>>;

public:

    tsm(State&& initial_state, Transitions&& transitions) : tsm(std::move(initial_state), std::move(transitions), {}, {})
    {
        callbacks_active = false;
    }

    tsm(State&& initial_state, Transitions&& transitions, EnterExistCallbacks&& enter_callbacks, EnterExistCallbacks&& exit_callbacks) : 
        state_(std::move(initial_state)), 
        transitions_(std::move(transitions)),
        enter_callbacks_(std::move(enter_callbacks)),
        exit_callbacks_(std::move(exit_callbacks)),
        callbacks_active(true)
    {
        set_triggers();
    }

    template <typename S, typename T, typename E>
    tsm(S&& initial_state, T&& transitions, E&& enter_callbacks, E&& exit_callbacks)
        : state_(std::forward<S>(initial_state)), 
          transitions_(std::forward<T>(transitions)),
          enter_callbacks_(std::forward<E>(enter_callbacks)),
          exit_callbacks_(std::forward<E>(exit_callbacks))
    {
        set_triggers();
        callbacks_active = enter_callbacks_.size() != 0 && exit_callbacks_.size() != 0;
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
    EnterExistCallbacks enter_callbacks_;
    EnterExistCallbacks exit_callbacks_;
    bool callbacks_active = false;

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

                if(callbacks_active) {
                    const auto& exit_callback_it = this->exit_callbacks_.find(start_state);
                    if(exit_callback_it != this->exit_callbacks_.end())
                        exit_callback_it->second();

                    const auto& enter_callback_it = this->enter_callbacks_.find(end_state);
                    if(enter_callback_it != this->enter_callbacks_.end())
                        enter_callback_it->second();
                }

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