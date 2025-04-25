/**
 * @file    led_hsm.c
 * @brief   ʹ�ò��ʽ״̬������LED��ʾ��
 * @copyright  (c) 2024, Ҷ����, <y956370541@163.com>
 * @details 
 *          ��ʾ����ʾ�����ʹ�ò��ʽ״̬�����
 *          ʵ��LED��˸���ơ�
 */

#include "led_hsm.h"

/*------------------------------- ״̬�������� ---------------------------------*/
HSM_RET_T led_blinking_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_not_blinking_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_on_state_func(led_hsm_t *me, hsm_event_t *e);
HSM_RET_T led_off_state_func(led_hsm_t *me, hsm_event_t *e);

/*------------------------------- ״̬���� ------------------------------*/
/* ��˸״̬ - ����״̬����״̬ */
hsm_t led_blinking_state = {
    .name = "blinking",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(led_blinking_state_func)
};

/* ����˸״̬ - ����״̬����״̬ */
hsm_t led_not_blinking_state = {
    .name = "not_blinking",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(led_not_blinking_state_func)
};

/* LED��״̬ - ��˸״̬����״̬ */
hsm_t led_on_state = {
    .name = "led_on",
    .parent = &led_blinking_state,
    .state_func = HSM_STATE_FUNC(led_on_state_func)
};

/* LED��״̬ - ��˸״̬����״̬ */
hsm_t led_off_state = {
    .name = "led_off",
    .parent = &led_blinking_state,
    .state_func = HSM_STATE_FUNC(led_off_state_func)
};

/* ״̬ת���ص����������ڵ��ԣ� */
static void led_state_tran_callback(hsm_t *source, hsm_t *target) {
    /* �����ڴ���ӵ������ */
    (void)source;
    (void)target;
}

/*------------------------------- ״̬ʵ�� --------------------------*/

/* ��˸״̬ʵ�� */
HSM_RET_T led_blinking_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* ������˸״̬ */
            me->current_mode = LED_MODE_BLINKING;
            return HSM_HANDLED();
            
        case HSM_INIT_SIG:
            /* ��ʼ��Ϊ������״̬ */
            return HSM_TRAN(&led_on_state);
            
        case HSM_EXIT_SIG:
            /* �˳���˸״̬ */
            return HSM_HANDLED();
            
        case LED_TOGGLE_BLINK_SIG:
            /* �յ��л��źţ�ת��������˸״̬ */
            return HSM_TRAN(&led_not_blinking_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/* ����˸״̬ʵ�� */
HSM_RET_T led_not_blinking_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* �������˸״̬ */
            me->current_mode = LED_MODE_NOT_BLINKING;
            me->led_off(false);  /* Ĭ�Ϲر�LED */
            me->led_state = false;
            return HSM_HANDLED();
            
        case LED_TOGGLE_BLINK_SIG:
            /* �յ��л��źţ�ת������˸״̬ */
            return HSM_TRAN(&led_blinking_state);
            
        case LED_USER_SIG:
            /* �û��¼������ڷ���˸״̬�²���LED */
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

/* LED����״̬ʵ�� */
HSM_RET_T led_on_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* ����LED��״̬ */
            me->led_on(true);
            me->led_state = true;
            return HSM_HANDLED();
            
        case LED_TIMER_SIG:
            /* ��ʱ�����ڣ��л���LED��״̬ */
            return HSM_TRAN(&led_off_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/* LED����״̬ʵ�� */
HSM_RET_T led_off_state_func(led_hsm_t *me, hsm_event_t *e) {
    switch (e->sig) {
        case HSM_ENTRY_SIG:
            /* ����LED��״̬ */
            me->led_off(false);
            me->led_state = false;
            return HSM_HANDLED();
            
        case LED_TIMER_SIG:
            /* ��ʱ�����ڣ��л���LED��״̬ */
            return HSM_TRAN(&led_on_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

/*------------------------------- �������� ------------------------------*/

/* LED HSM��ʼ�� */
void led_hsm_init(led_hsm_t *me, led_control_func_t led_on, led_control_func_t led_off) {
    /* �洢LED���ƺ��� */
    me->led_on = led_on;
    me->led_off = led_off;
    
    /* ��ʼ��Ĭ��ֵ */
    me->current_mode = LED_MODE_NOT_BLINKING;
    me->led_state = false;
    me->blink_period_ms = 500; /* Ĭ��500ms��˸���� */
    
    /* ����״̬ת���ص� */
    me->super.state_tran_cb = led_state_tran_callback;
    
    /* �Է���˸״̬��ʼ��״̬�� */
    hsm_init(&me->super, &led_not_blinking_state);
} 