## 命名规范
约定的前缀：
```C
v    void
x    BaseType_t、布尔状态、通用对象
ux   UBaseType_t、计数、索引、长度

c    int8_t / char
s    int16_t
l    int32_t
uc   uint8_t
us   uint16_t
ul   uint32_t

f    float
d    double

p    指针总前缀
pc   char *
puc  uint8_t *
pus  uint16_t *
pul  uint32_t *
px   结构体或对象指针
pv   void *
```

### 类型名
使用首字母大写与 `_t` 结尾的风格，例如：
```C
TaskHandle_t
StackType_t
TickType_t
```

### 函数名
沿用 FreeRTOS 的风格：
`[前缀][模块名][对象或动作][补充说明]`
例如：
```C
BaseType_t xMotorStart( MotorHandle_t xMotor );
void vMotorStop( MotorHandle_t xMotor );
uint32_t ulEncoderGetCount( EncoderHandle_t xEncoder );
```

### 变量名
与函数名类似，采用类型前缀与后续单词首字母大写

### 宏
全部大写 + 下划线分隔，如：
```C
MOTOR_ENABLE_SOFT_START
MOTOR_MAX_DUTY

MOTOR_FLAG_ENABLED
IMU_STATUS_READY
```

### 枚举
枚举类型使用 _t，枚举值全大写并带模块前缀：
```C
typedef enum
{
    CTRL_STATE_IDLE = 0,
    CTRL_STATE_INIT,
    CTRL_STATE_RUNNING,
    CTRL_STATE_ERROR
} ControlState_t;
```

### 私有函数与私有宏
使用 prv 前缀或模块前缀，如：
```
#define motorDEFAULT_TIMEOUT_MS    10U
#define motorMAX_INSTANCE_COUNT    4U

static void prvMotorApplyDuty( MotorHandle_t xMotor, uint16_t usDuty );
static BaseType_t prvImuReadRawData( ImuHandle_t xImu, ImuRawData_t * pxData );
```

### 局部变量
局部临时变量的命名可以随意，但建议保留指针、句柄等变量的前缀
