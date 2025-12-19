mw = 0.05; % mass of 2 wheels 
m = 0.25; % mass of the upper body
L = 0.05; % distance
I = m * L^2; % inertia of the body part

g = 9.8;

A1 = ((m + mw) * m * g * L) / ((m + mw) * I + m * mw * L^2);
B1 = (m * L) / ((m + mw) * I + m * mw * L^2);
A2 = (m^2 * L^2 * g) / ((m + mw) * I + m * mw * L^2);
B2 = (m * L^2 + I) / ((m + mw) * I + m * mw *L^2);

A = [0 1 0 0;
    A1 0 0 0;
    0 0 0 1;
    -A2 0 0 0];
B = [0;-B1;0;B2];
Q = [200 0 0 0;
    0 1 0 0;
    0 0 300 0;
    0 0 0 1];
R = 1;
Ts = 0.002;
[Kd,S,e] = lqrd(A, B, Q, R, Ts);
Kd