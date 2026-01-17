// Test circuit 0001 - Same logic as test0000.v
// Should be equivalent to test0000.v

module TopLevel0001(I0, I1, O0, O1);
  input I0, I1;
  output O0, O1;
  
  wire n1, n1_buf, n2;
  
  inv inv1(.A(I0), .Y(n1));           // n1 = ~I0
  buffer buf1(.A(n1), .Y(n1_buf));    // n1_buf = n1 (buffer added)
  and2 and1(.A(n1_buf), .B(I1), .Y(n2)); // n2 = ~I0 & I1 (swapped operands)
  or2 or1(.A(I0), .B(n2), .Y(O0));   // O0 = I0 | n2 (swapped operands)
  inv inv2(.A(I1), .Y(O1));           // O1 = ~I1
  
endmodule
