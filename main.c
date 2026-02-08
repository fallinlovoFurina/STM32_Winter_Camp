
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "motor.h"
#include "IRSensor.h"
#include "delay.h"
#include "Ultrasound.h"

// ???????(5cm)
#define ULTRASONIC_STOP_DIST 5.0f
// ???????
#define ULTRASONIC_INVALID_CNT 3
// ????????
#define WALL_ADJUST_SPEED 20.0f

// ????????
uint8_t ultrasonic_invalid_count = 0;

// ???????
uint8_t red1_state, red2_state, red5_state, red6_state;

// ????
void System_Init(void);
void GPIO_Init_All(void);
uint8_t Check_Ultrasonic_Stop(void);
void Read_IR_Sensors(void);
void Execute_Case1_3(void);
void Execute_Case4(void);
void Execute_Case5(void);
void Adjust_Wall_Distance(void);

int main(void)
{
    // ?????
    System_Init();

    // ?????(??SysTick)
    delay_init();

    // ???GPIO
    GPIO_Init_All();

    // ????????
    IRSensor_Init();

    // ?????(TIM2)
    Motor_Init();

    // ??????(TIM1)
    Ultrasound_Init();

    // ??1?,?????
    Delay_ms(1000);

    // ????:??
    Motor_Stop();

    // ???
    while (1)
    {
        // ???????????
        Read_IR_Sensors();
        
        // ???????????
        uint8_t ultra_stop = Check_Ultrasonic_Stop();
        
        // ==================== ??1-5:??????? ====================
        if (ultra_stop)
        {
            // ??1?
            Motor_Stop();
            Delay_ms(1000);
            
            // ??1: RED1+RED2?? + RED5/6???
            if (red1_state == IR_HAVE_OBSTACLE && red2_state == IR_HAVE_OBSTACLE &&
                red5_state == IR_NO_OBSTACLE && red6_state == IR_NO_OBSTACLE)
            {
                Execute_Case1_3(); // ????
            }
            // ??2: RED1+RED2?? + RED5??+RED6??
            else if (red1_state == IR_HAVE_OBSTACLE && red2_state == IR_HAVE_OBSTACLE &&
                     red5_state == IR_NO_OBSTACLE && red6_state == IR_HAVE_OBSTACLE)
            {
                Execute_Case1_3(); // ????
            }
            // ??3: RED1+RED2?? + RED6??+RED5??
            else if (red1_state == IR_HAVE_OBSTACLE && red2_state == IR_HAVE_OBSTACLE &&
                     red5_state == IR_HAVE_OBSTACLE && red6_state == IR_NO_OBSTACLE)
            {
                // ????
                Delay_ms(200); // ??200ms
                Motor_TurnRight90();
                Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
                Motor_TurnRight90();
            }
            // ??4: RED1?? + RED2/5/6???
            else if (red1_state == IR_HAVE_OBSTACLE && red2_state == IR_NO_OBSTACLE &&
                     red5_state == IR_NO_OBSTACLE && red6_state == IR_NO_OBSTACLE)
            {
                Execute_Case4();
            }
            // ??5: RED2?? + RED1/5/6???
            else if (red1_state == IR_NO_OBSTACLE && red2_state == IR_HAVE_OBSTACLE &&
                     red5_state == IR_NO_OBSTACLE && red6_state == IR_NO_OBSTACLE)
            {
                Execute_Case5();
            }
            // ?????????:????????
            else
            {
                Execute_Case1_3();
            }
            
            // ?????????????
            Motor_Forward(NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
        }
        // ==================== ??6:RED1/RED2??? ====================
        else if (red1_state == IR_NO_OBSTACLE && red2_state == IR_NO_OBSTACLE)
        {
            Adjust_Wall_Distance();
        }
        // ==================== ??7:??????? ====================
        else
        {
            // ????
            Motor_Forward(NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
        }
        
        // ????
        Delay_ms(50);
    }
}

/**
 * @brief  ?????
 */
void System_Init(void)
{
    // ???????(??????)
    SystemInit();
    
    // ??NVIC?????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/**
 * @brief  ?????GPIO
 */
void GPIO_Init_All(void)
{
    // ???????IRSensor_Init????
    // ??????????Init??????
    // ?????????
}

/**
 * @brief  ???????????
 * @retval 1:????, 0:?????
 */
uint8_t Check_Ultrasonic_Stop(void)
{
    static uint8_t last_valid = 0;
    static float last_distance = 0.0f;
    
    // ???????
    float distance = Test_Distance();
    
    // ???????
    if (distance <= 0)
    {
        ultrasonic_invalid_count++;
        
        // ????????
        if (ultrasonic_invalid_count >= ULTRASONIC_INVALID_CNT)
        {
            ultrasonic_invalid_count = ULTRASONIC_INVALID_CNT;
            
            // ?????????,???
            if (last_valid && last_distance > 0)
            {
                distance = last_distance;
            }
            else
            {
                return 0; // ??????,?????
            }
        }
        else
        {
            return 0; // ????????,?????
        }
    }
    else
    {
        // ????,?????
        ultrasonic_invalid_count = 0;
        last_distance = distance;
        last_valid = 1;
    }
    
    // ????????
    if (distance <= ULTRASONIC_STOP_DIST)
    {
        // ??????,????
        Motor_Stop();
        return 1; // ??????
    }
    
    return 0; // ?????
}

/**
 * @brief  ???????????
 */
void Read_IR_Sensors(void)
{
    red1_state = IRSensor_Detect(IR_PORT, RED1_PIN);
    red2_state = IRSensor_Detect(IR_PORT, RED2_PIN);
    red5_state = IRSensor_Detect(IR_PORT, RED5_PIN);
    red6_state = IRSensor_Detect(IR_PORT, RED6_PIN);
}

/**
 * @brief  ????1-3?????
 */
void Execute_Case1_3(void)
{
    // ??200ms
    Delay_ms(200);
    
    // ??90?
    Motor_TurnLeft90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnLeft90();
}

/**
 * @brief  ????4???
 */
void Execute_Case4(void)
{
    // ??3cm
    Motor_MoveBack(3.0f);
    
    // ??90?
    Motor_TurnRight90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnLeft90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnLeft90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnRight90();
}

/**
 * @brief  ????5???
 */
void Execute_Case5(void)
{
    // ??3cm
    Motor_MoveBack(3.0f);
    
    // ??90?
    Motor_TurnLeft90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnRight90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnRight90();
    
    // ??10cm
    Motor_MoveForward(10.0f, NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    
    // ??90?
    Motor_TurnLeft90();
}

/**
 * @brief  ??????(??6)
 */
void Adjust_Wall_Distance(void)
{
    // (1)?RED5??:?????????(????)
    if (red5_state == IR_HAVE_OBSTACLE && red6_state == IR_NO_OBSTACLE)
    {
        float left_speed = NORMAL_LEFT_SPEED + WALL_ADJUST_SPEED;
        if (left_speed > 99.0f) left_speed = 99.0f;
        Motor_Forward(left_speed, NORMAL_RIGHT_SPEED);
    }
    // (2)?RED6??:?????????(????)
    else if (red5_state == IR_NO_OBSTACLE && red6_state == IR_HAVE_OBSTACLE)
    {
        float right_speed = NORMAL_RIGHT_SPEED + WALL_ADJUST_SPEED;
        if (right_speed > 99.0f) right_speed = 99.0f;
        Motor_Forward(NORMAL_LEFT_SPEED, right_speed);
    }
    // ????:????
    else
    {
        Motor_Forward(NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
    }
}
