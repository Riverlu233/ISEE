module timer(clk,r,en,done);
//Declaration
parameter  n=10; 
parameter  counter_bits=4;
input   clk,en,r;
output  done;
reg [counter_bits-1:0]  q=0;
//Achievement
assign  done=(q==(n-1)) && en;
always @(posedge clk) 
begin
	if (r) q=0;
	else if (en) q=q+1;
end

endmodule
