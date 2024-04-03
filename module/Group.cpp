#include "Group.h"

#define throwError(x) \
	{\
		ULONG ret = x;\
		if (ret != 0) {\
			char errorMsg[100];\
			Acm_GetErrorMessage(ret, errorMsg, 100);\
			ret = 0;\
			throw errorMsg;\
		}\
	}


Group::Group() {

}

Group::Group(Axis *masterAxis, Axis *slaveAxis) {
    throwError(Acm_GpAddAxis(&this->hand, masterAxis->getHand()));
    throwError(Acm_GpAddAxis(&this->hand, slaveAxis->getHand()));
}

Group::~Group() {
}
