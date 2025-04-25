/**
 * @file    led_hsm.h
 * @brief   ʹ�ò��ʽ״̬������LED��ʾ��
 * @copyright  (c) 2024, Ҷ����, <y956370541@163.com>
 * @details 
 *          ��ʾ����ʾ�����ʹ�ò��ʽ״̬�����
 *          ʵ��LED��˸���ơ�
 */

#ifndef __LED_HSM_H__
#define __LED_HSM_H__

#include <stdint.h>
#include <stdbool.h>
#include "hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LED���Ƶ��û��Զ����ź� */
enum {
    LED_TOGGLE_BLINK_SIG = HSM_DEF_SIG + 1,  // LED�л���˸״̬�ź�
    LED_TIMER_SIG,                          // ��ʱ���¼��ź�
    LED_USER_SIG,                           // �û��¼��ź�
};

/* LED����ģʽ */
typedef enum {
    LED_MODE_BLINKING,     // LED��˸ģʽ
    LED_MODE_NOT_BLINKING, // LED����˸ģʽ
} led_mode_t;

/* LEDӲ�����ƺ���ԭ�� */
typedef void (*led_control_func_t)(bool state);

/* LED״̬������ */
typedef struct {
    hsm_obj_t super;            // ����״̬�����󣨱�����ڵ�һλ��
    led_mode_t current_mode;    // ��ǰLEDģʽ
    led_control_func_t led_on;  // ��LED�ĺ���
    led_control_func_t led_off; // �ر�LED�ĺ���
    bool led_state;             // ��ǰLED״̬����/�أ�
    uint32_t blink_period_ms;   // ��˸���ڣ����룩
} led_hsm_t;

/* �ⲿ״̬���� */
extern hsm_t led_blinking_state;      // ��˸״̬
extern hsm_t led_not_blinking_state;  // ����˸״̬
extern hsm_t led_on_state;            // LED����״̬
extern hsm_t led_off_state;           // LED����״̬

/* LED HSM��ʼ�� */
void led_hsm_init(led_hsm_t *me, led_control_func_t led_on, led_control_func_t led_off);

#ifdef __cplusplus
}
#endif

#endif /* __LED_HSM_H__ */ 