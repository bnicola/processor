

module RegisterFile
       (
          src1, 
          src2,
          dest,
          inData,
          outData1,
          outData2,
          wr,
          clk,
          reset
       );
               
input [4:0] src1;
input [4:0] src2;
input [4:0] dest; 
input [31:0] inData;
output reg [31:0] outData1;
output reg [31:0] outData2;
input wr;
input clk;
input reset;


// The actual register file.
reg [31:0] regFile[31:0];

always @(*)
begin 
   outData1 =  regFile[src1];
end


always @(*)
begin 
   outData2 =  regFile[src2];
end


always @(posedge clk)
begin 
   if (wr)
      regFile[dest] <= inData;
end

 
endmodule 