module stopwatch(ButtonIn,clk,reset,pos,dp,a,b,c,d,e,f,g);
//Declaration of the ports
parameter sim=0;
input ButtonIn,clk,reset;
output dp,a,b,c,d,e,f,g;
output [3:0] pos;

//Achievement
wire pulse400Hz,pulse10Hz;
Devider #(.sim(sim))devider_inst(.clk(clk),.pulse400Hz(pulse400Hz),.pulse10Hz(pulse10Hz));

wire Button_pulse;
button_process_unit #(.sim(sim))butt_proc(.reset(reset),.ButtonIn(ButtonIn),.clk(clk),.ButtonOut(Button_pulse));

wire clr,count,save,disp;
count_controller controller(.clk(clk),.reset(reset),.in(Button_pulse),.clr(clr),.count(count),.save(save),.disp(disp));

wire en;
assign en = pulse10Hz&&count;
wire [3:0] d0,d1,d2,d3; //to save the output of the d-flip-flop
wire [3:0] q0;//to save the output of the counter
wire [7:0] qs;
wire [3:0] qm;
wire [3:0] out0,out1,out2,out3;//the output of the mux

Counter counter(.clk(clk),.r(clr),.en(en),.q0(q0),.qs(qs),.qm(qm));
//Saving Module
mux_2to1 #(.n(4))disp0(.out(out0),.in0(q0),.in1(d0),.addr(disp));
mux_2to1 #(.n(4))disp1(.out(out1),.in0(qs[3:0]),.in1(d1),.addr(disp));
mux_2to1 #(.n(4))disp2(.out(out2),.in0(qs[7:4]),.in1(d2),.addr(disp));
mux_2to1 #(.n(4))disp3(.out(out3),.in0(qm),.in1(d3),.addr(disp));

dffre #(.n(4))dff_out0(.d(q0),.en(save),.r(reset),.clk(clk),.q(d0));
dffre #(.n(4))dff_out1(.d(qs[3:0]),.en(save),.r(reset),.clk(clk),.q(d1));
dffre #(.n(4))dff_out2(.d(qs[7:4]),.en(save),.r(reset),.clk(clk),.q(d2));
dffre #(.n(4))dff_out3(.d(qm),.en(save),.r(reset),.clk(clk),.q(d3));

display display_inst(.clk(clk),.scan(pulse400Hz),.d0(out0),.d1(out1),.d2(out2),.d3(out3),.a(a),.b(b),.c(c),.d(d),.e(e),.f(f),.g(g),.dp(dp),.pos(pos));

endmodule
