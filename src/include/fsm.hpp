#ifndef FSM_HPP
#define FSM_HPP


namespace fsm {


struct NoGuard {
    template <typename FromStateType, typename EventType, typename TargetStateType>
    bool operator()(FromStateType&, const EventType&, TargetStateType&) {
        return true;
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


template <std::size_t N, typename Fsm>
struct TryTransitionHelper {
    template <typename EventType>
    static bool tryTransition(Fsm* fsm, const EventType& evt);
};


template <typename Fsm>
struct TryTransitionHelper<0, Fsm> {
    template <typename EventType>
    static bool tryTransition(Fsm* fsm, const EventType& evt);
};


template <typename FsmDef, typename FsmData>
class Fsm {
public:
    Fsm(FsmData& fsmData);

    void start();

    template<typename EventType>
    void process_event(const EventType& evt);

    FsmData& userData();

private:
    using States = typename FsmDef::States;
    using Transitions = typename FsmDef::Transitions;
    using InitialState = typename FsmDef::InitialState;

    template <std::size_t N, typename SomeFsm>
    friend class TryTransitionHelper;

    std::size_t     m_current_state_id;
    States          m_states;

    FsmData        &m_fsmData;

    template <typename EventType>
    void transitionNotFound(const EventType&);

    template <typename EventType, std::size_t N>
    bool tryTransition(const EventType& evt);


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
template <typename EventType>
void Fsm<FsmDef, FsmData>::process_event(const EventType& evt) {
    constexpr std::size_t NumTransitions = std::tuple_size<Transitions>::value;
    TryTransitionHelper<NumTransitions-1, Fsm<FsmDef, FsmData>>::tryTransition(this, evt);
}


template <typename FsmDef, typename FsmData>
template <typename EventType>
void Fsm<FsmDef, FsmData>::transitionNotFound(const EventType&) {
    throw std::runtime_error("no transition found");
}


template <typename FsmDef, typename FsmData>
template <typename EventType, std::size_t N>
bool Fsm<FsmDef, FsmData>::tryTransition(const EventType& evt) {
    using TransitionType        = typename std::tuple_element<N, Transitions>::type;
    using FromStateType         = typename std::tuple_element<0, TransitionType>::type;
    using TransitionEventType   = typename std::tuple_element<1, TransitionType>::type;
    using GuardType             = typename std::tuple_element<2, TransitionType>::type;
    using ToStateType           = typename std::tuple_element<3, TransitionType>::type;
    using ActionType            = typename std::tuple_element<4, TransitionType>::type;

    constexpr std::size_t fromStateIndex        = TypeIndex<FromStateType, States>::value;
    constexpr std::size_t toStateIndex          = TypeIndex<ToStateType, States>::value;

    if (!std::is_same<EventType, TransitionEventType>::value)
        return false;

    if (fromStateIndex != m_current_state_id)
        return false;

    FromStateType  &fromState = std::get<fromStateIndex>(m_states);
    ToStateType    &toState   = std::get<toStateIndex>(m_states);
    GuardType       guard;
    
    bool ok = guard(fromState, evt, toState);

    if (ok) {
        FsmDef::exitAction(*this, fromState);
        m_current_state_id = toStateIndex;
        ActionType action;
        action(*this, fromState, evt, toState);
        FsmDef::entryAction(*this, toState);
        return true;
    } else
        return false;
}


template <typename FsmDef, typename FsmData>
FsmData& Fsm<FsmDef, FsmData>::userData() {
    return m_fsmData;
}



template <std::size_t N, typename Fsm>
template <typename EventType>
bool TryTransitionHelper<N, Fsm>::tryTransition(Fsm* fsm, const EventType& evt) {
    if (fsm->tryTransition<EventType, N>(evt))
        return true;
    else
        return TryTransitionHelper<N-1, Fsm>::tryTransition(fsm, evt);
};


template <typename Fsm>
template <typename EventType>
bool TryTransitionHelper<0, Fsm>::tryTransition(Fsm* fsm, const EventType& evt) {
    if (fsm->tryTransition<EventType, 0>(evt))
        return true;
    else {
        fsm->transitionNotFound(evt);
        return false;
    }
}

} // namespace fsm
#endif

