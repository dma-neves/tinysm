#ifndef EVENT_GROUP
#define EVENT_GROUP

#include "event.hpp"

#include <vector>

template <typename State>
class event_group : public event<State> {

public:

    void trigger() {
        event<State>::trigger();
    }

    event_group(std::vector<event<State>*>&& transitions) {

        transitions_ = std::move(transitions);

        for(const auto& transition : transitions_) {

            transition->set_trigger([this](){

                this->trigger();
                return true;
            });
        }
    }

private:

    std::vector<event<State>*> transitions_;
};

#endif