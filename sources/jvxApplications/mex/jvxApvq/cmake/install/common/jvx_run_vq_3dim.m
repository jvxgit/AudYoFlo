dim = 3;
delta_steps = 25;

disp(['Dimension = ' num2str(dim) ', steps = ' num2str(delta_steps)]); 
idQ = jvxApvq_m('init', dim, -1, delta_steps);
[numQ1 numQ2] = jvxApvq_m('numq', idQ);

disp(['Number vectors = ' num2str(numQ1)]);
for(ind = 0:numQ1-1)
    [v] = jvxApvq_m('decode', idQ, ind, 0, 0);
    disp(sprintf('%f ', v));
end
jvxApvq_m('terminate', idQ); 