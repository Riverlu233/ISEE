module pulse_width_conv(in,out,clk);
//Declaration of the ports
input in,clk;
output out;
//Achievement
wire q;
dffr dff1(.d(in),.r(1'b0),.clk(clk),.q(q));
assign out= (!q) && in;

endmodule
