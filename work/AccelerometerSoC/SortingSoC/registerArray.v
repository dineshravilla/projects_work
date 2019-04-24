module registerArray(
	input [3:0] inbus,
	input clk,
	input reset,
	input reg even, odd,
	output [3:0] outbus
);	//port declarations
	
	//internal variables
	parameter n	=	4; //bit lenght is 4 bits
	parameter N	=	8; //no.of numbers // N should be even always
	reg [n-1:0] myrows [0 : N-1];
	
	integer i;

always @(posedge clk or negedge reset) begin
	if(!reset) begin
		myrows[0] <= 4'b0011;
		myrows[1] <= 4'b0000;
		myrows[2] <= 4'b1111;
		myrows[3] <= 4'b0000;
		myrows[4] <= 4'b1100;
		myrows[5] <= 4'b0000;
		myrows[6] <= 4'b0000;
		myrows[7] <= 4'b0000;		
	end // reset if ends here
	
	else begin
		
		if(even==1'b1 || odd==1'b0) begin
			
			for(i=0; i<N ; i=i+2) begin
				if( myrows[i] > myrows[i+1]) begin
					myrows [i+1] <= myrows [i];
					myrows [i] <= myrows [i+1];
				end // if ends here
			end // for ends
		end // even if ends here

		if(even == 1'b0 || odd==1'b1) begin
			for(i=1; i<=N-3 ; i=i+2) begin // first and last registers not included in odd
				if( myrows[i] > myrows[i+1]) begin
					myrows [i+1] <= myrows [i];
					myrows [i] <= myrows [i+1];
				end // if ends here
			end // for ends
		end // odd if ends here

		//myrows[0] <= myrows[1];
		//myrows[1] <= myrows[0];


	end // reset else ends here
end //always ended here

endmodule //fourInputBits ends here
