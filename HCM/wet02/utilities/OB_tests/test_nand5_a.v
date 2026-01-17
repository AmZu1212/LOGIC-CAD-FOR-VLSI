// Test circuit with large NAND gate (nand5)
// Should be equivalent to test_nand5_b.v
// Logic: O = NAND(I0, I1, I2, I3, I4) = ~(I0 & I1 & I2 & I3 & I4)

module TopLevelNand5A(I0, I1, I2, I3, I4, O);
  input I0, I1, I2, I3, I4;
  output O;
  
  // Direct implementation using nand5
  nand5 nand1(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O));
  
endmodule
