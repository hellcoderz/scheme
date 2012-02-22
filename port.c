#include <stdio.h>
#include "object.h"
#include "mem.h"

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

void port_free(object *port) {
    FILE *stream;

    if (is_input_port(port)) {
        stream = obj_ipv(port);
    } else if (is_output_port(port)) {
        stream = obj_opv(port);
    } else {
        return;
    }

    if (stream != NULL) {
        fclose(stream);
    }
}

