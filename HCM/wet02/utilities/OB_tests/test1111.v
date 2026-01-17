// Test circuit 1111 - Different from test1110.v (O1 is inverted)
// Should NOT be equivalent to test1110.v

module TopLevel1111(I0, I1, O0, O1);
  input I0, I1;
  output O0, O1;
  
  wire n1, n2, n3;
  
  inv inv1(.A(I0), .Y(n1));           // n1 = ~I0
  and2 and1(.A(I1), .B(n1), .Y(n2)); // n2 = I1 & ~I0
  or2 or1(.A(n2), .B(I0), .Y(O0));   // O0 = n2 | I0 (same as 1110)
  inv inv2(.A(I1), .Y(n3));           // n3 = ~I1
  inv inv3(.A(n3), .Y(O1));           // O1 = ~~I1 = I1 (DIFFERENT!)
  
endmodule
