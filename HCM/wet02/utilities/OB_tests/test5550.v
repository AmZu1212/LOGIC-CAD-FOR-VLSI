// Test circuit 5550 - Comprehensive gate test
// Tests all gate types from stdcell.v (sizes 2-9)
// Should be equivalent to test5551.v

// 4-to-1 MUX using AND/OR gates
module mux4to1(D0, D1, D2, D3, S0, S1, Y);
  input D0, D1, D2, D3;
  input S0, S1;
  output Y;
  wire s0_n, s1_n;
  wire w0, w1, w2, w3;
  
  inv inv_s0(.A(S0), .Y(s0_n));
  inv inv_s1(.A(S1), .Y(s1_n));
  
  and3 and0(.A(D0), .B(s1_n), .C(s0_n), .Y(w0));
  and3 and1(.A(D1), .B(s1_n), .C(S0), .Y(w1));
  and3 and2(.A(D2), .B(S1), .C(s0_n), .Y(w2));
  and3 and3_g(.A(D3), .B(S1), .C(S0), .Y(w3));
  
  or4 or_final(.A(w0), .B(w1), .C(w2), .D(w3), .Y(Y));
endmodule

// Full adder using gates
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

// Priority encoder using larger gates
module priority_encoder(req0, req1, req2, req3, req4, req5, req6, req7, grant0, grant1, grant2);
  input req0, req1, req2, req3, req4, req5, req6, req7;
  output grant0, grant1, grant2;
  wire w0, w1, w2, w3, w4, w5, w6;
  
  // grant[0] - check odd positions
  or4 or0(.A(req1), .B(req3), .C(req5), .D(req7), .Y(grant0));
  
  // grant[1] - complex logic
  nor4 nor1(.A(req0), .B(req1), .C(req4), .D(req5), .Y(w0));
  inv inv1(.A(w0), .Y(grant1));
  
  // grant[2] - high nibble
  or4 or2(.A(req4), .B(req5), .C(req6), .D(req7), .Y(grant2));
endmodule

module TopLevel5550(
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
  
  wire w_combine1, w_combine2, w_combine3, w_temp;
  
  // Single input gates
  inv g_inv(.A(I0), .Y(O_inv));
  buffer g_buf(.A(I1), .Y(O_buf));
  
  // AND gates (2-9)
  and2 g_and2(.A(I0), .B(I1), .Y(O_and2));
  and3 g_and3(.A(I0), .B(I1), .C(I2), .Y(O_and3));
  and4 g_and4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_and4));
  and5 g_and5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_and5));
  and6 g_and6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_and6));
  and7 g_and7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_and7));
  and8 g_and8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_and8));
  and9 g_and9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_and9));
  
  // OR gates (2-9)
  or2 g_or2(.A(I0), .B(I1), .Y(O_or2));
  or3 g_or3(.A(I0), .B(I1), .C(I2), .Y(O_or3));
  or4 g_or4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_or4));
  or5 g_or5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_or5));
  or6 g_or6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_or6));
  or7 g_or7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_or7));
  or8 g_or8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_or8));
  or9 g_or9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_or9));
  
  // NAND gates (2-9)
  nand2 g_nand2(.A(I0), .B(I1), .Y(O_nand2));
  nand3 g_nand3(.A(I0), .B(I1), .C(I2), .Y(O_nand3));
  nand4 g_nand4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_nand4));
  nand5 g_nand5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_nand5));
  nand6 g_nand6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_nand6));
  nand7 g_nand7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_nand7));
  nand8 g_nand8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_nand8));
  nand9 g_nand9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_nand9));
  
  // NOR gates (2-9)
  nor2 g_nor2(.A(I0), .B(I1), .Y(O_nor2));
  nor3 g_nor3(.A(I0), .B(I1), .C(I2), .Y(O_nor3));
  nor4 g_nor4(.A(I0), .B(I1), .C(I2), .D(I3), .Y(O_nor4));
  nor5 g_nor5(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .Y(O_nor5));
  nor6 g_nor6(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .Y(O_nor6));
  nor7 g_nor7(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .Y(O_nor7));
  nor8 g_nor8(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .Y(O_nor8));
  nor9 g_nor9(.A(I0), .B(I1), .C(I2), .D(I3), .E(I4), .F(I5), .G(I6), .H(I7), .I(I8), .Y(O_nor9));
  
  // XOR gate
  xor2 g_xor2(.A(I0), .B(I1), .Y(O_xor2));
  
  // Additional logic using outputs to ensure they're all properly constrained
  // Combine some AND outputs
  or5 combine1(.A(O_and2), .B(O_and3), .C(O_and4), .D(O_and5), .E(O_and6), .Y(w_combine1));
  
  // Combine some OR outputs
  nand4 combine2(.A(O_or2), .B(O_or3), .C(O_or4), .D(O_or5), .Y(w_combine2));
  
  // Combine some NAND/NOR outputs
  and3 combine3(.A(O_nand2), .B(O_nor2), .C(O_xor2), .Y(w_combine3));
  
  // Final outputs with inverters and buffer
  inv final_inv1(.A(w_combine1), .Y(w_temp));
  buffer final_buf(.A(w_temp), .Y(O_final1));
  // Instantiate submodules to test composite circuits
  // 4-to-1 MUX
  mux4to1 mux_inst(.D0(I0), .D1(I1), .D2(I2), .D3(I3), .S0(I4), .S1(I5), .Y(O_mux));
  
  // Full adder
  full_adder fa_inst(.A(I0), .B(I1), .Cin(I2), .Sum(O_sum), .Cout(O_cout));
  
  // Priority encoder
  priority_encoder pe_inst(.req0(I0), .req1(I1), .req2(I2), .req3(I3), .req4(I4), .req5(I5), .req6(I6), .req7(I7), .grant0(O_grant[0]), .grant1(O_grant[1]), .grant2(O_grant[2]));
  
  
  inv final_inv2(.A(w_combine2), .Y(O_final2));
  
endmodule
