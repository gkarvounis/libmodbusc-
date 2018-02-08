#ifndef FSM_HPP
#define FSM_HPP


namespace fsm {


struct NoGuard {
    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    bool operator()(Fsm&, FromStateType&, const EventType&, TargetStateType&) {
        return true;
    }
};


struct NoAction {
    template <typename Fsm, typename FromStateType, typename EventType, typename TargetStateType>
    void operator()(Fsm&, FromStateType&, EventType&, TargetStateType&) {
    }
};


template <typename T, typename TupleType>
struct TypeIndex {
    static_assert(std::tuple_size<TupleType>::value != 0, "type not found in tuple");
};


template <typename T, typename... Types>
struct TypeIndex<T, std::tuple<T, Types...>> {
    static const std::size_t value = 0;
};


template <typename T, typename U, typename... Types>
struct TypeIndex<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + TypeIndex<T, std::tuple<Types...>>::value;
};


template <typename Transition>
using GetFromState = typename std::tuple_element<0, Transition>::type;

template <typename Transition>
using GetEvent = typename std::tuple_element<1, Transition>::type;

template <typename Transition>
using GetGuard = typename std::tuple_element<2, Transition>::type;

template <typename Transition>
using GetToState = typename std::tuple_element<3, Transition>::type;

template <typename Transition>
using GetAction = typename std::tuple_element<4, Transition>::type;


template <typename Fsm, typename FromState, typename TransitionEvent, typename Guard, typename ToState, typename Action, typename CurrentState, typename OccuredEvent>
struct TransitionExecutor {
    static bool process_event(Fsm&, CurrentState&, const OccuredEvent&, ToState&) {
        return false;
    }
};


template <typename Fsm, typename CurrentState, typename OccuredEvent, typename Guard, typename ToState, typename Action>
struct TransitionExecutor<Fsm, CurrentState, OccuredEvent, Guard, ToState, Action, CurrentState, OccuredEvent> {
    static bool process_event(Fsm& fsm, CurrentState& fromState, const OccuredEvent& evt, ToState& toState) {
        if (Guard()(fsm, fromState, evt, toState)) {
            fsm.exitAction(fromState);
            Action()(fsm, fromState, evt, toState);
            fsm.entryAction(toState);
            return true;
        } else {
            return false;
        }
    }
};


template <typename Fsm, std::size_t N, typename CurrentState, typename OccuredEvent>
struct TransitionTableIterator {
    using Transition = typename std::tuple_element<N, typename Fsm::Transitions>::type;

    using FromState = GetFromState<Transition>;
    using TransitionEvent = GetEvent<Transition>;
    using Guard = GetGuard<Transition>;
    using ToState = GetToState<Transition>;
    using Action = GetAction<Transition>;


    static bool process_event(Fsm& fsm, CurrentState& fromState, const OccuredEvent& evt) {
        static constexpr std::size_t ToStateIndex = TypeIndex<ToState, typename Fsm::States>::value;

        ToState& toState = std::get<ToStateIndex>(fsm.m_states);

        if (TransitionExecutor<Fsm, FromState, TransitionEvent, Guard, ToState, Action, CurrentState, OccuredEvent>::process_event(fsm, fromState, evt, toState))
            return true;
        else
            return TransitionTableIterator<Fsm, N-1, CurrentState, OccuredEvent>::process_event(fsm, fromState, evt);
    }
};


template <typename Fsm, typename CurrentState, typename OccuredEvent>
struct TransitionTableIterator<Fsm, 0, CurrentState, OccuredEvent> {
    using Transition = typename std::tuple_element<0, typename Fsm::Transitions>::type;

    using FromState = GetFromState<Transition>;
    using TransitionEvent = GetEvent<Transition>;
    using Guard = GetGuard<Transition>;
    using ToState = GetToState<Transition>;
    using Action = GetAction<Transition>;


    static bool process_event(Fsm& fsm, CurrentState& fromState, const OccuredEvent& evt) {
        static constexpr std::size_t ToStateIndex = TypeIndex<ToState, typename Fsm::States>::value;

        ToState& toState = std::get<ToStateIndex>(fsm.m_states);

        if (TransitionExecutor<Fsm, FromState, TransitionEvent, Guard, ToState, Action, CurrentState, OccuredEvent>::process_event(fsm, fromState, evt, toState))
            return true;
        else {
            fsm.transitionNotFound(evt);
            return false;
        }
    }
};


template <typename Fsm, typename OccuredEvent, std::size_t StateIndex>
struct StatesIterator {
    using States = typename Fsm::States;
    using FromState = typename std::tuple_element<StateIndex, States>::type;
    static constexpr std::size_t NumTransitions = std::tuple_size<typename Fsm::Transitions>::value;

    static bool process_event(Fsm& fsm, const OccuredEvent& evt) {
        if (fsm.m_current_state_id == StateIndex) {
            FromState& fromState = std::get<StateIndex>(fsm.m_states);

            return TransitionTableIterator<Fsm, NumTransitions-1, FromState, OccuredEvent>::process_event(fsm, fromState, evt);
        }
        else
            return StatesIterator<Fsm, OccuredEvent, StateIndex-1>::process_event(fsm, evt);
    }
};


template <typename Fsm, typename OccuredEvent>
struct StatesIterator<Fsm, OccuredEvent, 0> {
    using States = typename Fsm::States;
    using FromState = typename std::tuple_element<0, States>::type;
    static constexpr std::size_t NumTransitions = std::tuple_size<typename Fsm::Transitions>::value;

    static bool process_event(Fsm& fsm, const OccuredEvent& evt) {
        if (fsm.m_current_state_id != 0)
            return false;

        FromState& fromState = std::get<0>(fsm.m_states);

        return TransitionTableIterator<Fsm, NumTransitions-1, FromState, OccuredEvent>::process_event(fsm, fromState, evt);
    }
};



template <typename FsmDef, typename FsmData>
class Fsm {
public:
    Fsm(FsmData& fsmData);

    void start();

    template<typename OccuredEvent>
    void process_event(const OccuredEvent& evt);

    FsmData& userData();

private:
    using States = typename FsmDef::States;
    using Transitions = typename FsmDef::Transitions;
    using InitialState = typename FsmDef::InitialState;


    template <typename Fsm, typename OccuredEvent, std::size_t StateIndex>
    friend struct StatesIterator;

    template <typename Fsm, std::size_t N, typename CurrentState, typename OccuredEvent>
    friend struct TransitionTableIterator;

    template <typename Fsm, typename FromState, typename TransitionEvent, typename Guard, typename ToState, typename Action, typename CurrentState, typename OccuredEvent>
    friend struct TransitionExecutor;

    std::size_t     m_current_state_id;
    States          m_states;

    FsmData        &m_fsmData;


    template <typename EventType>
    void transitionNotFound(const EventType&);


    template <typename ToState>
    void setCurrentState(ToState& toState);


    template <typename State>
    void exitAction(State& state) {
        FsmDef::exitAction(*this, state);
    }


    template <typename State>
    void entryAction(State& state) {
        m_current_state_id = TypeIndex<State, States>::value;
        FsmDef::entryAction(*this, state);
    }
};


template <typename FsmDef, typename FsmData>
Fsm<FsmDef, FsmData>::Fsm(FsmData& fsmData) :
    m_current_state_id(TypeIndex<InitialState, States>::value),
    m_states(),
    m_fsmData(fsmData)
{}


template <typename FsmDef, typename FsmData>
void Fsm<FsmDef, FsmData>::start() {
    constexpr std::size_t entryStateIndex = TypeIndex<InitialState, States>::value;
    InitialState& initialState = std::get<entryStateIndex>(m_states);
    FsmDef::entryAction(*this, initialState);
}


template <typename FsmDef, typename FsmData>
template <typename OccuredEvent>
void Fsm<FsmDef, FsmData>::process_event(const OccuredEvent& evt) {
    constexpr std::size_t NumStates = std::tuple_size<States>::value;

    if (!StatesIterator<Fsm<FsmDef, FsmData>, OccuredEvent, NumStates-1>::process_event(*this, evt)) {
        throw std::runtime_error("no transition found");
    }
}


template <typename FsmDef, typename FsmData>
template <typename EventType>
void Fsm<FsmDef, FsmData>::transitionNotFound(const EventType&) {
    throw std::runtime_error("no transition found");
}


template <typename FsmDef, typename FsmData>
FsmData& Fsm<FsmDef, FsmData>::userData() {
    return m_fsmData;
}


template <typename FsmDef, typename FsmData>
template <typename ToState>
void Fsm<FsmDef, FsmData>::setCurrentState(ToState& toState) {
    constexpr std::size_t toStateIndex = TypeIndex<ToState, States>::value;
    m_current_state_id = toStateIndex;
}

} // namespace fsm
#endif

