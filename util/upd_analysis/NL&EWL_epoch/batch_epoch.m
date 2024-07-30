clear;clc;
yyyy=2019;      
sdoy=106;        
edoy=106;   
ref_sat=2;                                             %ref_sat represents the order of satellites and does not represent the PRN number
sysid='E';      
updmode = 'NL'                                         %options: NL or EWL_epoch
wdir='C:\Users\27878\Desktop\论文结果\results-upd';    %source directory of upd files
sdir='C:\Users\27878\Desktop\论文结果\results\fig';    %save directory

for idoy=sdoy:edoy
    fname=strcat('upd_nl_',num2str(yyyy),num2str(idoy,'%03d'),'_',sysid);  
    %fname=strcat('upd_ewl_',num2str(yyyy),num2str(idoy,'%03d'),'_',sysid);  
    pname=strcat(updmode,'-',num2str(yyyy),num2str(idoy,'%03d'));            %figure name
    filename=strcat(wdir,'\',fname);                                         %the name of upd with absolute path
    X = read_epoch_upd_file( filename, sysid, ref_sat);
    draw_epoch_upd( updmode, X, pname, sdir, ref_sat);
end
