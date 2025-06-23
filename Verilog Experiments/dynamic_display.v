module display(clk,scan,d0,d1,d2,d3,a,b,c,d,e,f,g,dp,pos);
//Declaration
input  clk,scan;
input  [3:0] d0;
input  [3:0] d1;
input  [3:0] d2;
input  [3:0] d3;
output  a,b,c,d,e,f,g,dp;
output [3:0]  pos;

//Achievement
wire [3:0] din;
wire [1:0] sel;
reg dp_tmp;
reg [6:0] seq = 7'b1111111;
//Instantiate the mod-4 counter
counter_n #(.n(4),.counter_bits(2)) counter_mod4(.clk(clk),.r(),.en(scan),.co(),.q(sel));
//Instantiate the mux-4to1
mux_4to1 #(.n(4)) mux(.out(din),.in0(d0),.in1(d1),.in2(d2),.in3(d3),.addr(sel));
//Instantiate the 2to4 decoder
decoder_24 decoder(.out(pos),.in(sel));

always@(*)
begin
  case(pos)
    4'b0111:	dp_tmp=1'b0;
    4'b1101:	dp_tmp=1'b0;
    default:	dp_tmp=1'b1;
  endcase
end

assign dp=dp_tmp;

always @(*)
begin
  case(din)
    4'b0000:	seq = 7'b0000001;
    4'b0001:	seq = 7'b1001111;
    4'b0010:	seq = 7'b0010010;
    4'b0011:	seq = 7'b0000110;
    4'b0100:	seq = 7'b1001100;
    4'b0101:	seq = 7'b0100100;
    4'b0110:	seq = 7'b0100000;
    4'b0111:	seq = 7'b0001111;
    4'b1000:	seq = 7'b0000000;
    4'b1001:	seq = 7'b0000100;
    default:	seq = 7'b1111111;
  endcase
end

assign a = seq[6];
assign b = seq[5];
assign c = seq[4];
assign d = seq[3];
assign e = seq[2];
assign f = seq[1];
assign g = seq[0];

endmodule
