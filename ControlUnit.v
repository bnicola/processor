

module ControlUnit
         (
         instruction,
         instructionAddress,
         jump,
         pcLoadImmediate,
         opCode,
         src1,
         src2,
         dest,
         wr, 
         memrd,
         memwr,
         immediate,
         immediateVal,
         jal,
         aEqualB,
         aSmallerB,
         aBiggerB,
         aSmallerEqualB,
         aBiggerEqualB,
         );

input[31:0] instruction;
input[31:0] instructionAddress;
input aEqualB;
input aSmallerB;
input aBiggerB;
input aSmallerEqualB;
input aBiggerEqualB;

output reg jump;
output reg pcLoadImmediate;
output reg [4:0] opCode;
output reg [4:0] src1;
output reg [4:0] src2;
output reg [4:0] dest;
output reg wr;
output reg memrd;
output reg memwr;
output reg immediate;
output reg [31:0] immediateVal;
output reg jal;

parameter nop_ = 0;
parameter add_ = 1;
parameter sub_ = 2;
parameter and_ = 3;
parameter or_  = 4;
parameter xor_ = 5;
parameter not_ = 6;
parameter mul_ = 7;
parameter div_ = 8;
parameter shl_ = 9;
parameter shr_ = 10;
parameter pas_ = 11;

parameter jal_ = 12;
parameter ret_ = 13;
parameter jmp_ = 14;
parameter lui_ = 15;
parameter mov_ = 16;

parameter addi_ = 17;
parameter subi_ = 18;
parameter andi_ = 19;
parameter ori_  = 20;
parameter xori_ = 21;
parameter noti_ = 22;
parameter muli_ = 23;
parameter divi_ = 24;
parameter shli_ = 25;
parameter shri_ = 26;

parameter lod_  = 27;
parameter str_  = 28;
parameter movi_ = 29;

parameter jeq_  = 30;
parameter jneq_ = 31;
parameter jg_   = 32;
parameter jl_   = 33;

parameter jge_   = 34;
parameter jle_   = 35;

parameter spc_   = 36;


always@(*)
begin
case(instruction[31:26])
  nop_:
     begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
     
   add_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00000;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   addi_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00000;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
    
    sub_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00001;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   subi_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00001;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    and_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00010;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   andi_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00010;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    or_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00011;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   ori_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00011;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
   xor_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00100;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   xori_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00100;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    not_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00101;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   noti_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00101;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end

     mul_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00110;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   muli_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00110;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    div_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00111;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   divi_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b00111;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    shl_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01000;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   shli_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01000;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
   
   shr_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01001;
        src1 <= instruction[20:16];
        src2 <= instruction[15:11];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
   
   shri_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01001;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[15:0];
        jal  <= 0;
     end
     
    pas_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
     
    jal_:
      begin
        jump <= 1;
        pcLoadImmediate <= 1;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= 31;
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[25:0];
        jal  <= 1;
     end
     
    ret_:
      begin
        jump <= 1;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= 31;
        src2 <= 31;
        dest <= 31;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 0;
     end
     
     jmp_:
      begin
        jump <= 1;
        pcLoadImmediate <= 1;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;
        immediateVal <= instruction[25:0];
        jal  <= 0;
     end
     
     
     lui_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;   
        immediateVal[31:16] = instruction[15:0];
        immediateVal[15:0] <= 0;
        jal  <= 0;
      end
      
     mov_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;   
        immediateVal <= 0;
        jal  <= 0;
      end
      
     movi_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 1;   
        immediateVal <= instruction[20:0];
        jal  <= 0;
      end
      
    lod_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        if (instruction[15] == 1)
           opCode <= 5'b00001;
        else if (instruction[15] == 0)
          opCode <= 5'b00000;
        
        src1 <= instruction[20:16];
        src2 <= instruction[20:16];
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 1;
        memwr <= 0;
        immediate <= 1;   
        immediateVal <= instruction[14:0];
        jal  <= 0;
      end
      
     str_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        if (instruction[15] == 1)
           opCode <= 5'b00001;
        else if (instruction[15] == 0)
          opCode <= 5'b00000;
        src1 <= instruction[20:16];
        src2 <= instruction[25:21];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 1;
        immediate <= 1;   
        immediateVal <= instruction[14:0];
        jal  <= 0;
      end
      
     jeq_:
       begin
        jump <= aEqualB;
        pcLoadImmediate <= aEqualB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end
      
    jneq_:
       begin
        jump <= !aEqualB;
        pcLoadImmediate <= !aEqualB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end
       
    jg_:
       begin
        jump <= aBiggerB;
        pcLoadImmediate <= aBiggerB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end
       
     jl_:
       begin
        jump <= aSmallerB;
        pcLoadImmediate <= aSmallerB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end
       
     jge_:
      begin
        jump <= aBiggerEqualB;
        pcLoadImmediate <= aBiggerEqualB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end
       
     jle_:
       begin
        jump <= aSmallerEqualB;
        pcLoadImmediate <= aSmallerEqualB;
        opCode <=  5'b00001;
        src1 <= instruction[25:21];
        src2 <= instruction[20:16];
        dest <= 0;
        wr <= 0;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
         if (instruction[15] == 1)
             immediateVal <= instructionAddress - instruction[14:0];
          else
             immediateVal <= instructionAddress + instruction[14:0];
        jal  <= 0;
     end

     spc_:
      begin
        jump <= 0;
        pcLoadImmediate <= 0;
        opCode <= 5'b01010;
        src1 <= 0;
        src2 <= 0;
        dest <= instruction[25:21];
        wr <= 1;
        memrd <= 0;
        memwr <= 0;
        immediate <= 0;
        immediateVal <= 0;
        jal  <= 1;
     end       
      
endcase

end

endmodule 