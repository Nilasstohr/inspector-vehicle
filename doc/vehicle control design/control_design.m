clc
close all
clear
wo = 7.44;
Vo = 11.80;
Io = 36.6/1000;
R = Vo/Io;
%kt = (Vo-Io*R)/wo;
kt = (11.80-0.0366*322)/7.44;
ke = kt;
f = Io*kt/wo;
w_63_3 = wo*0.633;
Tau = 0.05924; % read from data
J = Tau*(f+kt*ke/R);
Ks = kt/(R*f+kt*ke);
a = 1/J*(f+kt*ke/R);
b = kt/(R*J);
fprintf('Io=%.4f A\n' , Io);
fprintf('R=%d\n', R);
fprintf('kt=%.5f\n', kt);
fprintf('ke=%.5f\n', ke);
fprintf('Tau=%.2f\n', w_63_3);
fprintf('f=%.8f\n', f);
fprintf('J=%.8f\n', J);
fprintf('Ks=%.2f\n', Ks);
fprintf('a=%.4f\n', a);
fprintf('b=%.4f\n', b);

% construct transfer function

Tsys = tf([0, b],[1, a])
grid on

figure
%subplot(2,1,1);
%step(Tsys)
%subplot(2,1,2); 
margin(Tsys)

%%

% P controller design
    ess = 0.05;
    kp = (Vo/ess-1)*Ks;
    %kp = 3.5;
    fprintf('kp=%.4f\n',kp)

    % open loop 
    figure
    [Gm,Pm,Wcg,Wc] = margin(Tsys*kp);
    margin(Tsys*kp)
    fprintf('Wc=%.4f\n',Wc)
    
    % closed loop
    figure
    H = kp*Tsys/(1 + kp*Tsys);
    margin(H)
    step(H)

%%
    
% Pi controller design
    Wk = Wc/4;
    tauI = 1/Wk;
    fprintf("tauI=%.4f\n",tauI);
    Tcpi = kp*tf([tauI, 1],[tauI 0]);
   
    % convert pi controller to discrete time
    Ts = 1/1000; % samplerate is chosen to 1kHz
    Tdpi = c2d(Tcpi,Ts,'tustin');
    
    [cb cn]=tfdata(Tdpi,'v');
    
    disp('--------------------------------------------');
    disp('direct realization pi controller coeffiences');
    disp('--------------------------------------------');
    fprintf('b0=%.2f\n', cb(1));
    fprintf('b1=%.2f\n', cb(2));
    fprintf('a1=%.4f\n', cn(2));
    
    %%
    % convert model to discrete time with zero
    Tdsys = c2d(Tcpi,Ts,'zoh');
    
    % open loop continuous time
    figure
    [Gm,Pm,Wcg,Wc] = margin(Tcpi*Tsys);
    margin(Tcpi*Tsys)
    
    % closed loop continuous time
    Hpi = Tcpi*Tsys/(1+Tcpi*Tsys);
    bode(Hpi);
    
%%

% testing the P and Pi seperately
    close all
    omega = 3; % rad/sek;
    hold on
    step(Hpi*omega)%*omega;
    step(H*omega)%*omega;
    hold off
    
    
