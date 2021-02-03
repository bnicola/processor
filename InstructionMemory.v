


module InstructionMemory
       (
       read,
       write,
	   address,
      dataIn,
      dataOut
	   );
 
input  read,write;
input[31:0]  address;
input[31:0]  dataIn;
output reg [31:0] dataOut;

reg [31:0] mem[1024]; 
	   
always @(*)
begin
   if (read)
      dataOut <= mem[address];
end


 initial 
 begin
  $readmemh("memory.list", mem);
 end
	  
endmodule 