// Test circuit 5551 - Comprehensive gate test (equivalent version)
// Tests all gate types using De Morgan equivalents where possible (sizes 2-9)
// Should be equivalent to test5550.v

// 4-to-1 MUX using NAND/NOR (De Morgan equivalent)
module mux4to1(D0, D1, D2, D3, S0, S1, Y);
  input D0, D1, D2, D3;
  input S0, S1;
  output Y;
  wire s0_n, s1_n;
  wire w0_n, w1_n, w2_n, w3_n;
  wire w0, w1, w2, w3;
  
  inv inv_s0(.A(S0), .Y(s0_n));
  inv inv_s1(.A(S1), .Y(s1_n));
  
  // Use NAND instead of AND
  nand3 nand0(.A(D0), .B(s1_n), .C(s0_n), .Y(w0_n));
  inv i0(.A(w0_n), .Y(w0));
  
  nand3 nand1(.A(D1), .B(s1_n), .C(S0), .Y(w1_n));
  inv i1(.A(w1_n), .Y(w1));
  
  nand3 nand2(.A(D2), .B(S1), .C(s0_n), .Y(w2_n));
  inv i2(.A(w2_n), .Y(w2));
  
  nand3 nand3_g(.A(D3), .B(S1), .C(S0), .Y(w3_n));
  inv i3(.A(w3_n), .Y(w3));
  
  // Use NOR instead of OR
  nor4 nor_f(.A(w0), .B(w1), .C(w2), .D(w3), .Y(w0_n));
  inv i_final(.A(w0_n), .Y(Y));
endmodule

// Full adder using gates (same as test5550)
module full_adder(A, B, Cin, Sum, Cout);
  input A, B, Cin;
  output Sum, Cout;
  wire xor1, and1, and2, and3;
  
  xor2 x1(.A(A), .B(B), .Y(xor1));
  xor2 x2(.A(xor1), .B(Cin), .Y(Sum));
  
  and2 a1(.A(A), .B(B), .Y(and1));
  and2 a2(.A(xor1), .B(Cin), .Y(and2));
  or2 o1(.A(and1), .B(and2), .Y(Cout));
endmodule

// Priority encoder using De Morgan equivalents
module priority_encoder(req0, req1, req2, req3, req4, req5, req6, req7, grant0, grant1, grant2);
  input req0, req1, req2, req3, req4, req5, req6, req7;
  output grant0, grant1, grant2;
  wire w0, w1, w2, w3, w4, w5, w6;
  wire w_tmp;
  
  // grant[0] - use NOR instead of OR
  nor4 nor0(.A(req1), .B(req3), .C(req5), .D(req7), .Y(w_tmp));
  inv inv0(.A(w_tmp), .Y(grant0));
  
  // grant[1] - same logic
  nor4 nor1(.A(req0), .B(req1), .C(req4), .D(req5), .Y(w0));
  inv inv1(.A(w0), .Y(grant1));
  
  // grant[2] - use NOR instead of OR
  nor4 nor2(.A(req4), .B(req5), .C(req6), .D(req7), .Y(w1));
  inv inv2(.A(w1), .Y(grant2));
endmodule

module TopLevel5551(
  I0, I1, I2, I3, I4, I5, I6, I7, I8,
  O_inv, O_buf,
  O_and2, O_and3, O_and4, O_and5, O_and6, O_and7, O_and8, O_and9,
  O_or2, O_or3, O_or4, O_or5, O_or6, O_or7, O_or8, O_or9,
  O_nand2, O_nand3, O_nand4, O_nand5, O_nand6, O_nand7, O_nand8, O_nand9,
  O_nor2, O_nor3, O_nor4, O_nor5, O_nor6, O_nor7, O_nor8, O_nor9,
  O_xor2,
  O_final1, O_final2,
  O_mux, O_sum, O_cout, O_grant
);
  input I0, I1, I2, I3, I4, I5, I6, I7, I8;
  output O_inv, O_buf;
  output O_and2, O_and3, O_and4, O_and5, O_and6, O_and7, O_and8, O_and9;
  output O_or2, O_or3, O_or4, O_or5, O_or6, O_or7, O_or8, O_or9;
  output O_nand2, O_nand3, O_nand4, O_nand5, O_nand6, O_nand7, O_nand8, O_nand9;
  output O_nor2, O_nor3, O_nor4, O_nor5, O_nor6, O_nor7, O_nor8, O_nor9;
  output O_xor2;
  output O_final1, O_final2;
  output O_mux, O_sum, O_cout;
  output [2:0] O_grant;
  
  wire w_and2, w_and3, w_and4, w_and5, w_and6, w_and7, w_and8, w_and9;
  wire w_or2, w_or3, w_or4, w_or5, w_or6, w_or7, w_or8, w_or9;
  wire w_combine1, w_combine2, w_combine3, w_temp;
  
  // Single input gates - use synonyms
  not g_inv(.A(I0), .Y(O_inv));        // not = inv
  buffer g_buf(.A(I1), .Y(O_buf));     // same
  
  // AND gates = ~NAND (sizes 2-9)
  nand2 g_and2_n(.A(I0), .B(I1), .Y(w_and2));
  inv g_and2_i(.A(w_and2), .Y(O_and2));
  
  nand3 g_and3_n(.A(I0), .B(I1), .C(I2), .Y(w_and3));
  inv g_and3_i(.A(w_and3), .Y(O_and3));
  
  nand4 g_and4_n(.A(I0), .B(I1), .C(I2), .D(I3), .Y(w_and4));
  inv g_and4_i(.A(w_and4), .Y(O_and4));
  
  nand5 g_and5_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(w_and5));
  inv g_and5_i(.A(w_and5), .Y(O_and5));
  
  nand6 g_and6_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(w_and6));
  inv g_and6_i(.A(w_and6), .Y(O_and6));
  
  nand7 g_and7_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(w_and7));
  inv g_and7_i(.A(w_and7), .Y(O_and7));
  
  nand8 g_and8_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(w_and8));
  inv g_and8_i(.A(w_and8), .Y(O_and8));
  
  nand9 g_and9_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(w_and9));
  inv g_and9_i(.A(w_and9), .Y(O_and9));
  
  // OR gates = ~NOR (sizes 2-9)
  nor2 g_or2_n(.A(I0), .B(I1), .Y(w_or2));
  inv g_or2_i(.A(w_or2), .Y(O_or2));
  
  nor3 g_or3_n(.A(I0), .B(I1), .C(I2), .Y(w_or3));
  inv g_or3_i(.A(w_or3), .Y(O_or3));
  
  nor4 g_or4_n(.A(I0), .B(I1), .C(I2), .D(I3), .Y(w_or4));
  inv g_or4_i(.A(w_or4), .Y(O_or4));
  
  nor5 g_or5_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(w_or5));
  inv g_or5_i(.A(w_or5), .Y(O_or5));
  
  nor6 g_or6_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(w_or6));
  inv g_or6_i(.A(w_or6), .Y(O_or6));
  
  nor7 g_or7_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(w_or7));
  inv g_or7_i(.A(w_or7), .Y(O_or7));
  
  nor8 g_or8_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(w_or8));
  inv g_or8_i(.A(w_or8), .Y(O_or8));
  
  nor9 g_or9_n(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(w_or9));
  inv g_or9_i(.A(w_or9), .Y(O_or9));
  
  // NAND gates - direct (sizes 2-9)
  nand2 g_nand2(.A(I0), .B(I1), .Y(O_nand2));
  nand3 g_nand3(.A(I0), .B(I1), .C(I2), .Y(O_nand3));
  nand4 g_nand4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_nand4));
  nand5 g_nand5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_nand5));
  nand6 g_nand6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_nand6));
  nand7 g_nand7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_nand7));
  nand8 g_nand8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_nand8));
  nand9 g_nand9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_nand9));
  
  // NOR gates - direct (sizes 2-9)
  nor2 g_nor2(.A(I0), .B(I1), .Y(O_nor2));
  nor3 g_nor3(.A(I0), .B(I1), .C(I2), .Y(O_nor3));
  nor4 g_nor4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_nor4));
  nor5 g_nor5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_nor5));
  nor6 g_nor6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_nor6));
  nor7 g_nor7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_nor7));
  nor8 g_nor8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_nor8));
  nor9 g_nor9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_nor9));
  
  // XOR gate - same
  xor2 g_xor2(.A(I0), .B(I1), .Y(O_xor2));
  
  // Additional logic using outputs (same as test5550)
  // Combine some AND outputs
  or5 combine1(.A(O_and2), .B(O_and3), .C(O_and4), .D(O_and5), .E(O_and6), .Y(w_combine1));
  
  // Combine some OR outputs
  nand4 combine2(.A(O_or2), .B(O_or3), .C(O_or4), .D(O_or5), .Y(w_combine2));
  
  // Combine some NAND/NOR outputs
  and3 combine3(.A(O_nand2), .B(O_nor2), .C(O_xor2), .Y(w_combine3));
  
  // Final outputs with inverters and buffer
  inv final_inv1(.A(w_combine1), .Y(w_temp));
  buffer final_buf(.A(w_temp), .Y(O_final1));
  // Instantiate submodules to test composite circuits (same as test5550)
  // 4-to-1 MUX
  mux4to1 mux_inst(.D0(I0), .D1(I1), .D2(I2), .D3(I3), .S0(I4), .S1(I5), .Y(O_mux));
  
  // Full adder
  full_adder fa_inst(.A(I0), .B(I1), .Cin(I2), .Sum(O_sum), .Cout(O_cout));
  
  // Priority encoder
  priority_encoder pe_inst(.req0(I0), .req1(I1), .req2(I2), .req3(I3), .req4(I4), .req5(I5), .req6(I6), .req7(I7), .grant0(O_grant[0]), .grant1(O_grant[1]), .grant2(O_grant[2]));
  
  
  inv final_inv2(.A(w_combine2), .Y(O_final2));
  
endmodule
