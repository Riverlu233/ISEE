module synchroniser(asynch_in,synch_out,clk);
//Declaration of the ports
input asynch_in,clk;
output synch_out;
//Achievement
wire q;
dffr dff1(.d(asynch_in),.r(1'b0),.clk(clk),.q(q));
dffr dff2(.d(q),.r(1'b0),.clk(clk),.q(synch_out));

endmodule
