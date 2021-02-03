module Mux5(in1, in2, sel, out);

input[4:0] in1;
input[4:0] in2;
input sel;
output reg [4:0]  out;

always @(*)
   if (sel == 0)
      out = in1;
   else
      out = in2;

endmodule 