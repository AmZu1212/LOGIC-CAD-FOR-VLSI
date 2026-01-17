// Test circuit 4441 - Same logic with DFF (equivalent version)
// Should be equivalent to test4440.v
// Logic: O0 = I0 | I1 (using NAND gates)
//        O1 = register(~I1) & I0 (using NAND gates)

module TopLevel4441(I0, I1, CLK, O0, O1);
  input I0, I1, CLK;
  output O0, O1;
  
  wire n1, n2, n3, q1, n4;
  
  // Combinational logic for O0 using NAND: I0 | I1 = ~(~I0 & ~I1)
  inv inv1(.A(I0), .Y(n2));            // n2 = ~I0
  inv inv2(.A(I1), .Y(n3));            // n3 = ~I1
  nand2 nand1(.A(n2), .B(n3), .Y(O0)); // O0 = ~(~I0 & ~I1) = I0 | I1
  
  // Registered output O1 using NAND: q1 & I0 = ~(~q1 | ~I0) = ~~(q1 & I0)
  inv inv3(.A(I1), .Y(n1));            // n1 = ~I1
  dff dff1(.D(n1), .CLK(CLK), .Q(q1)); // q1 = register(~I1)
  nand2 nand2(.A(q1), .B(I0), .Y(n4)); // n4 = ~(q1 & I0)
  inv inv4(.A(n4), .Y(O1));            // O1 = ~~(q1 & I0) = q1 & I0
  
endmodule
