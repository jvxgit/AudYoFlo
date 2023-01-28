%% Function to decode the PDM bitstream into PCM signals with subsampling factor of 16
% This function exploits the symmetry of the computed impulse response and
% other useful properties. Code is very optimized, in comment, you will
% find a better readible version
function [handle, out, tt] = jvx_pdm_dec_cpu_process_r16_int16le(handle, in, apply_rbit)
   
    use_rbit = false;
    cnt = 1;
    ppcnt = handle.ppcnt;
    
    if(nargin > 2)
        use_rbit = apply_rbit;
    end
    
    numBits = 16;
    if(strcmp(class(in),'uint32'))
        numBits = 32;
    else
        
        % this option only in 32 bit mode
        use_rbit = false;
    end
    
    nP = 0.0;
    % Loop over samples
    for(ind = 1:size(in,2))
        
         p = ind/size(in,2);
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        
        % Take the 16 bits values and store in memory in byte chunks. From lsb to msb
        % means drom older to younger
        val = in(ind);
        if(use_rbit)
            val = rbit(val, numBits);
        end
        
        for(lnd=1:handle.cfg.n_regs_per_value)

            for(iind=1:handle.cfg.n_fragments_per_reg-1)

                %% Each iteration accessing 16 bit register
                p = bitand(val, hex2dec('FF'));
                handle.mem.p(ppcnt+1) = p;
                ppcnt = mod(ppcnt +1, handle.derived.lir_fragments);
                val = bitshift(val, -8);
            end
            
            p = bitand(val, hex2dec('FF'));
            handle.mem.p(ppcnt+1) = p;
            
            accu = 0;
            
            % Circular buffer addressing
            
            % One pointer for forward iterations
            ppcntfwd = ppcnt;
            
            ppcnt = mod(ppcnt +1, handle.derived.lir_fragments);
            val = bitshift(val, -8);
            
            % One pointer for backward iterations
            ppcntbwd = ppcnt;
            
            % Loop over all 8 samples segments
            % Exploit symmetrie of impulse response and propagate forward
            % and backward
            for(iind=0:handle.derived.lir_fragments/2-1)
                
                % Forward
                idx = handle.mem.p(ppcntfwd+1);
                tt = handle.lookuptables_16(iind+1, idx+1);
                accu = accu + tt;
                ppcntfwd = mod(ppcntfwd + handle.derived.lir_fragments-1, handle.derived.lir_fragments);
                
                % Backward
                idx = handle.mem.p(ppcntbwd+1);
                tt = handle.lookuptables_16( iind+1, handle.lookupbitrevtable(idx+1)+1);
                accu = accu + tt;
                ppcntbwd = mod(ppcntbwd + 1, handle.derived.lir_fragments);
                
            end
            
            % Write output
            out(cnt) = accu;
            cnt = cnt + 1;
        end
        
        handle.reg = 0;
    end
    handle.ppcnt = ppcnt ;
    
    function [out] = rbit(in, numBits)
        out = in;
        ins = dec2bin(in, numBits);
        outs = fliplr(ins);
        out = bin2dec(outs);
        