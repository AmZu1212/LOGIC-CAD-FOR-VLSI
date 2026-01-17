module HA (A, B, S, C);

  input   A;
  input   B;
  output   S;
  output   C;
  
nor I1(A, B, S);
nor I2(C, A, B);

endmodule /* HA */

module FA (A, B, Cin, Sum, Cout);

  input   A;
  input   B;
  input   Cin;
  output   Sum;
  output   Cout;
  
  wire   n1;
  wire   n2;
  wire   n3;
  
HA I1(A, B, n1, n2);
HA I2(Cin, n1, Sum, n3);
nor I3(Cout, n2, n3);

endmodule /* FA */

module TopLevel1405 (A, B, Sum, Cout);

  input[0:2]   A;
  input[0:2]   B;
  output[0:2]   Sum;
  output   Cout;
  
  wire   n1;
  wire   n2;
  
HA I1(A[0], A[0], Sum[0], n1);
FA I2(A[1], A[1], n1 ,Sum[1], n2);
FA I3(A[2], A[2], n2, Sum[2], Cout);

endmodule /* ADDER3bit */