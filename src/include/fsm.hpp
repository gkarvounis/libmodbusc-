#ifndef FSM_HPP
#define FSM_HPP


namespace fsm {

class State {
public:
    virtual std::string name() const  { return "noname state"; }
    virtual void entryAction() { std::cout << "entry " << name() << std::endl; };
    virtual void exitAction() { std::cout << "exit " << name() << std::endl; };
};


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


template <typename FsmDef>
class Fsm {
public:
    using States = typename FsmDef::States;
    using Transitions = typename FsmDef::Transitions;
    using InitialState = typename FsmDef::InitialState;

    void start();

    template<typename EventType>
    void process_event(const EventType& evt);

    Fsm();

private:
    template <std::size_t N, typename SomeFsm>
    friend class TryTransitionHelper;

    std::size_t     m_current_state_id;
    States          m_states;

    template <typename EventType>
    void transitionNotFound(const EventType&);

    template <typename EventType, std::size_t N>
    bool tryTransition(const EventType& evt);


};


template <typename FsmDef>
Fsm<FsmDef>::Fsm() :
    m_current_state_id(TypeIndex<InitialState, States>::value),
    m_states()
{}


template <typename FsmDef>
void Fsm<FsmDef>::start() {
    constexpr std::size_t entryStateIndex = TypeIndex<InitialState, States>::value;
    InitialState& initialState = std::get<entryStateIndex>(m_states);
    initialState.entryAction();
}


template <typename FsmDef>
template <typename EventType>
void Fsm<FsmDef>::process_event(const EventType& evt) {
    constexpr std::size_t NumTransitions = std::tuple_size<Transitions>::value;
    TryTransitionHelper<NumTransitions-1, Fsm<FsmDef>>::tryTransition(this, evt);
}


template <typename FsmDef>
template <typename EventType>
void Fsm<FsmDef>::transitionNotFound(const EventType&) {
    throw std::runtime_error("no transition found");
}


template <typename FsmDef>
template <typename EventType, std::size_t N>
bool Fsm<FsmDef>::tryTransition(const EventType& evt) {
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
        fromState.exitAction();
        m_current_state_id = toStateIndex;
        ActionType action;
        action(*this, fromState, evt, toState);
        toState.entryAction();
        return true;
    } else
        return false;
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

