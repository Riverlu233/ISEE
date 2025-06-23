module mux_2to1(out,in0,in1,addr);
//Declarations
parameter n=1;
output reg [n-1:0] out;
input [n-1:0] in0,in1;
input addr;

//Achievements
always @(*)
begin
  case(addr)
    1'b0:	out = in0;
    1'b1:	out = in1;
  endcase
end

endmodule
