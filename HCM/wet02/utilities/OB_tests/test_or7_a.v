// Test circuit with large OR gate (or7)
// Should be equivalent to test_or7_b.v
// Logic: O0 = OR(I0, I1, I2, I3, I4, I5, I6)
//        O1 = NOR(I0, I1, I2)

module TopLevelOr7A(I0, I1, I2, I3, I4, I5, I6, O0, O1);
  input I0, I1, I2, I3, I4, I5, I6;
  output O0, O1;
  
  // Direct implementation using or7 and nor3
  or7 or1(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O0));
  nor3 nor1(.A(I0), .B(I1), .C(I2), .Y(O1));
  
endmodule
