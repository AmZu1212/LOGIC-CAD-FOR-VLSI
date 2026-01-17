// Test circuit 1110 - Different from test1111.v
// Should NOT be equivalent to test1111.v

module TopLevel1110(I0, I1, O0, O1);
  input I0, I1;
  output O0, O1;
  
  wire n1, n2;
  
  inv inv1(.A(I0), .Y(n1));           // n1 = ~I0
  and2 and1(.A(I1), .B(n1), .Y(n2)); // n2 = I1 & ~I0
  or2 or1(.A(n2), .B(I0), .Y(O0));   // O0 = n2 | I0
  inv inv2(.A(I1), .Y(O1));           // O1 = ~I1
  
endmodule
