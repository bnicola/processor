
module PC
       (
	   clk, 
      reset, 
      load, 
      hlt,
	   instrAddress,
	   pcOut,
      pcOutPlus1
	   );

input clk, reset, load, hlt;
input[31:0] instrAddress;
output reg [31:0] pcOut, pcOutPlus1;


always @(posedge clk or posedge reset)
if (reset ==1)
  pcOut <= 0;
else
if (load)
  pcOut <=  instrAddress;
else
  if (!hlt)
     pcOut <= pcOut + 1;
  else
     pcOut <= pcOut;


always @(*)
  pcOutPlus1 <= pcOut + 1;

endmodule 