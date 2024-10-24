#include "Events.h"

namespace NB {

void NULL_FUNC() {}

std::invalid_argument null_mask_error("NULL MASK NOT ALLOWED");

Events::Events(const uint64_t initMask, void (*initFunc)(), const char* initName):
    mask(initMask), func(initFunc), name(initName) {
    if (mask == 0x0) {
        throw null_mask_error;
    }

}

Events::Events(const uint64_t initMask, const char* initName):
    Events(initMask, NULL_FUNC, initName) {}

Events::Events(const Events& cpy)
:Events(cpy.mask, cpy.func, cpy.name.c_str())  {}

Events& Events::operator=(const Events& cpy) {
    func =  cpy.func;
    name = cpy.name;
    mask = cpy.mask;
    if (mask==0x0) {
        throw null_mask_error;
    }
    return *this;
}

Events::~Events() { func = nullptr; }

const std::string Events::getName() const { return name; }

const uint64_t Events::getMask() const { return mask; }

void Events::setMask(const uint64_t newMask) {
    mask = newMask;
}

void Events::setName(const char* newName) {
    name = newName;
}

void Events::setFunc(void (*newFunc)()) {
    func = newFunc;
}

NBEventBasic* NBEventBasic::clone() const { return new NBEventBasic(*this); }

NBEventState* NBEventState::clone() const { return new NBEventState(*this); }

const uint64_t NBEventBasic::check(const uint64_t refState) const {
    if ((mask!=0) && ((refState&mask)==mask)) {
        func();
        return refState&(~mask);
    }
    return refState;
}

const uint64_t NBEventState::check(const uint64_t refState) const {
    if ((mask!=0) && ((refState&mask)==mask)) {
        func();
    }
    return refState;
}

NBEventListener::NBEventListener(Events** initEventList, uint16_t initNum, const uint64_t initState, state_register* initStatePtr) {
    if (initStatePtr == nullptr) {
        state = new state_register;
    } else {
        state = initStatePtr;
    }
    
    state->store(initState);
    numEvents = initNum;
    eventList = new Events*[numEvents];
    for (uint16_t i = 0; i < initNum; ++i) {
        eventList[i] = initEventList[i]->clone();
    }
}

state_register* NBEventListener::getStatePtr() const {
    return state;
}

const uint64_t NBEventListener::getState() const {
    return state->load();
}

void NBEventListener::raiseFlags(const uint64_t newState) {
    bufferLock.lock();
    stateBuffer.push(NBStateChange{STATE_RAISE, newState});
    bufferLock.unlock();
}

void NBEventListener::raiseFlags(const Events& newEvent) {
    raiseFlags(newEvent.mask);
}

void NBEventListener::dropFlags(const uint64_t dropState) {
    bufferLock.lock();
    stateBuffer.push(NBStateChange{STATE_DROP, dropState});
    bufferLock.unlock();
}

void NBEventListener::dropFlags(const Events& dropEvent) {
    dropFlags(dropEvent.mask);
}

const bool NBEventListener::snoop(const uint64_t refState) const {
    return ((getState()&refState)==refState);
}

const bool NBEventListener::snoop(const Events& refEvent) const {
    return snoop(refEvent.mask);
}

void NBEventListener::_setState(const uint64_t newState) {
    bufferLock.lock();
    std::queue<NBStateChange>().swap(stateBuffer);
    bufferLock.unlock();
    state->store(newState);
}

void NBEventListener::listen() {
    uint64_t oldState = getState();
    for (uint16_t i = 0; i < numEvents; ++i) {
        oldState = eventList[i]->check(oldState);
    }
    NBStateChange curr;
    bufferLock.lock();
    for (; !stateBuffer.empty(); stateBuffer.pop()) {
        curr = stateBuffer.front();
        switch (curr.type) {
        case STATE_RAISE:
            oldState |= curr.mask;
            break;
        case STATE_DROP:
            oldState &= ~curr.mask;
            break;
        case STATE_SET:
            oldState = curr.mask;
            break;
        default:
            break;
        }
    }
    bufferLock.unlock();
    _setState(oldState);
}

void NBEventListener::listen(const Events& refEvent) {
    uint64_t oldState = getState();
    oldState = refEvent.check(oldState);
    _setState(oldState);
}

};