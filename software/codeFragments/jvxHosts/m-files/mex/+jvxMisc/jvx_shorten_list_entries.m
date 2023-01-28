function [ lst ] = jvx_shorten_list_entries( lst, num_chars_max, num_start)

    num_chars_max = max(num_chars_max, num_start + 3 + 1);
    for(ind=1:length(lst))
        oneentry = lst{ind};
        numchars = length(oneentry);
        if(numchars > num_chars_max)
            oneentry = [oneentry(1:num_start) '...' oneentry(end-(num_chars_max - 3 - num_start):end)];
        end
        lst{ind} = oneentry;
    end
end

