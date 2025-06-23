module Counter(clk,r,en,q0,qs,qm);
//Declaration
input  clk,r,en;
output  [3:0] q0;
output  [7:0] qs;
output  [3:0] qm;

//Achievement
wire co1,co2,co3;
counter_n #(.n(10),.counter_bits(4)) counter_add1(.clk(clk),.r(r),.en(en),.co(co1),.q(q0));
counter_n #(.n(10),.counter_bits(4)) counter_add2(.clk(clk),.r(r),.en(co1),.co(co2),.q(qs[3:0]));
counter_n #(.n(6),.counter_bits(4)) counter_add3(.clk(clk),.r(r),.en(co2),.co(co3),.q(qs[7:4]));
counter_n #(.n(10),.counter_bits(4)) counter_add4(.clk(clk),.r(r),.en(co3),.co(),.q(qm));

endmodule
