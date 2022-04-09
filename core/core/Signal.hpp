#pragma once

#include <vector>
#include <functional>

template<class T>class Signal {
public:
    typedef unsigned int SlotId;
    typedef std::function<void(const T&)> Fn;

public:
    ~Signal() {
        _slots.clear();
    }

    SlotId connect(Fn listener) {
        static SlotId id = 1;

        Slot slot;
        slot.id = id++;
        slot.callback = listener;

        _slots.emplace_back(slot);
        return slot.id;
    }

    SlotId disconnect(SlotId id) {
        if (id == 0)
            return 0;

        for (auto it = _slots.begin(); it < _slots.end(); ++it) {
            if (it->id == id) {
                _slots.erase(it);
                break;
            }
        }
        return 0;
    }

    void emmit(const T& data) {
        for (Slot& slot : _slots)
            slot.callback(data);
    }

protected:
    struct Slot {
        SlotId id = 0;
        Fn callback;
    };

    std::vector<Slot> _slots;
};
