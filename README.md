# HSM - 层次式状态机框架

HSM 是一个轻量级的、事件驱动型层次式状态机框架，专为嵌入式系统设计，类似于 QP（Quantum Platform）但针对资源受限环境进行了优化。该框架支持嵌套状态、状态转换、进入/退出动作以及基于简洁 C 语言 API 的事件处理。

## 极致轻量化设计

这是一个**极其轻量级**的状态机实现，整个模块仅由**一个 .c 文件**和**一个 .h 文件**组成：

- `hsm.h` - 包含所有公共API和数据结构定义
- `hsm.c` - 包含状态机核心实现

没有复杂的依赖关系，无需额外的库支持，易于集成到任何嵌入式项目中。模块的简单性使其非常容易理解和使用，同时保持了强大的功能。

## 特性

- 轻量级实现，内存占用小
- 支持层次化状态嵌套的父子关系
- 事件驱动型架构
- 支持状态进入/退出动作
- 易于定义的状态转换
- 支持初始状态动作
- 可配置的最大嵌套层次（默认：10层）
- 无动态内存分配（适合嵌入式系统）
- 简洁的 C 语言 API（无需 C++）

## 状态机架构

HSM 框架采用层次化方法管理状态，如下图所示：

```
┌────────────────────────────────────────────┐
│               顶层状态                      │
│                                            │
│  ┌────────────────────────────────────┐    │
│  │             父状态                  │    │
│  │                                    │    │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  │
│  │  │ 状态 A  │  │ 状态 B  │  │ 状态 C  │  │
│  │  └─────────┘  └─────────┘  └─────────┘  │
│  │                                    │    │
│  └────────────────────────────────────┘    │
│                                            │
└────────────────────────────────────────────┘
```

此架构允许：
- 子状态继承父状态的行为
- 若子状态未处理事件，则事件会向上传递给父状态
- 系统行为以模块化方式清晰组织

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

HSM 提供以下内置信号：

```c
// 内置事件信号
enum {
    HSM_ENTRY_SIG = -5,  // 进入事件信号（内部）
    HSM_EXIT_SIG = -4,   // 退出事件信号（内部）
    HSM_INIT_SIG = -3,   // 初始化事件（内部）
    HSM_CYCLE_SIG = -2,  // 周期事件信号（用户触发）
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

// 检查一个状态是否是另一个状态的子状态
bool hsm_is_child(const hsm_t *parent, const hsm_t *child);

// 创建带信号和参数的事件
hsm_event_t* hsm_event_make(HSM_SIG_T sig, void *param);
```

### 返回值

```c
typedef enum {
    HSM_RET_HANDLED,   // 事件已处理，不再传播
    HSM_RET_UNHANDLED, // 事件未处理，继续传播给父状态
    HSM_RET_TRAN,      // 状态转换已启动
} HSM_RET_T;
```

### 宏定义

```c
// 返回事件已处理
#define HSM_HANDLED() (HSM_RET_HANDLED)

// 返回事件未处理（传播给父状态）
#define HSM_UNHANDLED() (HSM_RET_UNHANDLED)

// 转换到新状态
#define HSM_TRAN(target) (((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, \
                          ((hsm_obj_t*)(me))->current = target, HSM_RET_TRAN)

// 状态函数类型转换
#define HSM_STATE_FUNC(func) (state_func_t)(func)
```

## 使用示例

请参阅 [examples/led](./examples/led) 目录，查看使用 HSM 控制 LED 的完整示例。

## 许可证

版权所有 (c) 2024, 叶增辉, <y956370541@163.com>

保留所有权利。 