#include "utils.h"

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>

Error_Block make_error_block(void) {
    Error_Block eb;
    Error_init(&eb);

    return eb;
}

Event_Handle event_new(Error_Block *const error) {
    Event_Params params;
    Event_Params_init(&params);

    Event_Handle event = Event_create(&params, error);

    if (!event) {
        System_abort("event_new: unable to create Event\n");
    }

    return event;
}

Task_Handle task_new(const Task_FuncPtr function, void *const first_arg,
                      void *const second_arg, Error_Block *const error) {
    Task_Params params;
    Task_Params_init(&params);

    params.arg0 = (UArg) first_arg;
    params.arg1 = (UArg) second_arg;

    Task_Handle task = Task_create(function, &params, error);

    if (!task) {
        System_abort("task_new: unable to create Task_Handle\n");
    }

    return task;
}
