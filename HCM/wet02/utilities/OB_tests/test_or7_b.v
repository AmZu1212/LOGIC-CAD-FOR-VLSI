// Test circuit with large OR gate (or7) - equivalent version
// Should be equivalent to test_or7_a.v
// Logic: O0 = OR(I0, I1, I2, I3, I4, I5, I6)
//        O1 = NOR(I0, I1, I2)
// Implemented using De Morgan's law: OR = ~(NAND of inverted inputs)
//                                    NOR = ~(OR)

module TopLevelOr7B(I0, I1, I2, I3, I4, I5, I6, O0, O1);
  input I0, I1, I2, I3, I4, I5, I6;
  output O0, O1;
  
  wire w1, w2, w3, w4, w5, w6, w7, w8, w9;
  
  // Equivalent implementation for O0: OR = ~NAND(~inputs)
  inv inv1(.A(I0), .Y(w1));
  inv inv2(.A(I1), .Y(w2));
  inv inv3(.A(I2), .Y(w3));
  inv inv4(.A(I3), .Y(w4));
  inv inv5(.A(I4), .Y(w5));
  inv inv6(.A(I5), .Y(w6));
  inv inv7(.A(I6), .Y(w7));
  nand7 nand1(.A(w1), .B(w2), .C(w3), .D(w4), .E(w5), .F(w6), .G(w7), .Y(O0));
  
  // Equivalent implementation for O1: NOR = ~OR
  or3 or1(.A(I0), .B(I1), .C(I2), .Y(w9));
  inv inv8(.A(w9), .Y(O1));
  
endmodule
