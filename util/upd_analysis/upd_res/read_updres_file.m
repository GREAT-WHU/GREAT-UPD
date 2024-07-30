function [ wl_res, nl_res ] = read_updres_file( updres_file )
%READ_UPDRES_FILE Summary of this function goes here
%   Detailed explanation goes here

wl_res=[];
nl_res=[];

fid = fopen(updres_file);
count =0;
while ~feof(fid)
    line = fgetl(fid);
    count = count+1;
    if mod(count,100000)==0
        count
    end
    if findstr(line,'EPOCH-TIME')
        continue;
    end
    
   %%------------------Format---------------------%%
    % "  ABMF  G05    -0.010 yes     -0.055 yes "
    if findstr(line(23:25),'yes')
       res_w = str2num(line(14:21));
       wl_res = [wl_res; res_w];
    end
    
    if findstr(line(38:40),'yes')
        res_r =str2num(line(29:36));
        nl_res = [nl_res; res_r];
    end
   %%------------------Format---------------------%% 
   
    
end
fclose(fid);

end

