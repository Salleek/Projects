/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header file */
#include "Board.h"

int error = 0;
int home_button;
int learn_button;
int back_to_select;
int back_to_locker;
int back_to_code;
int start_button;

int new_button;
int existing_button;

//Lockers
int chosen_locker;

int locker_1;
int locker_2;
int locker_3;

int open_locker_1;
int open_locker_2;
int open_locker_3;

int locker1_lock;
int locker2_lock;
int locker3_lock;

int locker1_occ;    //Used when choosing a locker to unlock
int locker2_occ;
int locker3_occ;

int locker1_status;
int locker2_status;
int locker3_status;

int current;        //Keeps track of what screen the user should be at

//Password
int enter_ready;
int pass_full;
int code[4];
int locker_code_1[4];
int locker_code_2[4];
int locker_code_3[4];
int confirm_code_1[4];
int confirm_code_2[4];
int confirm_code_3[4];
char * msg;
char * display[4];
//Keypad
int count;
int keypad_0;
int keypad_1;
int keypad_2;
int keypad_3;
int keypad_4;
int keypad_5;
int keypad_6;
int keypad_7;
int keypad_8;
int keypad_9;
int keypad_confirm;
int keypad_reset;
int keypad_final;
int opening;

void isFull(int n)
{
    if(n == 4)
    {
        count = 0;
        pass_full = 1;
        enter_ready = 0;
    }
}

void reset_code()
{
    for(int i = 0; i < 4; i++)
    {
        code[i] = 0;
        display[i] = 0;
    }
    count = 0;
    pass_full = 0;
    enter_ready = 1;
}

void check_lockers()
{
    if(locker1_status != 0)
    {
        locker1_lock = 1;
        locker1_occ = 0;
    }
    
    else if(locker1_status == 0)
    {
        locker1_lock = 0;
        locker1_occ = 1;
    }
    
    if(locker2_status != 0)
    {
        locker2_lock = 1;
        locker2_occ = 0;
    }
    
    else if(locker2_status == 0)
    {
        locker2_lock = 0;
        locker2_occ = 1;
    }
    
    if(locker3_status != 0)
    {
        locker3_lock = 1;
        locker3_occ = 0;
    }
    
    else if(locker3_status == 0)
    {
        locker3_lock = 0;
        locker3_occ = 1;
    }
}

void init_buttons()
{
    error = 0;
    home_button = 0;
    learn_button = 0;
    back_to_select = 0;
    back_to_locker = 0;
    back_to_code = 0;
    start_button = 0;

    enter_ready = 1;
    new_button = 0;
    existing_button = 0;
    
    count = 0;
    keypad_0 = 0;
    keypad_1 = 0;
    keypad_2 = 0;
    keypad_3 = 0;
    keypad_4 = 0;
    keypad_5 = 0;
    keypad_6 = 0;
    keypad_7 = 0;
    keypad_8 = 0;
    keypad_9 = 0;
    keypad_confirm = 0;
    keypad_reset = 0;
    keypad_final = 0;
    opening = 0;
    
    chosen_locker = 0;

    locker_1 = 0;
    locker_2 = 0;
    locker_3 = 0;

    open_locker_1 = 0;
    open_locker_2 = 0;
    open_locker_3 = 0;
    
    locker1_lock = 0;
    locker2_lock = 0;
    locker3_lock = 0;

    locker1_occ = 1;  
    locker2_occ = 1;
    locker3_occ = 1;

    locker1_status = 0;
    locker2_status = 0;
    locker3_status = 0;

    current = 0; 
    
    pass_full = 0;
    msg = 0;
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* 1 second delay */
    uint32_t time = 10;

    /* Call driver init functions */
    GPIO_init();
    // I2C_init();
    // SPI_init();
    // UART_init();
    // Watchdog_init();

    GPIO_setConfig(Board_GPIO_08, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    GPIO_write(Board_GPIO_08, 1);
    
    GPIO_setConfig(Board_GPIO_07, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    GPIO_write(Board_GPIO_07, 1);
    
    GPIO_setConfig(Board_GPIO_06, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    GPIO_write(Board_GPIO_06, 1);

    init_buttons();
    
    while (1) {
        
        check_lockers();
        
        if(home_button != 0)        //Goes to home screen
        {
            current = 0;
            home_button = 0;
        }
        
        if(learn_button != 0)
        {
            current = 10;           
            learn_button = 0;
        }
        
        if(back_to_select != 0)     //Goes back to selecting new or existing lockers
        {
            current = 1;
            back_to_select = 0;
        }
        
        if(back_to_locker != 0)
        {
            switch(opening)
            {
                case 0:
                    current = 2;
                    break;
                case 1:
                    current = 3;
                    break;
            }
            back_to_locker = 0;
        }
        
        if(back_to_code != 0)       //In case the user wants to use a different password before confirming
        {
            current = 4;
            back_to_code = 0;
        }
        
        if(start_button != 0)       //Goes to screen to select new or occupied locker
        {
            current = 1;
            start_button = 0;
        }
        
        if(new_button != 0)
        {
            current = 2;
            opening = 0;
            new_button = 0;
        }
        
        //New lockers
        if(locker_1 != 0)
        {
            msg = "SET PASSCODE FOR LOCKER 1";
            chosen_locker = 1;
            current = 4;             //Goes to the screen to create password
            locker_1 = 0;
        }
        
        if(locker_2 != 0)
        {
            msg = "SET PASSCODE FOR LOCKER 2";
            chosen_locker = 2;
            current = 4;
            locker_2 = 0;
        }
        
        if(locker_3 != 0)
        {
            msg = "SET PASSCODE FOR LOCKER 3";
            chosen_locker = 3;
            current = 4;
            locker_3 = 0;
        }
        
        //Existing lockers
        if(existing_button != 0)
        {
            current = 3;
            opening = 1;
            existing_button = 0;
        }
        
        if(open_locker_1 != 0)
        {
            msg = "ENTER PASSCODE FOR LOCKER 1";
            chosen_locker = 1;
            current = 6;      
            open_locker_1 = 0;
            
        }
        
        if(open_locker_2 != 0)
        {
            msg = "ENTER PASSCODE FOR LOCKER 2";
            chosen_locker = 2;
            current = 6;
            open_locker_2 = 0;
        }
        
        if(open_locker_3 != 0)
        {
            msg = "ENTER PASSCODE FOR LOCKER 3";
            chosen_locker = 3;
            current = 6;
            open_locker_3 = 0;
        }
        
        //Keypad
        if(keypad_0 != 0)
        {
            display[count] = "*";
            code[count] = 0;
            count++;
            isFull(count);
            keypad_0 = 0;
        }
        
        if(keypad_1 != 0)
        {
            display[count] = "*";
            code[count] = 1;
            count++;
            isFull(count);
            keypad_1 = 0;
        }
        
        if(keypad_2 != 0)
        {
            display[count] = "*";
            code[count] = 2;
            count++;
            isFull(count);
            keypad_2 = 0;
        }
        
        if(keypad_3 != 0)
        {
            display[count] = "*";
            code[count] = 3;
            count++;
            isFull(count);
            keypad_3 = 0;
        }
        
        if(keypad_4 != 0)
        {
            display[count] = "*";
            code[count] = 4;
            count++;
            isFull(count);
            keypad_4 = 0;
        }
        
        if(keypad_5 != 0)
        {
            display[count] = "*";
            code[count] = 5;
            count++;
            isFull(count);
            keypad_5 = 0;
        }
        
        if(keypad_6 != 0)
        {
            display[count] = "*";
            code[count] = 6;
            count++;
            isFull(count);
            keypad_6 = 0;
        }
        
        if(keypad_7 != 0)
        {
            display[count] = "*";
            code[count] = 7;
            count++;
            isFull(count);
            keypad_7 = 0;
        }
        
        if(keypad_8 != 0)
        {
            display[count] = "*";
            code[count] = 8;
            count++;
            isFull(count);
            keypad_8 = 0;
        }
        
        if(keypad_9 != 0)
        {
            display[count] = "*";
            code[count] = 9;
            count++;
            isFull(count);
            keypad_9 = 0;
        }
        
        if(keypad_confirm != 0)
        {
            switch(chosen_locker)
            {
                case 1:
                    msg = "CONFIRM PASSCODE FOR LOCKER 1";
                    for(int i = 0; i < 4; i++)
                    {
                        locker_code_1[i] = code[i];
                    }
                    break;
                    
                case 2:
                    msg = "CONFIRM PASSCODE FOR LOCKER 2";
                    for(int i = 0; i < 4; i++)
                    {
                        locker_code_2[i] = code[i];
                    }
                    break;
                    
                case 3:
                    msg = "CONFIRM PASSCODE FOR LOCKER 3";
                    for(int i = 0; i < 4; i++)
                    {
                        locker_code_3[i] = code[i];
                    }
                    break;    
            }
            
            reset_code();
            keypad_confirm = 0;
            current = 5;
        }
        
        if(keypad_reset != 0)
        {
            reset_code();
            keypad_reset = 0;
        }
        
        if(keypad_final != 0)
        {
            error = 0;
            
            switch(chosen_locker)
            {
                case 1:
                    for(int i = 0; i < 4; i++)
                    {
                        confirm_code_1[i] = code[i];
                    }
                    break;
                    
                case 2:
                    for(int i = 0; i < 4; i++)
                    {
                        confirm_code_2[i] = code[i];
                    }
                    break;
                    
                case 3:
                    for(int i = 0; i < 4; i++)
                    {
                        confirm_code_3[i] = code[i];
                    }
                    break;    
            }
            
            switch(chosen_locker)
            {
                case 1:
                    for(int i = 0; i < 4; i++)
                    {
                        if(confirm_code_1[i] != locker_code_1[i])
                        {
                            error = 1;
                            break;
                        }
                    }
                    
                case 2:
                    for(int i = 0; i < 4; i++)
                    {
                        if(confirm_code_2[i] != locker_code_2[i])
                        {
                            error = 1;
                            break;
                        }
                    }
                    
                case 3:
                    for(int i = 0; i < 4; i++)
                    {
                        if(confirm_code_3[i] != locker_code_3[i])
                        {
                            error = 1;
                            break;
                        }
                    }
            }
            
            if(error)
            {
                reset_code();
                current = 8;
                sleep(5);
                switch(opening)
                {
                    case 0:
                        current = 5;
                        break;
                    case 1:
                        current = 6;
                        break;
                }
            }
            
            else
            {
                reset_code();
                switch(opening)
                {
                    case 0:
                        current = 7;
                        switch(chosen_locker)
                        {
                            case 1:
                                msg = "LOCKER 1 IS READY TO USE!";
                                locker1_status = 1;           
                                GPIO_write(Board_GPIO_08, 0);
                                sleep(time);
                                GPIO_write(Board_GPIO_08, 1);
                                break;
                            case 2:
                                msg = "LOCKER 2 IS READY TO USE!";
                                locker2_status = 1;
                                GPIO_write(Board_GPIO_07, 0);   
                                sleep(time);
                                GPIO_write(Board_GPIO_07, 1);
                                break;
                            case 3:
                                msg = "LOCKER 3 IS READY TO USE!";
                                locker3_status = 1;
                                GPIO_write(Board_GPIO_06, 0);   
                                sleep(time);
                                GPIO_write(Board_GPIO_06, 1);
                                break;
                        }
                        break;
                        
                    case 1:
                        current = 9;
                        switch(chosen_locker)
                        {
                            case 1:
                                locker1_status = 0;
                                GPIO_write(Board_GPIO_08, 0);   
                                sleep(time);
                                GPIO_write(Board_GPIO_08, 1);
                                break;
                            case 2:
                                locker2_status = 0;
                                GPIO_write(Board_GPIO_07, 0);   
                                sleep(time);
                                GPIO_write(Board_GPIO_07, 1);
                                break;
                            case 3:
                                locker3_status = 0;
                                GPIO_write(Board_GPIO_06, 0);   
                                sleep(time);
                                GPIO_write(Board_GPIO_06, 1);
                                break;
                        }
                        break;
                }
                current = 0;    //Should only change if the password was correct
            }
            keypad_final = 0;
        }
    }
}

