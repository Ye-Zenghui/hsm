/**
 * @file    led_hsm.c
 * @brief   使用层次式状态机控制LED的示例
 * @copyright  (c) 2024, 叶增辉, <y956370541@163.com>
 * @details 
 *          本示例演示了如何使用层次式状态机框架
 *          实现LED闪烁控制。
 */

#include "led_hsm.h"

/*------------------------------- 状态函数声明 ---------------------------------*/
HSM_RET_T led_blinking_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_not_blinking_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_on_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_off_state_func(led_hsm_t *me, hsm_event_t *e);

/*------------------------------- 状态定义 ------------------------------*/
/* 闪烁状态 - 顶层状态的子状态 */
hsm_t led_blinking_state = {
    .name = "blinking",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(led_blinking_state_func)
};

/* 非闪烁状态 - 顶层状态的子状态 */
hsm_t led_not_blinking_state = {
    .name = "not_blinking",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(led_not_blinking_state_func)
};

/* LED开状态 - 闪烁状态的子状态 */
hsm_t led_on_state = {
    .name = "led_on",
    .parent = &led_blinking_state,
    .state_func = HSM_STATE_FUNC(led_on_state_func)
};

/* LED关状态 - 闪烁状态的子状态 */
hsm_t led_off_state = {
    .name = "led_off",
    .parent = &led_blinking_state,
    .state_func = HSM_STATE_FUNC(led_off_state_func)
};

/* 状态转换回调函数（用于调试） */
static void led_state_tran_callback(hsm_t *source, hsm_t *target) {
    /* 可以在此添加调试输出 */
    (void)source;
    (void)target;
}

/*------------------------------- 状态实现 --------------------------*/

/* 闪烁状态实现 */
HSM_RET_T led_blinking_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* 进入闪烁状态 */
            me->current_mode = LED_MODE_BLINKING;
            return HSM_HANDLED();
            
        case HSM_INIT_SIG:
            /* 初始化为开启子状态 */
            return HSM_TRAN(&led_on_state);
            
        case HSM_EXIT_SIG:
            /* 退出闪烁状态 */
            return HSM_HANDLED();
            
        case LED_TOGGLE_BLINK_SIG:
            /* 收到切换信号，转换到非闪烁状态 */
            return HSM_TRAN(&led_not_blinking_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/* 非闪烁状态实现 */
HSM_RET_T led_not_blinking_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* 进入非闪烁状态 */
            me->current_mode = LED_MODE_NOT_BLINKING;
            me->led_off(false);  /* 默认关闭LED */
            me->led_state = false;
            return HSM_HANDLED();
            
        case LED_TOGGLE_BLINK_SIG:
            /* 收到切换信号，转换到闪烁状态 */
            return HSM_TRAN(&led_blinking_state);
            
        case LED_USER_SIG:
            /* 用户事件可以在非闪烁状态下操作LED */
            me->led_state = !me->led_state;
            if (me->led_state) {
                me->led_on(true);
            } else {
                me->led_off(false);
            }
            return HSM_HANDLED();
            
        default:
            return HSM_UNHANDLED();
    }
}

/* LED开子状态实现 */
HSM_RET_T led_on_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* 进入LED开状态 */
            me->led_on(true);
            me->led_state = true;
            return HSM_HANDLED();
            
        case LED_TIMER_SIG:
            /* 定时器到期，切换到LED关状态 */
            return HSM_TRAN(&led_off_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/* LED关子状态实现 */
HSM_RET_T led_off_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* 进入LED关状态 */
            me->led_off(false);
            me->led_state = false;
            return HSM_HANDLED();
            
        case LED_TIMER_SIG:
            /* 定时器到期，切换到LED开状态 */
            return HSM_TRAN(&led_on_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/*------------------------------- 公共函数 ------------------------------*/

/* LED HSM初始化 */
void led_hsm_init(led_hsm_t *me, led_control_func_t led_on, led_control_func_t led_off) {
    /* 存储LED控制函数 */
    me->led_on = led_on;
    me->led_off = led_off;
    
    /* 初始化默认值 */
    me->current_mode = LED_MODE_NOT_BLINKING;
    me->led_state = false;
    me->blink_period_ms = 500; /* 默认500ms闪烁周期 */
    
    /* 设置状态转换回调 */
    me->super.state_tran_cb = led_state_tran_callback;
    
    /* 以非闪烁状态初始化状态机 */
    hsm_init(&me->super, &led_not_blinking_state);
} 