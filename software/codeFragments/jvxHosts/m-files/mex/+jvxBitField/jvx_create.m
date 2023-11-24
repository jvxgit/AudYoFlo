function [selection] = jvx_create(posiBitSet, allSetUpToPosition)
    if(nargin < 2)
		allSetUpToPosition = false;
    end
    if(nargin < 1)
		posiBitSet = -1;
	end
	selection = '';
    if(posiBitSet < 0)
        selection = '00000000';
    else
		if(allSetUpToPosition)
			cnt = 0;
			num32BitValues = ceil((posiBitSet+1)/32);
			for(ind = 1:num32BitValues)
				for(jnd=1:8)
					token = [];
					for(knd=1:4)
						if(cnt <= posiBitSet)
							token = [ '1' token];
						else
							token = [ '0' token];
						end
						cnt = cnt + 1;
					end
					selection = [dec2hex(bin2dec(token),1) selection];
				end
			end
		else
			cnt = 0;
			num32BitValues = ceil((posiBitSet+1)/32);
			for(ind = 1:num32BitValues)
				for(jnd=1:8)
					token = [];
					for(knd=1:4)
						if(cnt == posiBitSet)
							token = [ '1' token];
						else
							token = [ '0' token];
						end
						cnt = cnt + 1;
					end
					selection = [dec2hex(bin2dec(token),1) selection];
				end
			end
		end
    end
    selection = ['0x' selection];
        