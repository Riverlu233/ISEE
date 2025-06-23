module count_controller(clk,reset,in,clr,count,save,disp);
//Declaration of in&out
input  clk,reset,in;
output  clr,count,save,disp;

//Define the states
parameter RESET=3'b000;
parameter TIMING_1=3'b001;
parameter TIMING_2=3'b010;
parameter STOP_1=3'b011;
parameter STOP_2=3'b100;

//machine variabe
reg [2:0] cur_state;
reg [2:0] next_state;

//Part1 State Transfer
always @(posedge clk or posedge reset)
begin
  if (reset)
	cur_state=RESET;
  else
	cur_state=next_state;
end

//Part2 state switch
always@(*)
begin
  case(cur_state)
	RESET:
	  case(in)
		1'b0:next_state=RESET;
		1'b1:next_state=TIMING_1;
	  endcase
	TIMING_1:
	  case(in)
		1'b0:next_state=TIMING_1;
		1'b1:next_state=TIMING_2;
	  endcase
	TIMING_2:
	  case(in)
		1'b0:next_state=TIMING_2;
		1'b1:next_state=STOP_1;
	  endcase
	STOP_1:
	  case(in)
		1'b0:next_state=STOP_1;
		1'b1:next_state=STOP_2;
	  endcase
	STOP_2:
	  case(in)
		1'b0:next_state=STOP_2;
		1'b1:next_state=RESET;
	  endcase
  endcase
end

//Assign the output
assign clr = !cur_state[2] && !cur_state[1] && !cur_state[0];
assign count = cur_state[1]^cur_state[0];
assign save = !cur_state[1]&&cur_state[0]&&in;
assign disp = cur_state[2];

endmodule
