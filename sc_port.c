#include "sc_object.h"
#include "sc_mem.h"

object* make_input_port(FILE *stream) {
    object *obj;

    obj = alloc_object();
    type(obj) = INPUT_PORT;
    obj_ipv(obj) = stream;
    return obj;
}

int is_input_port(object *obj) {
    return obj != NULL && type(obj) == INPUT_PORT;
}

object* make_output_port(FILE *stream) {
    object *obj;

    obj = alloc_object();
    type(obj) = OUTPUT_PORT;
    obj_opv(obj) = stream;
    return obj;
}

int is_output_port(object *obj) {
    return obj != NULL && type(obj) == OUTPUT_PORT;
}

