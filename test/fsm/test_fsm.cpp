#define CATCH_CONFIG_MAIN

#include "Catch.hpp"

#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <iostream>

#include "fsm.hpp"


template <typename T>
static std::string toString(const T& s);


class FsmDef {
public:
    struct Evt0 {};
    struct Evt1 {};
    struct Evt2 {};


    struct State0 {};
    struct State1 {};
    struct State2 {};


    using States = std::tuple<
        State0,
        State1,
        State2
    >;


    using InitialState = State0;


    using Transitions = std::tuple<
        std::tuple<State0, Evt0, fsm::NoGuard, State0>,
        std::tuple<State0, Evt1, fsm::NoGuard, State1>,
        std::tuple<State1, Evt1, fsm::NoGuard, State1>,
        std::tuple<State1, Evt0, fsm::NoGuard, State0>,
        std::tuple<State1, Evt2, fsm::NoGuard, State2>
    >;


    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    static void transitionAction(Fsm& fsm, FromStateType& fromState, const EventType& evt, TargetStateType& toState) {
        fsm.userData().push_back(toString(fromState) + ":" + toString(evt) + "->" + toString(toState));
    }

    template <typename Fsm, typename State>
    static void entryAction(Fsm& fsm, State& state) {
        fsm.userData().push_back("entry:" + toString(state));
    }


    template <typename Fsm, typename State>
    static void exitAction(Fsm& fsm, State& state) {
        fsm.userData().push_back("exit:" + toString(state));
    }
};


template <>
std::string toString(const FsmDef::State0&) {
    return "State0";
}

template <>
std::string toString(const FsmDef::State1&) {
    return "State1";
}

template <>
std::string toString(const FsmDef::State2&) {
    return "State2";
}

template <>
std::string toString(const FsmDef::Evt0&) {
    return "Evt0";
}

template <>
std::string toString(const FsmDef::Evt1&) {
    return "Evt1";
}

template <>
std::string toString(const FsmDef::Evt2&) {
    return "Evt2";
}


TEST_CASE("test basic fsm functionality", "[fsm]") {
    std::vector<std::string>transitions;
    fsm::Fsm<FsmDef, std::vector<std::string>> fsm(transitions);

    fsm.start();
    fsm.process_event(FsmDef::Evt0());
    fsm.process_event(FsmDef::Evt0());
    fsm.process_event(FsmDef::Evt1());
    fsm.process_event(FsmDef::Evt2());

    REQUIRE(transitions == (std::vector<std::string>{
        "entry:State0",
        "exit:State0",
        "State0:Evt0->State0",
        "entry:State0",
        "exit:State0",
        "State0:Evt0->State0",
        "entry:State0",
        "exit:State0",
        "State0:Evt1->State1",
        "entry:State1",
        "exit:State1",
        "State1:Evt2->State2",
        "entry:State2"

    }));
}

