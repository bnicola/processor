


module ProgramCounter
       (
	   clk,
	   reset,
      load,
	   loadAddress,
	   interrupt,
	   interruptAddress,
	   instrAddress,
	   hlt,
      call,
      return
	   );

input clk, reset, hlt,call, return;	   
input[31:0]  loadAddress;
input[31:0]  interruptAddress;
input        interrupt, load;
output reg [31:0] instrAddress ; 
	   
always @(posedge clk or negedge reset)
begin
   if (!reset)
      instrAddress <=0;
   else
     begin
	    if (load == 0 )
		   begin
		      if (hlt == 0)
			     begin
		            if (interrupt == 0)
                     instrAddress <= instrAddress + 1; 
					   else
                     instrAddress <= interruptAddress;
				   end
			  else
              instrAddress <= instrAddress;
		   end
		else 
         instrAddress <= loadAddress;	
	 end
end
	  
endmodule 