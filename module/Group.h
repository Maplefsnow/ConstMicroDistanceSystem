#pragma once

#include "Axis.h"


class Group {
public:
    Group();
    Group(Axis* masterAxis, Axis* slaveAxis);
    ~Group();

private:
    HAND hand;

};

