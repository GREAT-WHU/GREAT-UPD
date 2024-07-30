function draw_updres( updres,pname,figsavepath,xlabelname)
%DRAW_UPDRES Summary of this function goes here
%   Detailed explanation goes here

count_15=0;
count_25=0;
for i=1:length(updres)
    if(abs(updres(i,1))<=0.15)
        count_15=count_15+1;%%within 0.15 weeks
    end
    if(abs(updres(i,1))<=0.25)
        count_25=count_25+1;%%within 0.25 weeks
    end
end
percentage_15=count_15/length(updres)*100;
percentage_25=count_25/length(updres)*100;
binranges = [-0.5:0.05:0.5];
bincounts = histc(updres, binranges);
figure();
bar(binranges,bincounts/length(updres)*100,'histc');
mean_res=roundn(mean(updres),-4);
words=strcat('aveSTD:',num2str(mean_res),'Within 0.15 Cycles:',num2str(percentage_15),'%','Within 0.25 Cycles:',num2str(percentage_25),'%');
gtext(words);
xlabel(xlabelname,'FontSize',10);
ylabel('Percentage(%)','FontSize',10);
title(pname);
set(gca,'FontSize',10,'FontName','Arial');    

savename=strcat(figsavepath,'\',pname,'.fig');
saveas(gcf,savename);
end

