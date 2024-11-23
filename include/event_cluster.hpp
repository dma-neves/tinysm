#ifndef EVENT_CLUSTER
#define EVENT_CLUSTER

#include "event.hpp"

#include <vector>

template <typename State>
class event_cluster : public event<State> {

public:

    void trigger() {
        event<State>::trigger();
    }

    event_cluster(std::vector<event<State>*>&& transitions) {

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