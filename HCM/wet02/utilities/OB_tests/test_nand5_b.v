// Test circuit with large NAND gate (nand5) - equivalent version
// Should be equivalent to test_nand5_a.v
// Logic: O = NAND(I0, I1, I2, I3, I4) = ~(I0 & I1 & I2 & I3 & I4)
// Implemented as: AND all inputs, then invert

module TopLevelNand5B(I0, I1, I2, I3, I4, O);
  input I0, I1, I2, I3, I4;
  output O;
  
  wire w1;
  
  // Equivalent implementation: AND then INV
  and5 and1(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(w1));
  inv inv1(.A(w1), .Y(O));
  
endmodule
