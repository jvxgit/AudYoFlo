h5create("myfile.h5","/DS1/ds/a",[1 3]);
h5write("myfile.h5","/DS1/ds/a", ones(1,3));

% Run "h5dump.exe myfile.h5" in library build to decode the contents