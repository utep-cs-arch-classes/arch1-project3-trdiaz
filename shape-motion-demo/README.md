<b>Name: Thomas Diaz</b>
<br><b>Date: 04.28.17</b>
</br>
<b>Instructions: </b>
<br><p>This MSP430 game is Pong, it works simply by using 4 buttons. Have endless fun.</p>

<b>Game:</b><p> Ping Pong
Explanation: Enjoy the fun and exciting game that is Ping Pong</p>
<br>
Controller: 
<br>Red
<br>SW1: Up 
SW2: Down

<br>Blue
<br>SW3: Up
SW4: Down

/******************ASSEMBLY******************************/
<br>.data
<br>state: .byte 0

<br>.text
<br>Jump: <br>.word case0
      .word case1
      .word case2
    
<br>case0:
    MOV #880, r12
    CALL #buzzer_set_period,
    MOV.b #1, &state
    JMP DONE
<br>case1:
    MOV #700, r12
    CALL #buzzer_set_period,
    MOV.b #2, &state
    JMP DONE
<br>case2:
    MOV #600, r12
    CALL #buzzer_set_period,
    MOV.b #2, &state
    JMP DONE

<br>default:
    MOV #0, r12
    CALL #buzzer_set_period
    
<br>DONE:
    pop R0

      
      


