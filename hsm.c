/**
 * @file    hsm.c
 * @brief   Hierarchical state machines for designing event-driven systems.
 * @copyright  (c) 2024, yezenghui, <y956370541@163.com>
 * @encoding:  GBK
 * @modification:
 * -# Date    -#Author     -#details
 * 20241113   yezenghui     create
 */

#include "hsm.h"



static HSM_RET_T top_state_func(hsm_obj_t *me, hsm_event_t *e){
    (void)me;
    (void)e;
    return HSM_RET_HANDLED;
}

const hsm_t top_state = {
    .name = "top_state",
    .parent = NULL,
    .state_func = top_state_func
};


static void hsm_tran(hsm_obj_t *me){

    int idx = 0;
    state_func_t func[HSM_MAX_LEVEL] = {NULL};
    const hsm_t *source = me->last;
    const hsm_t *target = me->current;

    if(me->state_tran_cb){
        me->state_tran_cb(me->last, me->current);
    }

    while (source != &top_state && !hsm_is_child(source, target) && target != source){

        source->state_func(me, hsm_event_make(HSM_EXIT_SIG, NULL));

        source = source->parent;
    }

    while (target != source){

        HSM_ASSERT(idx < HSM_MAX_LEVEL);

        func[idx++] = target->state_func;

        target = target->parent;
    }       
    
    while (idx > 0) {
        func[--idx](me, hsm_event_make(HSM_ENTRY_SIG, NULL));
    }

    if(func[0]){
        if(HSM_RET_TRAN == func[0](me, hsm_event_make(HSM_INIT_SIG, NULL))){
            hsm_tran(me);
        }
    }
}

void hsm_init(hsm_obj_t *me, hsm_t *init_state) {

    state_func_t func[HSM_MAX_LEVEL];
    const hsm_t *h = init_state;
    int idx = 0;

    me->current = init_state;
    me->last = init_state;

    for (; h != NULL; h = h->parent) {
        HSM_ASSERT(idx < HSM_MAX_LEVEL);
        func[idx++] = h->state_func;
    }

    while (idx > 0) {
        func[--idx](me, hsm_event_make(HSM_ENTRY_SIG, NULL));
    }

    if(HSM_RET_TRAN == me->current->state_func(me, hsm_event_make(HSM_INIT_SIG, NULL))){
        hsm_tran(me);
    }
}

bool hsm_is_child(const hsm_t *parent, const hsm_t *child) {
    const hsm_t *h = child->parent;
    for (; h != NULL; h = h->parent) {
        if(h == parent) {
            return true;
        }
    }

    return false;
}

void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e){
    HSM_RET_T ret;
    const hsm_t *h = me->current;

    if(!e){
        return;
    }

    do{
        ret = h->state_func(me, e);
        h = h->parent;
    }while (ret == HSM_RET_UNHANDLED);
    
    if(ret == HSM_RET_TRAN) {
        hsm_tran(me);
    }
}





