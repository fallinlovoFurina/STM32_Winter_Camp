#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"
#include <stdint.h>

// PID结构体定义
typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数  
    float Kd;           // 微分系数
    float integral;     // 积分项
    float prev_error;   // 上次误差
    float prev2_error;  // 上上次误差（增量式用）
    float output;       // 输出值
    float output_prev;  // 上次输出（增量式用）
    float max_output;   // 输出限幅
    float min_output;
    float max_integral; // 积分限幅
    float min_integral;
    
    // 减速比相关（可选）
    float gear_ratio;
    uint8_t control_target;
} PID_TypeDef;

void Incremental_PID_Init(PID_TypeDef* pid, float kp, float ki, float kd,
                         float max_out, float min_out);
void Positional_PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, 
                        float max_out, float min_out, float max_i, float min_i);

float Incremental_PID_Calculate(PID_TypeDef* pid, float setpoint, float feedback);

#endif

