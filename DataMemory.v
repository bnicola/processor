


module DataMemory
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

reg [31:0] mem[20]; 
	   
always @(*)
begin
   if (read)
      dataOut <= mem[address];
end

always @(*)
begin
   if (write)
      mem[address] = dataIn;
end

	  
endmodule 