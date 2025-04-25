/**
 * @file    led_example.c
 * @brief   ʹ��HSM����LED��ʾ��Ӧ�ó���
 * @copyright  (c) 2024, Ҷ����, <y956370541@163.com>
 * @details 
 *          ��ʾ����ʾ�����ʹ�ò��ʽ״̬�����
 *          ʵ��LED��˸���ơ�
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "led_hsm.h"

/* LED״̬��ʵ�� */
static led_hsm_t led_hsm;

/* ����ģ�ⶨʱ�� */
static uint32_t current_time_ms = 0;
static uint32_t last_timer_ms = 0;

/* ģ���û�����״̬ */
static bool user_button_pressed = false;

/*
 * ģ��LED���ƺ���
 * ��ʵ��Ӧ���У���Щ����������Ӳ��GPIO����
 */
static void led_hardware_on(bool state) {
    printf("[%u ms] LED ��\n", current_time_ms);
}

static void led_hardware_off(bool state) {
    printf("[%u ms] LED �ر�\n", current_time_ms);
}

/* ģ���ȡ��ǰʱ�� */
static uint32_t get_time_ms(void) {
    /* ��ʵ��Ӧ���У�����ϵͳʱ�ӻ�ȡ */
    return current_time_ms;
}

/* ģ��ʱ���ƽ� */
static void advance_time(uint32_t ms) {
    current_time_ms += ms;
    printf("ʱ���ƽ�: %u ms\n", ms);
}

/* ģ���û����� */
static void check_user_input(void) {
    /* ģ��ÿ1000ms����һ���û����� */
    static uint32_t last_input_time = 0;
    
    if (current_time_ms - last_input_time >= 1000) {
        /* ��ʵ��Ӧ���У�����鰴��״̬ */
        printf("[%u ms] ��⵽�û�����\n", current_time_ms);
        user_button_pressed = true;
        last_input_time = current_time_ms;
    }
}

/*
 * ������
 */
int main(void) {
    /* ��ʼ��LED״̬�� */
    led_hsm_init(&led_hsm, led_hardware_on, led_hardware_off);
    
    printf("LED HSM ʾ������\n");
    printf("----------------------------\n");
    printf("��ʼ״̬: ����˸\n\n");
    
    /* ��ѭ�� */
    uint32_t simulation_end_time = 5000; /* ģ��5�� */
    printf("��ʼģ�� (5��)...\n\n");
    
    /* ģ���ʼ״̬ */
    for (int i = 0; i < 2; i++) {
        /* ģ���û������ڷ���˸״̬���л�LED */
        printf("[%u ms] �û��������� - �л�LED\n", current_time_ms);
        hsm_dispatch(&led_hsm.super, hsm_event_make(LED_USER_SIG, NULL));
        advance_time(200);
    }
    
    /* �л�����˸״̬ */
    printf("\n[%u ms] �л�����˸״̬\n", current_time_ms);
    hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TOGGLE_BLINK_SIG, NULL));
    advance_time(100);
    
    /* ��ѭ�� */
    while (current_time_ms < simulation_end_time) {
        /* ����Ƿ���Ҫ���Ͷ�ʱ���¼� */
        if (led_hsm.current_mode == LED_MODE_BLINKING && 
            current_time_ms - last_timer_ms >= led_hsm.blink_period_ms) {
            
            printf("[%u ms] ��ʱ���¼�����\n", current_time_ms);
            hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TIMER_SIG, NULL));
            last_timer_ms = current_time_ms;
        }
        
        /* ����û����� */
        check_user_input();
        if (user_button_pressed) {
            /* �л���˸/����˸״̬ */
            printf("[%u ms] �û��������� - �л���˸״̬\n", current_time_ms);
            hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TOGGLE_BLINK_SIG, NULL));
            user_button_pressed = false;
        }
        
        /* ģ��ʱ���ƽ� */
        advance_time(100);
    }
    
    printf("\nLED HSM ʾ�����\n");
    
    return 0;
} 