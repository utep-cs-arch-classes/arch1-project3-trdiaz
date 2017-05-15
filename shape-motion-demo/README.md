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
    <br>  .word case1
    <br>  .word case2
    
<br>case0:
 <br>   MOV #880, r12
 <br>   CALL #buzzer_set_period,
 <br>   MOV.b #1, &state
   JMP DONE
<br>case1:
 <br>   MOV #700, r12
  <br>  CALL #buzzer_set_period,
 <br>   MOV.b #2, &state
    JMP DONE
<br>case2:
 <br>   MOV #600, r12
 <br>   CALL #buzzer_set_period,
 <br>   MOV.b #2, &state
    JMP DONE

<br>default:
 <br>   MOV #0, r12
  <br>  CALL #buzzer_set_period
    
<br>DONE:
  <br>  pop R0

      
      


