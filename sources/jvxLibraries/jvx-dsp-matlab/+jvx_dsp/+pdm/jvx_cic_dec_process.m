function [handle, out, normFac, tt] = jvx_cic_dec_process(handle, in, normalize, normal_oper)

    % Quickly compute transfer function
    % R=16; N = 1; M = 2; num = [1 zeros(1, R*M-2) -1]; den = [ 1 -1]; numu = 1; denu = 1; for(ind=1:N) numu = conv(numu, num); denu = conv(denu, den); end; figure; plot(impz(numu, denu));
    
    in0 = in;
    
    addf = @wrap_add;
    subf = @wrap_sub;
    
    accu = 0;
    accu2 = 0;
        
    if(nargin < 3)
        normalize = true;
    end
    
    if(nargin >= 4)
        if(normal_oper)
            addf = @normal_add;
            subf = @normal_sub;
        end
    end
    
    display('CIC Integrator Filter Stage');
    nP = 0.0;
    for(ind = 1:length(in))
        
        p = ind/length(in);
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        
        accu = in(ind);
        
        % Integrator
        for(jnd = 1:handle.N)
            accu1 = handle.mem_i(jnd);
            
            accu = addf(accu, accu1, 16);
            %accu = accu + accu1;
            %if(accup ~= int16(accu))
            %    disp(['-> ' num2str(accup) 'vs' num2str((accu))]);
            %end
            handle.mem_i(jnd) = accu;
        end
        
        in(ind) = accu;
    end
    if(nargout >=4)
        tt = in;
    end
      
    %in = in0;
    
    if(handle.runfull)
        lli = length(in);
        display('CIC Comb Filter Stage (full)');
        nP = 0.0;
        for(ind = 0:lli-1)
            
            p = ind/lli;
            if(p>= nP)
                disp([num2str(nP*100) '%']);
                nP = nP + 0.1;
            end
            % Input of second stage
            accu = in(ind+1);
            
            % Comb filter
            cnt = handle.cp;
            accu2 = handle.mem_c(cnt+1);
            %disp(['accu2 = ' num2str(accu2)]);
            
            for(jnd = 1:handle.N-1)
                
                % Comput filter output
                accu1 = subf(accu, accu2, 16);
                %accu1 = accu - accu2;
                %if(accu1p ~= int16(accu1))
                %    disp(['-> ' num2str(accu1p - int16(accu1))]);
                %end
                
                
                handle.mem_c(cnt+1) = accu;
                accu = accu1;

                cnt = cnt + handle.RM;
                accu2 = handle.mem_c(cnt+1);
            end
            
            accu1 = subf(accu, accu2, 16);
            %accu1 = accu - accu2;
            %if(accu1p ~= int16(accu1))
            %        disp(['-> ' num2str(accu1p - int16(accu1))]);
            %    end
            
            handle.mem_c(cnt+1) = accu;
            cnt = cnt + handle.RM;
            
            % Update counter with wrap around
            cnt = mod(cnt + 1, handle.RM);
            out(ind+1) = accu1;
            handle.cp = cnt;
        end
        if(normalize)
            out = out/handle.RNPM;
        end
        if(nargout >= 3)
            normFac = 1/handle.RNPM;
        end

    else
        if(mod(length(in), handle.R))
            error(['Incorrect number of input samples, must be multiple of ' num2str(handle.R)]);
        end
        llo = length(in)/handle.R;
        
        display('CIC Comb Filter Stage (subsampled)');
        nP = 0.0;
        for(ind = 0:llo-1)
            
            p = ind/llo;
            if(p>= nP)
                disp([num2str(nP*100) '%']);
                nP = nP + 0.1;
            end
            % Input of second stage
            accu = in(ind*handle.R+1);
            
            % Comb filter
            cnt = handle.cp;
            accu2 = handle.mem_c(cnt+1);
            
            for(jnd = 1:handle.N-1)
                
                % Comput filter output
                accu1 = subf(accu, accu2, 16);
                %accu1 = accu - accu2;
                %if(accu1p ~= int16(accu1))
                %    disp(['-> ' num2str(accu1p - int16(accu1))]);
                %end
                
                
                handle.mem_c(cnt+1) = accu;
                accu = accu1;
                cnt = cnt + handle.M;
                accu2 = handle.mem_c(cnt+1);
            end
            
            accu1 = subf(accu, accu2, 16);
            %accu1 = accu - accu2;
            %if(accu1p ~= int16(accu1))
            %        disp(['-> ' num2str(accu1p - int16(accu1))]);
            %    end
            
            handle.mem_c(cnt+1) = accu;
            out(ind+1) = accu1;
            cnt = cnt + handle.M;
            cnt = mod(cnt + 1, handle.M);
            handle.cp = cnt;
        end
        if(normalize)
            out = out/handle.RNPM;
        end
        if(nargout >= 3)
            normFac = 1/handle.RNPM;
        end
    end
    
% =======================================================================
function [out] = wrap_add(a, b, numBits)
    out = int16(mod(double(a) + double(b) + 2^(numBits-1), 2^numBits) - 2^(numBits-1));
        
function [out] = wrap_sub(a, b, numBits)
    out = int16(mod(double(a) - double(b) + 2^(numBits-1), 2^numBits) - 2^(numBits-1));

function [out] = normal_add(a, b, numBits)
    out = a + b;
        
function [out] = normal_sub(a, b, numBits)
    out = a - b;
