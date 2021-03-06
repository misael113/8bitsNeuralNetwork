%Prueba splines: proyecto Zumo32U4
%Misael Baeza Padilla

clear all, clc;
x  = [-8:0.001:8];     nl = length(x);

for i = 1:nl

 lambda_2 = -8;
 lambda_1  = -4;
 lambda0  = -1;
 lambda1  =0.5;
 lambda2  = 4;
 lambda3  = 8;

%  
  if x(i)>=lambda_2 && x(i)<lambda_1
       N1_0(i) = 1;
    else
       N1_0(i) = 0;
   end
%  
  if x(i)>=lambda_1 && x(i)<lambda0
      N1_1(i) = 1;
   else
      N1_1(i) = 0;
  end
%  
  if x(i)>=lambda0 && x(i)<lambda1
      N1_2(i) = 1;
   else
      N1_2(i) = 0;
  end
%  
   if x(i)>=lambda1 && x(i)<lambda2
      N1_3(i) = 1;
   else
      N1_3(i) = 0;
   end
% 
    if x(i)>=lambda2 && x(i)<lambda3
      N1_4(i) = 1;
   else
      N1_4(i) = 0;
    end
    
     
     %
 N2_1(i)   = ((x(i)-lambda_2)./(lambda_1-lambda_2)).*N1_0(i) + ((lambda0 - x(i))/(lambda0 - lambda_1))*N1_1(i);
 N2_3(i)   = ((x(i)-lambda_1)./(lambda0 - lambda_1)).*N1_1(i) + ((lambda1-x(i))./(lambda1-lambda0)).*N1_2(i);
 N2_4(i)   = ((x(i)-lambda0)./(lambda1-lambda0)).*N1_2(i) + ((lambda2-x(i))./(lambda2-lambda1)).*N1_3(i);
 N2_5(i)   = ((x(i)-lambda1)./(lambda2-lambda1)).*N1_3(i) + ((lambda3-x(i))./(lambda3-lambda2)).*N1_4(i);
 
end

%graficas de las b-splines
figure(1)                                
hold on, 
plot(x,N2_1, x,N2_3, x,N2_4, x, N2_5);
