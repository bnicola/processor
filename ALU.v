
module ALU(a,
           b,
           sel,
           f,
           aEqualB,
           aBiggerB,
           aBiggerEqualB,
           aSmallerB,
           aSmallerEqualB
           );
           
 input  [31:0]a;
 input  [31:0]b;
 input  [4:0]sel;
 output reg [31:0] f;
 output reg aEqualB;
 output reg aBiggerB;
 output reg aBiggerEqualB;
 output reg aSmallerB;
 output reg aSmallerEqualB;
           
always @(*)
begin
   case(sel)
   
     5'b00000:
        begin
           f <= a + b;
        end
     5'b00001:
         begin
           f <= a - b;
        end
     5'b00010:
      begin
          f <= a & b;
        end
     5'b00011:
      begin
         f <= a | b;
        end
     5'b00100:
      begin
         f <= a ^ b;
        end
     5'b00101:
      begin
          f <= ~a;
        end
     5'b00110:
      begin
           f <= a * b;
        end
     5'b00111:
      begin
           f <= a / b;
        end
     5'b01000:
      begin
          f <= a << b;
        end
     5'b01001:
      begin
          f <= a >> b;
        end
     5'b01010:
      begin
          f <= b;
        end
        
     endcase

end
always @(f)
begin
  aEqualB <= (f == 0);
  aBiggerB <= (f[31] == 0) & (f[30:0] > 0);
  aBiggerEqualB <= (aBiggerB | aEqualB);
  aSmallerB <= (f[31] == 1) & (f[30:0] > 0);
  aSmallerEqualB <= (aSmallerB | aEqualB);
end
           
endmodule