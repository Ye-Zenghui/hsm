/**
 * @file    led_hsm.h
 * @brief   使用层次式状态机控制LED的示例
 * @copyright  (c) 2024, 叶增辉, <y956370541@163.com>
 * @details 
 *          本示例演示了如何使用层次式状态机框架
 *          实现LED闪烁控制。
 */

#ifndef __LED_HSM_H__
#define __LED_HSM_H__

#include <stdint.h>
#include <stdbool.h>
#include "hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LED控制的用户自定义信号 */
enum {
    LED_TOGGLE_BLINK_SIG = HSM_DEF_SIG + 1,  // LED切换闪烁状态信号
    LED_TIMER_SIG,                          // 定时器事件信号
    LED_USER_SIG,                           // 用户事件信号
};

/* LED运行模式 */
typedef enum {
    LED_MODE_BLINKING,     // LED闪烁模式
    LED_MODE_NOT_BLINKING, // LED非闪烁模式
} led_mode_t;

/* LED硬件控制函数原型 */
typedef void (*led_control_func_t)(bool state);

/* LED状态机对象 */
typedef struct {
    hsm_obj_t super;            // 基础状态机对象（必须放在第一位）
    led_mode_t current_mode;    // 当前LED模式
    led_control_func_t led_on;  // 打开LED的函数
    led_control_func_t led_off; // 关闭LED的函数
    bool led_state;             // 当前LED状态（开/关）
    uint32_t blink_period_ms;   // 闪烁周期（毫秒）
} led_hsm_t;

/* 外部状态声明 */
extern hsm_t led_blinking_state;      // 闪烁状态
extern hsm_t led_not_blinking_state;  // 非闪烁状态
extern hsm_t led_on_state;            // LED开子状态
extern hsm_t led_off_state;           // LED关子状态

/* LED HSM初始化 */
void led_hsm_init(led_hsm_t *me, led_control_func_t led_on, led_control_func_t led_off);

#ifdef __cplusplus
}
#endif

#endif /* __LED_HSM_H__ */ 