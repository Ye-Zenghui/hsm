/**
 * @file    led_example.c
 * @brief   使用HSM控制LED的示例应用程序
 * @copyright  (c) 2024, 叶增辉, <y956370541@163.com>
 * @details 
 *          本示例演示了如何使用层次式状态机框架
 *          实现LED闪烁控制。
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "led_hsm.h"

/* LED状态机实例 */
static led_hsm_t led_hsm;

/* 用于模拟定时器 */
static uint32_t current_time_ms = 0;
static uint32_t last_timer_ms = 0;

/* 模拟用户按键状态 */
static bool user_button_pressed = false;

/*
 * 模拟LED控制函数
 * 在实际应用中，这些函数将控制硬件GPIO引脚
 */
static void led_hardware_on(bool state) {
    printf("[%u ms] LED 打开\n", current_time_ms);
}

static void led_hardware_off(bool state) {
    printf("[%u ms] LED 关闭\n", current_time_ms);
}

/* 模拟获取当前时间 */
static uint32_t get_time_ms(void) {
    /* 在实际应用中，这会从系统时钟获取 */
    return current_time_ms;
}

/* 模拟时间推进 */
static void advance_time(uint32_t ms) {
    current_time_ms += ms;
    printf("时间推进: %u ms\n", ms);
}

/* 模拟用户输入 */
static void check_user_input(void) {
    /* 模拟每1000ms产生一次用户输入 */
    static uint32_t last_input_time = 0;
    
    if (current_time_ms - last_input_time >= 1000) {
        /* 在实际应用中，这会检查按键状态 */
        printf("[%u ms] 检测到用户按键\n", current_time_ms);
        user_button_pressed = true;
        last_input_time = current_time_ms;
    }
}

/*
 * 主函数
 */
int main(void) {
    /* 初始化LED状态机 */
    led_hsm_init(&led_hsm, led_hardware_on, led_hardware_off);
    
    printf("LED HSM 示例程序\n");
    printf("----------------------------\n");
    printf("初始状态: 非闪烁\n\n");
    
    /* 主循环 */
    uint32_t simulation_end_time = 5000; /* 模拟5秒 */
    printf("开始模拟 (5秒)...\n\n");
    
    /* 模拟初始状态 */
    for (int i = 0; i < 2; i++) {
        /* 模拟用户按键在非闪烁状态下切换LED */
        printf("[%u ms] 用户按键触发 - 切换LED\n", current_time_ms);
        hsm_dispatch(&led_hsm.super, hsm_event_make(LED_USER_SIG, NULL));
        advance_time(200);
    }
    
    /* 切换到闪烁状态 */
    printf("\n[%u ms] 切换到闪烁状态\n", current_time_ms);
    hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TOGGLE_BLINK_SIG, NULL));
    advance_time(100);
    
    /* 主循环 */
    while (current_time_ms < simulation_end_time) {
        /* 检查是否需要发送定时器事件 */
        if (led_hsm.current_mode == LED_MODE_BLINKING && 
            current_time_ms - last_timer_ms >= led_hsm.blink_period_ms) {
            
            printf("[%u ms] 定时器事件触发\n", current_time_ms);
            hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TIMER_SIG, NULL));
            last_timer_ms = current_time_ms;
        }
        
        /* 检查用户输入 */
        check_user_input();
        if (user_button_pressed) {
            /* 切换闪烁/非闪烁状态 */
            printf("[%u ms] 用户按键触发 - 切换闪烁状态\n", current_time_ms);
            hsm_dispatch(&led_hsm.super, hsm_event_make(LED_TOGGLE_BLINK_SIG, NULL));
            user_button_pressed = false;
        }
        
        /* 模拟时间推进 */
        advance_time(100);
    }
    
    printf("\nLED HSM 示例完成\n");
    
    return 0;
} 