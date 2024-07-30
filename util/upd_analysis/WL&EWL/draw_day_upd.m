function draw_day_upd( updmode, start, ref_sat, X, pic_title, figsavepath)

 X=X';
[ndays , nsats ] =size(X);

lgds = [];
if nsats==31 && strcmp(updmode,'WL')
    lgds = ['G01';'G02';'G03';'G05';'G06';'G07';'G08';'G09';'G10';'G11';'G12';'G13';'G14';'G15';'G16';'G17';'G18';'G19';'G20';'G21';'G22';'G23';'G24';'G25';'G26';'G27';'G28';'G29';'G30';'G31';'G32'];
end
if nsats==12 && strcmp(updmode,'EWL')
    lgds = ['G01';'G03';'G06';'G08';'G09';'G10';'G24';'G25';'G26';'G27';'G30';'G32'];
end
if nsats==22
    lgds = ['R01';'R02';'R03';'R04';'R05';'R07';'R08';'R09';'R10';'R11';'R13';'R14';'R15';'R16';'R17';'R18';'R19';'R20';'R21';'R22';'R23';'R24'];
end
if nsats==24
    lgds = ['E01';'E02';'E03';'E04';'E05';'E07';'E08';'E09';'E11';'E12';'E13';'E14';'E15';'E18';'E19';'E21';'E24';'E25';'E26';'E27';'E30';'E31';'E33';'E36'];
end
if nsats==18
   lgds = ['C01';'C02';'C03';'C04';'C05';'C06';'C07';'C08';'C09';'C10';'C11';'C12';'C13';'C14';'C31';'C32';'C33';'C34'];
end
if nsats==15
   lgds = ['C01';'C02';'C03';'C04';'C05';'C06';'C07';'C08';'C09';'C10';'C11';'C12';'C13';'C14';'C16'];
end
 if nsats==10
   lgds = ['C06';'C07';'C08';'C09';'C10';'C11';'C12';'C13';'C14';'C16'];
end
if nsats==30
   lgds = ['C01';'C02';'C03';'C04';'C05';'C06';'C07';'C08';'C09';'C10';'C11';'C12';'C13';'C14';'C19';'C20';'C27';'C28';
       'C21';'C22';'C29';'C30';'C23';'C24';'C26';'C25';'C32';'C33';'C35';'C34';];
end

% Remove UPD of reference satellate
X(:,ref_sat)=[];
lgds(ref_sat,:)=[];
nsats=nsats-1;

%Drawing time series graph
nline=8;
nfigures = ceil((nsats-1)/nline);
for i=1:1:nfigures
    begin = (i-1)*nline+1;
    finish = i*nline;
    if i==nfigures
        finish=nsats;
    end

    figure;
    index = 0;
    for j=begin:1:finish
        index=index+1;
        switch index 
            case 1
                plot(X(:,j),'-s','color',[0,0,0],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[0,0,0] );
            case 2
                plot(X(:,j),'-o','color',[1,0,0],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[1,0,0] );
            case 3
                plot(X(:,j),'-^','color',[0,0,1],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[0,0,1] );
            case 4
                plot(X(:,j),'-v','color',[0,0.5,0.5],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[0,0.5,0.5] );
            case 5
                plot(X(:,j),'-<','color',[1,0,1],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[1,0,1] );
            case 6
                plot(X(:,j),'->','color',[0.5,0.5,0],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[0.5,0.5,0] );
            case 7
                plot(X(:,j),'-d','color',[0,0,0.5],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[0,0,0.5] );
            case 8
                plot(X(:,j),'-p','color',[1,0.5,0],'LineWidth',2,'MarkerSize', 5, 'MarkerFaceColor',[1,0.5,0] );
        end
        hold on;
    end
    hold off;
    ylabeltext=strcat(updmode,' UPD (Cycle)');            %figure name
    ylabel(ylabeltext,'FontSize',10);
    xlabel('DOY');
    set(gca,'YLim',[-1, 1]);
    set(gca,'XLim',[1,ndays]);
    set(gca,'YTick',-0.8:0.2:0.8);
    set(gca,'XTickLabel',num2str((start:1:start+ndays-1).'));
    set(gca,'FontSize',10,'FontName','Arial');
    
    legend_tick=[];
    if i== nfigures
        for j=(i-1)*nline+1:1:nsats
            legend_tick=[legend_tick; lgds(j,:)];
        end
    else
        for k=(i-1)*nline+1:i*nline
            legend_tick=[legend_tick;lgds(k,:)];
        end
    end
    legend({legend_tick},'FontSize',10,'Orientation','horizontal'); 
    % save figure
%     savename=strcat(figsavepath,'\',pic_title,'(',legend_tick(1,:),'-',legend_tick(end,:),').fig');
%     saveas(gcf,savename);
end


%Drawing std graph 
std_upd=[];
for i=1:nsats
    std_upd(i)=nanstd(X(:,i));  
    var(i)=max(X(:,i))-min(X(:,i));
end

mean(std_upd)

figure;
bar(std_upd,'group','FaceColor','b');
grid on;
hold on;
set(gca,'XLim',[0, nsats+1]);
set(gca,'YLim',[0, 0.3]);
set(gca,'XTick',1:1:nsats);
set(gca,'xticklabel',lgds);
set(gca,'YTick',0:0.05:0.3);
% Set the position, angle and font size of the new display label
xtb = get(gca,'XTickLabel');
xt = get(gca,'XTick');
yt = get(gca,'YTick');      
xtextp=xt;                     
ytextp=-0.3*yt(2)*ones(1,length(xt));
text(xtextp,ytextp,xtb,'HorizontalAlignment','right','rotation',30,'fontsize',10); 
% Hide the original label
set(gca,'xticklabel','');
ylabel('STD(Cycles)','FontSize',10);
set(gca,'FontSize',10,'FontName','Arial');
% save figure
% savename=strcat(figsavepath,'\',pic_title,'-STD.fig');
% saveas(gcf,savename);


end

