module mux_4to1(out, in0, in1, in2, in3, addr);
//Declarations
parameter n=1;
output reg [n-1:0] out;
input [n-1:0] in0,in1,in2,in3;
input [1:0] addr;

//Achievements
always @(*)
begin
  case(addr)
    2'b00:	out = in0;
    2'b01:	out = in1;
    2'b10:	out = in2;
    2'b11:	out = in3;
  endcase
end

endmodule
