module button_process_unit(reset,ButtonIn,clk,ButtonOut);
//Declaration of the ports
parameter sim=0;
input ButtonIn,clk,reset;
output ButtonOut;
//Achievement
wire synch_out,tmp_sig;
synchroniser synch(.asynch_in(ButtonIn),.synch_out(synch_out),.clk(clk));
debouncer #(.sim(sim))debonc(.in(synch_out),.out(tmp_sig),.reset(reset),.clk(clk));
pulse_width_conv converter(.in(tmp_sig),.out(ButtonOut),.clk(clk));

endmodule
