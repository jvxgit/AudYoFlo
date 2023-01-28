%% Function to decode the PDM bitstream into PCM signals with subsampling factor of 16
% This function exploits the symmetry of the computed impulse response and
% other useful properties. Code is very optimized, in comment, you will
% find a better readible version
function [handle, out, tt] = jvx_pdm_dec_cpu_process_r16(handle, in)
   
    cnt = 1;
    ppcnt = handle.ppcnt;
    
    nP = 0.0;
    % Loop over samples
    for(ind = 1:size(in,2))
        
         p = ind/size(in,2);
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        xn = in(ind);
        
        % This, we should make a little better on a DSP or MCU
        handle.reg = bitshift(handle.reg, -1);
        if(xn > 0.5)
            handle.reg =  bitor(handle.reg, bitshift(1,handle.cfg.bitwidth-1));
        end
        
        % This targets subsampling: collect samples until 16 are ready
        handle.pcnt = mod(handle.pcnt+1,  handle.cfg.bitwidth);
        if(handle.pcnt == 0)
        
            % Take the 16 bits values and store in memory in byte chunks. From lsb to msb
            % means drom older to younger
            val = handle.reg;
            
            for(ind=1:handle.cfg.n_fragments_per_reg-1)
                p = bitand(val, hex2dec('FF'));
                handle.mem.p(ppcnt+1) = p;
                ppcnt = mod(ppcnt + handle.derived.lir_fragments-1, handle.derived.lir_fragments);
                val = bitshift(val, -8);
            end
            
            p = bitand(val, hex2dec('FF'));
            handle.mem.p(ppcnt+1) = p;
            
            accu = 0;
            
            % Circular buffer addressing
    
            % One pointer for forward iterations
            ppcntfwd = ppcnt;
            
            ppcnt = mod(ppcnt + handle.derived.lir_fragments-1, handle.derived.lir_fragments);

            % One pointer for backward iterations
            ppcntbwd = ppcnt;            
            
            % Loop over all 8 samples segments
            % Exploit symmetrie of imüulse response and propagate forward
            % and backward
            for(ind=0:handle.derived.lir_fragments/2-1)
                
                % Forward
                idx = handle.mem.p(ppcntfwd+1);
                tt = handle.lookuptables(ind+1, idx+1);                
                accu = accu + tt;
                ppcntfwd = mod(ppcntfwd + 1, handle.derived.lir_fragments);

                % Backward
                idx = handle.mem.p(ppcntbwd+1);
                tt = handle.lookuptables( ind+1, handle.lookupbitrevtable(idx+1)+1);      
                accu = accu + tt;
                ppcntbwd = mod(ppcntbwd + handle.derived.lir_fragments-1, handle.derived.lir_fragments);
                
            end
           
           % Write output
            out(cnt) = accu;
            cnt = cnt + 1;          

            handle.reg = 0;
        end
    end
    handle.ppcnt = ppcnt ;
        

%         cnt = 1;
%     ppcnt = handle.ppcnt;
%     for(ind = 1:size(in,2))
%         xn = in(ind);
%         if(xn > 0.5)
%             handle.reg =  bitor(handle.reg, hex2dec('10000'));
%         end
%         handle.reg = bitshift(handle.reg, -1);
%         
%         handle.pcnt = mod(handle.pcnt+1,  handle.cfg.bitwidth);
%         if(handle.pcnt == 0)
%             val = handle.reg;
%             p2 = bitand(val, hex2dec('FF'));
%             val = bitshift(val, -8);
%             p1 = bitand(val, hex2dec('FF'));
%             
%             handle.mem.p(ppcnt+1) = p2;
%             ppcnt = mod(ppcnt + handle.derived.lir_fragments-1, handle.derived.lir_fragments);
%             handle.mem.p(ppcnt+1) = p1;
%             accu = 0;
%             for(ind=1:handle.derived.lir_fragments-1)
%                 idx = handle.mem.p(ppcnt+1);
%                 
%                 if(ind > handle.derived.lir_fragments/2)
%                     tt = handle.lookuptables((handle.derived.lir_fragments - ind+1), handle.lookupbitrevtable(idx+1)+1);
%                 else
%                     tt = handle.lookuptables(ind, idx+1);
%                 end
%                 
%                 accu = accu + tt;
%                 ppcnt = mod(ppcnt + 1, handle.derived.lir_fragments);
%             end
%             idx = handle.mem.p(ppcnt+1);
%             accu = accu + handle.lookuptables(ind, idx+1);
%             out(cnt) = accu;
%             cnt = cnt + 1;          
%             handle.reg = 0;
%         end
%     end
%     handle.ppcnt = ppcnt ;
        