module dffr(d,r,clk,q);
//Declaration of the ports
input d,r,clk;
output reg q;
//Achievement
initial
begin
	q=1'b0;
end

always @(posedge clk or posedge r)
begin	
	if (r) q=1'b0;
	else q=d;
end

endmodule
