
#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>
#include <boost/msm/event_traits.hpp>

using namespace std;
namespace msm = boost::msm;
using namespace msm::front;
namespace mpl = boost::mpl;
// for And_ operator
using namespace msm::front::euml;




// events
struct play {};
struct end_pause {};
struct stop {};
struct pause {};
struct open_close {};

namespace boost { namespace msm{
    template<> struct is_kleene_event< open_close > {
      typedef boost::mpl::true_ type;
    };
}}

// A "complicated" event type that carries some data.
enum DiskTypeEnum
{
    DISK_CD=0,
    DISK_DVD=1
};
struct cd_detected
{
    cd_detected(std::string name, DiskTypeEnum diskType)
        : name(name),
        disc_type(diskType)
    {}

    std::string name;
    DiskTypeEnum disc_type;
};

// The list of FSM states
struct Empty : public msm::front::state<>
{
    // every (optional) entry/exit methods get the event passed.
    template <class Event,class FSM>
    void on_entry(Event const&,FSM& ) {std::cout << "entering: Empty" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Empty" << std::endl;}
    struct internal_guard_fct
    {
        template <class EVT,class FSM,class SourceState,class TargetState>
        bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
        {
            std::cout << "Empty::internal_transition_table guard\n";
            return false;
        }
    };
    struct internal_action_fct
    {
        template <class EVT,class FSM,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
        {
            std::cout << "Empty::internal_transition_table action" << std::endl;
        }
    };
    // Transition table for Empty
    struct internal_transition_table : mpl::vector<
        //    Start     Event         Next      Action               Guard
   Internal <           cd_detected           , internal_action_fct ,internal_guard_fct    >
        //  +---------+-------------+---------+---------------------+----------------------+
    > {};
};
struct Open : public msm::front::state<>
{
    template <class Event,class FSM>
    void on_entry(Event const& ,FSM&) {std::cout << "entering: Open" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Open" << std::endl;}
};

struct Playing : public msm::front::state<>
{
    template <class Event,class FSM>
    void on_entry(Event const&,FSM& ) {std::cout << "entering: Playing" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Playing" << std::endl;}
};

// state not defining any entry or exit
struct Paused : public msm::front::state<>
{
};


// transition actions
// as the functors are generic on events, fsm and source/target state,
// you can reuse them in another machine if you wish
struct TestFct
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const&, FSM&,SourceState& ,TargetState& )
    {
        cout << "transition with event:" << typeid(EVT).name() << endl;
    }
};
struct start_playback
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::start_playback" << endl;
    }
};
struct open_drawer
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::open_drawer" << endl;
    }
};
struct close_drawer
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::close_drawer" << endl;
    }
};
struct store_cd_info
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const&,FSM& fsm ,SourceState& ,TargetState& )
    {
        cout << "player::store_cd_info" << endl;
        fsm.process_event(play());
    }
};
struct stop_playback
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::stop_playback" << endl;
    }
};
struct pause_playback
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::pause_playback" << endl;
    }
};
struct resume_playback
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::resume_playback" << endl;
    }
};
struct stopped_again
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        cout << "player::stopped_again" << endl;
    }
};
// guard conditions
struct DummyGuard
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    bool operator()(EVT const& evt,FSM& fsm,SourceState& src,TargetState& tgt)
    {
        return true;
    }
};
struct good_disk_format
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
    {
        // to test a guard condition, let's say we understand only CDs, not DVD
        if (evt.disc_type != DISK_CD)
        {
            std::cout << "wrong disk, sorry" << std::endl;
            return false;
        }
        return true;
    }
};

struct always_true
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
    {
        return true;
    }
};

struct say_monitor
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        std::cout << "say monitor" << std::endl;
    }
};
struct say_stopped
{
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const& ,FSM& ,SourceState& ,TargetState& )
    {
        std::cout << "say stopped" << std::endl;
    }
};

// front-end: define the FSM structure
struct Player_ : public msm::front::state_machine_def<Player_>
{
    struct Stopped;

    struct Monitor : public msm::front::state<>
    {
        template<class E, class FSM> void on_entry(E, FSM&) { }
        template<class E, class FSM> void on_exit(E, FSM&) { }
    };

    using transition_table = mpl::vector<
        Row < Stopped , open_close  , Open    >,
        Row < Stopped , stop        , Stopped >,
        Row < Open    , open_close  , Empty   >
    >;

    using initial_state = mpl::vector<Stopped>;

    template <class FSM, class Event>
    void no_transition(const Event& e, FSM&, int state)
    {
        std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
    }
};
typedef msm::back::state_machine<Player_> Player;


struct Player_::Stopped : public msm::front::state<>
{
    template<class E> void on_entry(E, Player&) { }
    template<class E> void on_exit(E, Player&) { }

    void on_entry(stop, Player& p)
    {
    }

    void on_entry(cd_detected, Player& p)
    {
    }

    void on_exit(play, Player&)
    {
    }

    void on_exit(open_close, Player&)
    {
    }

    void on_exit(stop, Player&)
    {
    }
};


//
// Testing utilities.
//
static char const* const state_names[] = { "Stopped", "Open", "Empty", "Playing", "Paused" };
void pstate(Player const& p)
{
    std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
}

void test()
{
    Player p;
    p.start();
    p.process_event(open_close{});
    p.process_event(stop{});

}

int main()
{
    test();
    return 0;
}
