%plot EWL/WL/NL residuals and statistic the percentages of residuals within ±0.15 cycles and ±0.25 cycles 

wdir='C:\Users\27878\Desktop\论文结果\res-result';        %res file directory
sdir='C:\Users\27878\Desktop\论文结果\res-result';        %fig directory
if ~exist(sdir) 
    mkdir(sdir);
end 
yyyy=2019;
sdoy=094;
edoy=094;
for idoy=sdoy:edoy
	fname=strcat('res_',num2str(yyyy),num2str(idoy,'%03d'));  
	filename=strcat(wdir,'\',fname); 
    fid=fopen(filename);
    if(fid==-1)
        fclose(fid);
        continue;
    end
    fclose(fid);
    
	[ wl_res, nl_res ] = read_updres_file( filename ); 
    pname=strcat('WL-',num2str(yyyy),num2str(idoy,'%03d'));  %figure name
    xlabelname='WL Residuals(cycle)';
	draw_updres( wl_res,pname,sdir,xlabelname)
    pname=strcat('NL-',num2str(yyyy),num2str(idoy,'%03d'));  %figure name
    xlabelname='NL Residuals(cycle)';
	draw_updres( nl_res,pname,sdir,xlabelname)
end
