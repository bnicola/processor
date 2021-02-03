

module ClockControl(clk,  
                    reset,
                    run,
                    halt,
                    step,
                    clockOut);
                    
input clk;
input reset;
input run;
input halt;
input step;
output wire clockOut;

reg allow;
 
parameter IDLE          = 0;
parameter RUNNING       = 1;
parameter STEPPING      = 2;
parameter WAIT_STEPPING = 3;
parameter HALT          = 4;

reg [2:0]     currentState     = 0;

always @(posedge clk or posedge reset)
 begin
    if (reset)
       allow <= 0;
    else 
     begin
       case (currentState)
          IDLE :
             begin
                allow <= 0;
                if(run)
                   currentState <= RUNNING;
                else if (step)
                   currentState <= STEPPING; 
                else
                   currentState <= IDLE; 
             end
          RUNNING :
             begin
               allow <= 1;
               if (halt)
                  currentState <= IDLE; 
               else
                  currentState <= RUNNING; 
             end     
          STEPPING :
             begin
               allow <= 1;
               currentState <= WAIT_STEPPING; 
             end    
           WAIT_STEPPING :
             begin
               allow <= 0;
               if (step)
                  currentState <= WAIT_STEPPING; 
               else
                  currentState <= IDLE;  
             end    
          HALT :
             begin
               currentState <= IDLE; 
             end
          default :
             currentState <= IDLE;          
        endcase
     end 
 end
 

  assign clockOut = clk & allow; 
 
 
endmodule
                    
           