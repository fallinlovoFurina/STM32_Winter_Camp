#include "stm32f10x.h"                  // Device header
#include "PID.h"

// 增量式PID初始化
void Incremental_PID_Init(PID_TypeDef* pid, float kp, float ki, float kd,
                         float max_out, float min_out) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->prev2_error = 0;
    pid->output = 0;
    pid->output_prev = 0;
    pid->max_output = max_out;
    pid->min_output = min_out;
    pid->max_integral = 0;
    pid->min_integral = 0;

}
						 
// 位置式PID初始化
void Positional_PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, 
                        float max_out, float min_out, float max_i, float min_i) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->output = 0;
    pid->max_output = max_out;
    pid->min_output = min_out;
    pid->max_integral = max_i;
    pid->min_integral = min_i;
}
						
//增量式PID计算
float Incremental_PID_Calculate(PID_TypeDef* pid, float setpoint, float feedback)
{
	float error, delta_output;
    
    // 计算误差
    error = setpoint - feedback;
    
    // 增量式PID计算
    delta_output = pid->Kp * (error - pid->prev_error) +
                   pid->Ki * error +
                   pid->Kd * (error - 2 * pid->prev_error + pid->prev2_error);
    
    // 计算当前输出
    pid->output = pid->output_prev + delta_output;
    
    // 输出限幅
    if (pid->output > pid->max_output)
	{
        pid->output = pid->max_output;
    } 
	else if (pid->output < pid->min_output) 
	{
        pid->output = pid->min_output;
    }
    
    // 更新误差记录
    pid->prev2_error = pid->prev_error;
    pid->prev_error = error;
    pid->output_prev = pid->output;
    
    return pid->output;
}

