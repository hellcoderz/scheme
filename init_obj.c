#include "object.h"
#include "sform.h"

#define CALL_WITH_CHECK(fn) \
    ret = fn(); \
    if (ret != 0) { \
        return ret; \
    } 
   
int init_obj(void) {
    int ret;

    CALL_WITH_CHECK(boolean_init);
    CALL_WITH_CHECK(eof_init);
    CALL_WITH_CHECK(empty_list_init);
    CALL_WITH_CHECK(symbol_init);
    CALL_WITH_CHECK(string_init);
    CALL_WITH_CHECK(sform_init);
    
    return 0;
}

void dispose_obj(void) {
    symbol_dispose();
    string_dispose();
    boolean_dispose();
    empty_list_dispose();
    eof_dispose();
}

