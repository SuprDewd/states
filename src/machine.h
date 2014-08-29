#ifndef MACHINE_H
#define MACHINE_H

#include <string>

enum machine_type {
    MACHINE_NFA,
    MACHINE_DFA,
    MACHINE_REGEX,
};

class machine {
public:

    virtual machine_type get_machine_type() = 0;
    virtual bool accepts(const std::string &s) = 0;
};

#endif
