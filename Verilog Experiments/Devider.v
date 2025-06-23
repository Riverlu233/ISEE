module Devider(clk,pulse400Hz,pulse10Hz);
//Declaration
parameter sim=1'b0;
input   clk;
output  pulse400Hz,pulse10Hz;
//Achievement
counter_n #(.n(sim ? 2:250000),.counter_bits(sim ? 2:18))counter_1(.clk(clk),.r(1'b0),.en(1'b1),.co(pulse400Hz),.q());
counter_n #(.n(sim ? 10:40),.counter_bits(sim ? 4:6))counter_2(.clk(clk),.r(1'b0),.en(pulse400Hz),.co(pulse10Hz),.q());

endmodule
