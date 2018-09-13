a=[1];
while size(a) ~= 50000, 
  tmp=randi(18300000,1,50070)';
  values=sort(tmp);
  a=unique(values,'rows');
  size(a)
end

fileID = fopen('randomValues.txt','w');
fprintf(fileID,'%d\n',a)
fclose(fileID)
