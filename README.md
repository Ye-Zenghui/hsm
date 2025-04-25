# HSM - 层次式状态机框架

HSM 是一个轻量级的、事件驱动型层次式状态机框架，专为嵌入式系统设计，借鉴了 QP（Quantum Platform）框架但进行了源代码无限制优化，可用于支持嵌入式状态机、状态转换、进入/退出动作，以及简洁的 C 语言 API 和事件处理。

## 目录结构与组成

这是一个**非常精简**的状态机实现，整个模块仅由**一个 .c 文件**和**一个 .h 文件**组成：

- `hsm.h` - 包含所有公共API和数据结构定义
- `hsm.c` - 包含状态机引擎实现

没有复杂的依赖关系，无需编译系统支持，可以集成到任何嵌入式项目中。模块简洁使用非常方便，简单使用，同时具备强大的功能。

## 特性

- 轻量级实现，内存占用小
- 支持层次化状态嵌套的复杂关系
- 事件驱动型架构
- 支持状态进入/退出动作
- 支持动态状态转换
- 支持初始状态配置
- 可配置的最大嵌套层次（默认：10层）
- 无动态内存分配（适合嵌入式系统）
- 简洁的 C 语言 API，兼容 C++。

## 状态机架构

HSM 框架采用层次化树形结构的状态设计，如图所示：

```
┌───────────────────────────────────────┐
│               顶层状态                 │
│                                       │
│  ┌───────────────────────────────┐    │
│  │             子状态             │    │
│  │                               │    │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  │
│  │  │ 状态 A  │  │ 状态 B  │  │ 状态 C  │  │
│  │  └─────────┘  └─────────┘  └─────────┘  │
│  │                               │    │
│  └───────────────────────────────┘    │
│                                       │
└───────────────────────────────────────┘
```

此架构允许：
- 子状态继承父状态行为
- 父状态未处理事件将事件向上传递给父状态
- 系统行为以模块化方式进行组织

## 在RT-Thread中使用

### 安装软件包

1. 在 RT-Thread 的包管理器中选择这个软件包

```
RT-Thread online packages
    miscellaneous packages --->
        [*] HSM: Hierarchical state machines for embedded systems --->
            [ ]   Enable HSM examples
                  Maximum nesting level for state hierarchy (10) --->
                  Version (latest) --->
```

2. 使用 `pkgs --update` 命令下载软件包

### 基本使用流程

1. 定义状态函数和状态对象
2. 创建状态机对象
3. 初始化状态机
4. 分发事件到状态机

### 代码示例

```c
#include "hsm.h"

// 定义信号
enum {
    SIG_BUTTON = HSM_DEF_SIG + 1,
};

// 定义状态函数原型
static HSM_RET_T off_state_handler(hsm_obj_t *me, hsm_event_t *e);
static HSM_RET_T on_state_handler(hsm_obj_t *me, hsm_event_t *e);

// 定义状态对象
static const hsm_t off_state = {
    .name = "off",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(off_state_handler)
};

static const hsm_t on_state = {
    .name = "on",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(on_state_handler)
};

// 实现状态函数
static HSM_RET_T off_state_handler(hsm_obj_t *me, hsm_event_t *e)
{
    switch(e->sig)
    {
        case HSM_ENTRY_SIG:
            // 关闭灯
            printf("Light turned OFF\n");
            return HSM_HANDLED();
            
        case SIG_BUTTON:
            // 按钮按下，切换到开灯状态
            return HSM_TRAN(&on_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

static HSM_RET_T on_state_handler(hsm_obj_t *me, hsm_event_t *e)
{
    switch(e->sig)
    {
        case HSM_ENTRY_SIG:
            // 打开灯
            printf("Light turned ON\n");
            return HSM_HANDLED();
            
        case SIG_BUTTON:
            // 按钮按下，切换到关灯状态
            return HSM_TRAN(&off_state);
            
        default:
            return HSM_UNHANDLED();
    }
}

// 使用状态机
void example(void)
{
    hsm_obj_t hsm_obj;
    
    // 初始化状态机，从关灯状态开始
    hsm_init(&hsm_obj, (hsm_t *)&off_state);
    
    // 模拟按钮事件
    hsm_dispatch(&hsm_obj, hsm_event_make(SIG_BUTTON, NULL));
    // 现在应该是开灯状态
    
    // 再次模拟按钮事件
    hsm_dispatch(&hsm_obj, hsm_event_make(SIG_BUTTON, NULL));
    // 现在应该回到关灯状态
}
```

## API 参考

### 核心结构

```c
// 状态机结构
typedef struct hsm {
    const char *name;
    const struct hsm *parent;
    state_func_t state_func;
} hsm_t;

// 状态机对象
typedef struct hsm_obj {
    hsm_t *current;
    hsm_t *last;
    void(*state_tran_cb)(hsm_t *source, hsm_t *target);
} hsm_obj_t;

// 状态机事件
typedef struct {
    HSM_SIG_T sig;  // 信号
    void *param;    // 事件参数
} hsm_event_t;
```

### 事件信号

HSM 提供了几种内置信号：

```c
// 内置事件信号
enum {
    HSM_ENTRY_SIG = -5,  // 进入事件信号，内部用
    HSM_EXIT_SIG = -4,   // 退出事件信号，内部用
    HSM_INIT_SIG = -3,   // 初始化事件，内部用
    HSM_CYCLE_SIG = -2,  // 周期事件信号，用户可用于周期
    HSM_DEF_SIG = -1,    // 默认信号
    // 用户自定义信号应大于 HSM_DEF_SIG
};
```

### 核心函数

```c
// 使用初始状态初始化状态机
void hsm_init(hsm_obj_t *me, hsm_t *init_state);

// 分发事件到状态机
void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e);

// 检测一个状态是否是另一个状态的子状态
bool hsm_is_child(const hsm_t *parent, const hsm_t *child);

// 创建带信号和参数的事件
hsm_event_t* hsm_event_make(HSM_SIG_T sig, void *param);
```

### 返回值

```c
typedef enum {
    HSM_RET_HANDLED,   // 事件已处理，无需再处理
    HSM_RET_UNHANDLED, // 事件未处理，交由父级状态处理
    HSM_RET_TRAN,      // 状态转换请求
} HSM_RET_T;
```

### 宏定义

```c
// 返回事件已处理
#define HSM_HANDLED() (HSM_RET_HANDLED)

// 返回事件未处理，交由父级状态处理
#define HSM_UNHANDLED() (HSM_RET_UNHANDLED)

// 转向目标状态
#define HSM_TRAN(target) (((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, \
                          ((hsm_obj_t*)(me))->current = target, HSM_RET_TRAN)

// 状态函数类型转换
#define HSM_STATE_FUNC(func) (state_func_t)(func)
```

## 完整示例

请查看 [examples/led](./examples/led) 目录，查看使用 HSM 的完整 LED 控制示例。

## 许可证

版权所有 (c) 2024, 叶增辉, <y956370541@163.com>

MIT开源许可证 