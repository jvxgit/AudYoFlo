function [out] = swapBits(in)
out = int16(in);
prg = 0.1;
for(ind=1:size(in,2))
    inOne = in(ind);
    if(inOne < 0)
        inOne = (double(2^16)+double(inOne));
       
    else 
        inOne = double(inOne);
    end
    outOne = bin2dec(fliplr(dec2bin(inOne, 16)));
    
    if(outOne > 2^15-1)
        outOne = outOne - double(2^16);
    end
    out(ind) = int16(outOne);
    p = ind/size(in,2);
    if(p > prg)
        disp(int16(p*100));
        prg = prg + 0.1;
    end
        
end
        