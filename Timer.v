`define START_TIMER_ADDRESS    32'h80000001
`define STOP_TIMER_ADDRESS     32'h80000002
`define TIMEOUT_VALUE_ADDRESS  32'h80000003

module Timer(
             clk,
             reset,
             address,
             data,
             wr,
             rd,
             timeout
             );
		  
input clk, reset;
input [31:0] address;
input wr,  rd;
output reg timeout;
input [31:0] data;

reg [31:0] internalTimer;
reg [31:0] timedoutPeriod;
reg started, stopped;

parameter IDLE         = 0;
parameter COUNTING     = 1;
parameter TIMEDOUT     = 2;

reg [2:0]  currentState  = 0;

always @(posedge clk or posedge reset)
begin
  if (reset)
     timedoutPeriod<= 0;
  else
     if (wr && address == `TIMEOUT_VALUE_ADDRESS)
        timedoutPeriod <= data;
     else
        timedoutPeriod <= timedoutPeriod;
end


always @(posedge clk or posedge reset)
begin
  if (reset)
     stopped<= 0;
  else
     if (wr && address == `STOP_TIMER_ADDRESS)
        stopped <= data[0];
     else
        stopped <= stopped;
    
end


always @(posedge clk or posedge reset)
begin
  if (reset)
     started<= 0;
  else
     if (wr && address == `START_TIMER_ADDRESS)
        started <= data[0];
     else
        started <= started;
end


always @(posedge clk or posedge reset)
begin
   if (reset)
      currentState = IDLE;
   else
     begin
     case (currentState)
     IDLE:
        begin
           if (started)
            begin
               currentState = COUNTING;
               timeout <= 0;
               internalTimer <= 0;
            end
           else
             begin
                currentState = IDLE;
                timeout <= 0;
             end
        end
     
     COUNTING:
        begin
           if (stopped == 0)
            begin	        
              if (internalTimer == (timedoutPeriod - 1))
              begin
                timeout <= 1;
                currentState = IDLE;
                internalTimer <= internalTimer;
              end
              else
               begin
                  timeout <= 0;
                  currentState = COUNTING;
                  internalTimer <= internalTimer + 1; 
               end
            end
            else
            begin
               currentState = IDLE;
               timeout <= 0;
               internalTimer <= internalTimer;
             end
        end
        
        
      default:
        begin
           currentState = IDLE;
           timeout <= 0;
        end
     
     endcase
     end
end


endmodule 