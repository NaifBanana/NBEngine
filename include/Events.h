#pragma once
#ifndef _NB_EVENTS
#define _NB_EVENTS

#include <atomic>
#include <stdexcept>
#include <queue>
#include <array>
#include <memory>
#include <mutex>
#include <string>

using state_register = std::atomic<uint64_t>;

namespace NB { 

extern std::invalid_argument null_mask_error;

void NULL_FUNC();

class Events {
friend class NBEventListener;
public:
    Events(const uint64_t, void (*initFunc)() = NULL_FUNC, const char* initName="");
    Events(const uint64_t, const char* initName="");
    Events(const Events&);
    Events& operator=(const Events&);
    ~Events();

    const std::string getName() const;
    const uint64_t getMask() const;
    void setMask(const uint64_t);
    void setName(const char*);
    void setFunc(void (*newFunc)());
    virtual const uint64_t check(const uint64_t) const = 0;
    virtual Events* clone() const = 0;

protected:
    uint64_t mask = 0x0;
    void (*func)() = nullptr;
    std::string name = "";

};

class NBEventBasic : public Events {
friend class NBEventListener;
public:
    using Events::Events;

    NBEventBasic* clone() const override;
    const uint64_t check(const uint64_t) const override;

};

class NBEventState : public Events {
friend class NBEventListener;
public:
    using Events::Events;

    NBEventState* clone() const override;
    const uint64_t check(const uint64_t) const override;
};

enum NBStateChangeType : uint8_t {
    STATE_RAISE, STATE_DROP, STATE_SET
};
 
struct NBStateChange {
    uint8_t type;
    uint64_t mask;
};

class NBEventListener {
public:
    NBEventListener(Events**, uint16_t, const uint64_t initState=(uint64_t)0x0, state_register* initStatePtr=nullptr);
    template<size_t SIZE>
    NBEventListener(std::array<Events*, SIZE> eventArray, const uint64_t initState=(uint64_t)0x0, state_register* initStatePtr=nullptr)
        : NBEventListener(eventArray.data(), eventArray.size(), initState, initStatePtr) {}

    state_register* getStatePtr() const;
    const uint64_t getState() const;
    void raiseFlags(const uint64_t);
    void raiseFlags(const Events&);
    void dropFlags(const uint64_t);
    void dropFlags(const Events&);
    const bool snoop(const uint64_t) const;
    const bool snoop(const Events&) const;
    void listen();
    void listen(const Events&);

protected:
    void _setState(const uint64_t);
    
    Events** eventList;
    uint16_t numEvents;
    state_register* state;
    std::mutex bufferLock;
    std::queue<NBStateChange> stateBuffer;

};

};
#endif