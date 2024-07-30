clear;clc;
yyyy=2019;
sdoy=091;  
edoy=120;
ref_sat=5;                                              %ref_sat represents the order of satellites and does not represent the PRN number
sysid='G';
updmode = 'WL'                                          %options: WL or EWL
wdir='C:\Users\27878\Desktop\论文结果\results-upd';     %source directory of upd files
sdir='C:\Users\27878\Desktop\论文结果\fig';             %save directory

% File paths
files = [];
for idoy=1:1:length(sdoy:edoy)
    fname=strcat('upd_wl_',num2str(yyyy),num2str(sdoy+idoy-1,'%03d'),'_',sysid); 
    files(idoy).path = [wdir,'\',fname];
end

% Read files
X = read_day_upd_files(updmode, files, sysid, ref_sat);

% Drawing
pname=strcat(updmode,'-',num2str(yyyy),num2str(sdoy,'%03d'),'-',num2str(yyyy), num2str(edoy,'%03d'));  %figure name
draw_day_upd(updmode, sdoy, ref_sat, X, pname, sdir)

