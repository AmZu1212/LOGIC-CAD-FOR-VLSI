// Test circuit 4440 - Simple logic with DFF
// Should be equivalent to test4441.v
// Logic: O0 = I0 | I1
//        O1 = register(~I1) & I0

module TopLevel4440(I0, I1, CLK, O0, O1);
  input I0, I1, CLK;
  output O0, O1;
  
  wire n1, q1;
  
  // Combinational logic for O0
  or2 or1(.A(I0), .B(I1), .Y(O0));    // O0 = I0 | I1
  
  // Registered output O1 with logic after DFF
  inv inv1(.A(I1), .Y(n1));           // n1 = ~I1
  dff dff1(.D(n1), .CLK(CLK), .Q(q1)); // q1 = register(~I1)
  and2 and1(.A(q1), .B(I0), .Y(O1));  // O1 = q1 & I0
  
endmodule
