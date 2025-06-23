module debouncer(in,out,reset,clk);
//Declaration of the ports
parameter sim=0;
input in,clk,reset;
output out;
//Achievement
wire pulse_1kHz;
wire q1,q2;
wire timer_done,timer_clr;
counter_n #(.n(sim?32:10_0000),.counter_bits(sim?5:18))counter(.clk(clk),.r(reset),.en(1'b1),.co(pulse_1kHz),.q());
timer timer10ms(.clk(clk),.r(timer_clr),.en(pulse_1kHz),.done(timer_done));
debunc_controller ctrl(.in(in),.timer_done(timer_done),.clk(clk),.timer_clr(timer_clr),.out(out),.reset(reset),.q1(q1),.q2(q2));

endmodule
