#include "sc_object.h"

int init_obj(void) {
    int ret;

    ret = boolean_init();
    if (ret != 0) {
        return ret;
    }

    ret = empty_list_init();
    return ret;
}

