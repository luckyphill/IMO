fid = fopen('death_test.txt', 'w');

runs = 100;
id = 0.1:0.025:0.5;
bd = 0.02:0.02:0.1;

for i = 1:runs
    for j = id
        for k = bd
            fprintf(fid,'%1.2f, %1.2f,%d\n',k,j,i);
        end
    end
end