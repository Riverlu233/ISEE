module decoder_24(out,in);
//Declarations
output [3:0] out;
input [1:0] in;
reg [3:0] out;

//Achievements
always @(*)
begin
  case(in)
//Low Voltage to be validate
    2'b00:	out = 4'b1110;
    2'b01:	out = 4'b1101;
    2'b10:	out = 4'b1011;
    default:	out = 4'b0111;
  endcase
end

endmodule
