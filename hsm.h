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

/*状态机支持的最大嵌套层数*/
#define HSM_MAX_LEVEL 10

/*前置声明*/
typedef struct hsm hsm_t;
typedef struct hsm_obj hsm_obj_t;

/*状态机默认事件信号*/
enum{
	HSM_ENTRY_SIG = -5,  //进入事件信号，进入状态时执行触发一次,内部信号
	HSM_EXIT_SIG = -4,   //退出事件信号,退出状态时触发一次，内部信号
    HSM_INIT_SIG = -3,   //初始化事件，内部信号
    //
    HSM_CYCLE_SIG = -2,  //周期事件信号，需用户自行触发
    //
    HSM_DEF_SIG = -1,
    //用户自定义事件在外部定义，需大于HSM_DEF_SIG
};
typedef int HSM_SIG_T;



/*状态机事件结构体*/
typedef struct{
	HSM_SIG_T sig;//信号
	void *param;  //事件参数 
}hsm_event_t;


/*状态机事件处理函数返回值*/
typedef enum{
	HSM_RET_HANDLED,  //事件已处理，不再向上传播到父状态
	HSM_RET_UNHANDLED, //事件继续向上传播给父状态
	HSM_RET_TRAN,    //状态转换
}HSM_RET_T;


typedef struct hsm_obj{
    hsm_t *current;
    hsm_t *last;

    //状态转换回调函数，可用于调试信息打印
    void(*state_tran_cb)(hsm_t *source, hsm_t *target);
}hsm_obj_t;

/*状态函数类型*/
typedef HSM_RET_T(*state_func_t)(hsm_obj_t *me, hsm_event_t *e);

typedef struct hsm{
    const char *name;
    const struct hsm *parent;
    state_func_t state_func;
}hsm_t;


/*状态机事件处理结果*/
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


/*顶层状态声明*/
extern const hsm_t top_state;

void hsm_init(hsm_obj_t *me, hsm_t *init_state);
void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e);
bool hsm_is_child(const hsm_t *parent, const hsm_t *child);

#ifdef __cplusplus
}
#endif
#endif



