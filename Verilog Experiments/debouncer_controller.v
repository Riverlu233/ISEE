module debunc_controller(in,timer_done,clk,timer_clr,out,reset,q1,q2);
//Declaration of the ports
input in,clk,timer_done,reset;
output timer_clr,out;
//Achievement
output q1,q2;
wire d1,d2;
mux_4to1 mux1(.out(d1),.in0(1'b0),.in1(timer_done),.in2(1'b1),.in3(!timer_done),.addr({q1,q2}));
mux_4to1 mux2(.out(d2),.in0(in),.in1(!timer_done),.in2(!in),.in3(!timer_done),.addr({q1,q2}));

dffr dff1(.d(d1),.r(reset),.clk(clk),.q(q1));
dffr dff2(.d(d2),.r(reset),.clk(clk),.q(q2));

assign timer_clr=!q2;
assign out=q1 || q2;

endmodule
