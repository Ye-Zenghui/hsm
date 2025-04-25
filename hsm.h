/**
 * @file    hsm.h
 * @brief   Hierarchical state machines for designing event-driven systems.
 * @copyright  (c) 2024, yezenghui, <y956370541@163.com>
 * @details 
 * @par Encoding:
 *          GBK
 * @par Modification:
 * -# Date    -#Author     -#details
 * 20241113   yezenghui     create
 */

#ifndef __HSM_H__
#define __HSM_H__
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#ifdef __cplusplus
extern "C" {
#endif

/*״̬��֧�ֵ����Ƕ�ײ���*/
#define HSM_MAX_LEVEL 10

/*ǰ������*/
typedef struct hsm hsm_t;
typedef struct hsm_obj hsm_obj_t;

/*״̬��Ĭ���¼��ź�*/
enum{
	HSM_ENTRY_SIG = -5,  //�����¼��źţ�����״̬ʱִ�д���һ��,�ڲ��ź�
	HSM_EXIT_SIG = -4,   //�˳��¼��ź�,�˳�״̬ʱ����һ�Σ��ڲ��ź�
    HSM_INIT_SIG = -3,   //��ʼ���¼����ڲ��ź�
    //
    HSM_CYCLE_SIG = -2,  //�����¼��źţ����û����д���
    //
    HSM_DEF_SIG = -1,
    //�û��Զ����¼����ⲿ���壬�����HSM_DEF_SIG
};
typedef int HSM_SIG_T;



/*״̬���¼��ṹ��*/
typedef struct{
	HSM_SIG_T sig;//�ź�
	void *param;  //�¼����� 
}hsm_event_t;


/*״̬���¼�����������ֵ*/
typedef enum{
	HSM_RET_HANDLED,  //�¼��Ѵ����������ϴ�������״̬
	HSM_RET_UNHANDLED, //�¼��������ϴ�������״̬
	HSM_RET_TRAN,    //״̬ת��
}HSM_RET_T;


typedef struct hsm_obj{
    hsm_t *current;
    hsm_t *last;

    //״̬ת���ص������������ڵ�����Ϣ��ӡ
    void(*state_tran_cb)(hsm_t *source, hsm_t *target);
}hsm_obj_t;

/*״̬��������*/
typedef HSM_RET_T(*state_func_t)(hsm_obj_t *me, hsm_event_t *e);

typedef struct hsm{
    const char *name;
    const struct hsm *parent;
    state_func_t state_func;
}hsm_t;


/*״̬���¼�������*/
#define HSM_HANDLED() (HSM_RET_HANDLED)
#define HSM_UNHANDLED() (HSM_RET_UNHANDLED)
#define HSM_TRAN(target) (((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, ((hsm_obj_t*)(me))->current = target, HSM_RET_TRAN)

#define HSM_STATE_FUNC(func) (state_func_t)(func)

#define HSM_ASSERT(cond) 	do{ if(!(cond)){ while(1); } }while(0)

#if   defined ( __CC_ARM )
	#define __STATIC_INLINE static __inline
#else
	#define __STATIC_INLINE static inline
#endif

__STATIC_INLINE hsm_event_t* hsm_event_make(HSM_SIG_T sig, void *param){ 
    static hsm_event_t _event = {0, NULL};
    _event.sig = sig;
    _event.param = param;
	return &_event;
}


/*����״̬����*/
extern const hsm_t top_state;

void hsm_init(hsm_obj_t *me, hsm_t *init_state);
void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e);
bool hsm_is_child(const hsm_t *parent, const hsm_t *child);

#ifdef __cplusplus
}
#endif
#endif



