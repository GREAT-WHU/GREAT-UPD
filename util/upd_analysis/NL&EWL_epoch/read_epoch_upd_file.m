function upd_epoch = read_epoch_upd_file( filename, sysid, ref_sat)
sats=[];
if sysid=='G'
    sats = [1,2,3,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32];
end
if strcmp(sysid,'R_LEICA')||strcmp(sysid,'R_TRIMBLE')||strcmp(sysid,'R_JAVAD')
     sats = [1,2,3,4,5,7,8,9,10,11,13,14,15,16,17,18,19,20,21,22,23,24];
end
if sysid =='E'
    sats = [1,2,3,4,5,7,8,9,11,12,13,14,15,18,19,21,24,25,26,27,30,31,33,36];
end
if sysid =='C'
    %sats = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,31,32,33,34];
    %sats = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,19,20,27,28,21,22,29,30,23,24,26,25,32,33,35,34];
    %sats = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,16];
    sats = [6,7,8,9,10,11,12,13,14,16];
end


nsats = length(sats);
upd_epoch = nan*ones(nsats, 2881);

fid= fopen(filename,'r');

mjd = 0;
sod = 0;
while ~feof(fid)
    line = fgetl(fid);
    if findstr(line,'%')
        continue;
    end
    if findstr(line,'EPOCH-TIME')
        mjd = str2num(line(15:19));
        sod = str2num(line(20:29));
        if (sod==86400.0)
            break;
        end
        continue;
    end
    which_col = round(sod/30+1);
    if (line(1:1)~=' ')
        continue;
    end
    sat_id = str2num(line(3:4));
    which_row = find(sats==sat_id);
    upd_epoch(which_row, which_col) = str2num(line(16:22));
end

% Minus the UPD of reference satellate
for i=1:1:nsats
    if i~=ref_sat
        upd_epoch(i,:) = upd_epoch(i,:)-upd_epoch(ref_sat,:);
    end
end
% Adjust jump caused by subtracting reference satellate
[row,col]=size(upd_epoch);
for i=1:1:nsats
    for j=2:1:col
        if abs(upd_epoch(i,j)-upd_epoch(i,1))>0.6
           jump =  round(upd_epoch(i,j)-upd_epoch(i,1));
           upd_epoch(i,j)=upd_epoch(i,j)-jump;
        end
    end
end
 
fclose(fid);
end
 
