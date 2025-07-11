`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: zju
// Engineer:qmj

////////////////////////////////////////////////////////////////////////////////

module control_tb_v;
   parameter delay=10;
	// Inputs
	reg clk;
	reg in;
	reg reset;

	// Outputs
	wire clr;
	wire count;
	wire save;
	wire disp;

	// Instantiate the Unit Under Test (UUT)
	count_controller control_inst(
		.clk(clk), 
		.reset(reset),
		.in(in), 	
		.clr(clr), 
		.count(count),
		.save(save),
		.disp(disp) );

	initial 
	begin
		// Initialize Inputs
		clk = 0;
		in = 0;
		reset = 0;
		#(delay/2+1) reset = 1;
		#(delay) reset = 0;
		repeat (10)
		 begin
		   #(delay*10) in = 1;
		   #(delay)    in = 0;
		 end
		#(delay*10)		$stop;
	end
     
		// 
	always 
		#(delay/2) clk=~clk;

	
endmodule

