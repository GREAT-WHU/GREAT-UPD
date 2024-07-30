function upd_days = read_day_upd_files(updmode, files, sysid, ref_sat)

%set satellites
sats=[];
if sysid=='G' && strcmp(updmode,'WL')
    sats = [1,2,3,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32];
end
if sysid=='G' && strcmp(updmode,'EWL')
    sats = [1,3,6,8,9,10,24,25,26,27,30,32];
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


% Read raw data
ndays = length(files);
nsats = length(sats);
upd_days = nan*ones(nsats, ndays);

for iday =1:1:ndays
    fid = fopen(files(iday).path);
    
    while ~feof(fid)
        line = fgetl(fid);
         if (line(1:1)~=' ')
            continue;
         end
    
        sat_id = str2num(line(3:4));
        which_row = find(sats==sat_id);
        upd_days(which_row,iday) = str2num(line(15:22));
    end
    
    fclose(fid);
end

% Minus UPD of reference satellate
for i=1:1:nsats
    if i~=ref_sat
        upd_days(i,:) = upd_days(i,:)-upd_days(ref_sat,:);
    end
end
% Adjust jump caused by subtracting reference satellate
for i=1:1:nsats
    for j=2:1:ndays
        if abs(upd_days(i,j)-upd_days(i,1))>0.6
           jump =  round(upd_days(i,j)-upd_days(i,1));
           upd_days(i,j)=upd_days(i,j)-jump;
        end
    end
end

for i=1:1:nsats
    for j=1:1:ndays
        if upd_days(i,j)>=1
            upd_days(i,j)=upd_days(i,j)-1;
        end
        if upd_days(i,j)<=-1
            upd_days(i,j)=upd_days(i,j)+1;
        end
           
    end
end

end



