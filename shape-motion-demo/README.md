Name: Thomas Diaz
Date: 04.28.17

Instructions: 
This MSP430 game is Pong, it works simply by using 4 buttons. Have endless fun.

Game: Ping Pong
Explanation: Enjoy the fun and exciting game that is Ping Pong

Controller: 
Red
SW1: Up 
SW2: Down

Blue
SW3: Up
SW4: Down

/******************ASSEMBLY******************************/
.data
state: .byte 0

.text
Jump: .word case0
      .word case1
      .word case2
    
case0:
    MOV #880, r12
    CALL #buzzer_set_period,
    MOV.b #1, &state
    JMP DONE
case1:
    MOV #700, r12
    CALL #buzzer_set_period,
    MOV.b #2, &state
    JMP DONE
case2:
    MOV #600, r12
    CALL #buzzer_set_period,
    MOV.b #2, &state
    JMP DONE

default:
    MOV #0, r12
    CALL #buzzer_set_period
    
DONE:
    pop R0

      
      


