#include "sc_object.h"
#include "sc_sform.h"

int init_obj(void) {
    int ret;

    ret = boolean_init();
    if (ret != 0) {
        return ret;
    }

    ret = empty_list_init();
    if (ret != 0) {
        return ret;
    }

    ret = symbol_init();
    if (ret != 0) {
        return ret;
    }

    ret = sform_init();
    return ret;
}

