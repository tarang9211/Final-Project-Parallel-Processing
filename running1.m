
load ('Eresult2.txt')
A = Eresult(1:450000,:);

B= reshape(A,[10000,90]);

for i=1:45
    scatter(B(1:10000,i),B(1:10000,45+i))
    axis([0,5000,-1000,1000]);
    pause(0.01);
end
%%
figure
subplot(1,2,1)
scatter(B(1:10000,44),B(1:10000,89))
axis([0,5000,-1000,1000]);
title('44th data pt')
subplot(1,2,2)
scatter(B(1:10000,45),B(1:10000,90))
axis([0,5000,-1000,1000])
%%
load ('Trial4.txt')
load ('Iresult4.txt')
I=Iresult4;

C = Trial4(1:60000,:);

D= reshape(C,[1000,120]);

for i=1:60
    scatter(D(1:1000,i),D(1:1000,60+i),'filled')
    hold on
    scatter(I(1:1000,1),I(1:1000,2),'r','filled')
    hold off
    axis([0,200,0,1000]);
    pause(0.2);
end