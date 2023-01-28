%% Script to demonstrate the signal processing model for digital mems microphones
% Here are some comments on this topic:
% 1) Typically, the PDM signals have a high SNR for lower frequencies but
% SNR starts to fall for higher frequencies. The test signals I processed
% have a ramp up of noise starting from 20 kHz. At 90 kHz, the noise floor
% is ca 15 dB higher than at 20 kHz. Sensitivity however does not change
% over frequency. As a conclusion, we may reduce the level at higher
% frequencies to get a constant noise profile - which will reduce the 
% signal unfortunately. I would want to retain a constant level over
% frequency and accept a higher noise level.
% 2) The CIC filter is a really badly designed anti aliasing filter. It 
% suffers from backfolding of anti aliasing signals and has a significant 
% roll-off for higher frequencies. However, it has one advantage: it avoids 
% fold back of quantization noise from really high frequencies. This is of
% really high importance since the noise increases up to 60 dB higher than
% at 20 kHz for a frequency of 200 kHz and stays high for higher
% frequencies. When used in US frequency area, the signal is really
% suffering fro roll off and noise backfolding.
% 3) The FIR byte aligned filter is a good solution since it designs a
% filter which is optimal for the application. This filter design, however,
% is of high importance: Typically, FIR designs are rather constant in the
% stop band area. In this stop band area, however, the noise level
% increases by 60 dB!! Hence, if we have a stopband attenuation of 60 dB,
% the noise at higher frequencies is attenuated by 60 dB and reaches the
% same level as that at 20 kHz. When resampling, that noise folds back into
% the audible frequency areas and reduces the perceived audio quality!
% Therefore, we need at least 80 dB of attenuation if stopband in constant.
% A design which falls down for higher frequencies is a good solution!
function [out_dec_ds] = jvx_pdm_run(in, fsin, decodeFirA, p1, p2, lsbfirstArg)

% =======================================
% Some processing parameters
% =======================================

% Activate plotting
doplot = true;
plotIn = true;

% ================================================
% Decode using either CIC filter or FIR filter
% ================================================
decodeFir = true;

% ================================================
% Simulation sample rate
% ================================================
%fs0 = 192000;
fs0 = 48000;
lsbFirst = true;

% ================================================
% FIR filter design rules
% 1: "filtercls1"; 
% 2: "filterls" <- preferred since it allows to define a roll-off and
% slightly reduces magnitude for increasing frequencies
% 3: Just use the CIC filter impulse response
% else: "firpm"
% ================================================
filterdesign = 2;

% For Filter design 1 ("filtercls1")
ATTEN = 120;

% For filter design 2 and "else" to define a roll-off in case of shorter 
% filter IRs ( do not forget: we need 80 dB magnitude suppresion!)
FIRDESIGN_FAC = 4;

% Specification of filter length (in non-pdm domain (e.g. 192 kHz)
ppfilterlength = 32;

% Generate C headers to use the byte aligned implementation
generateCHeader = true;

% Simulate byte aligned decoder
doCpuFir = true;

% Run FIR decoder in 16 bit fixed point
runIn16Bit = true;

input32BitRegisters = true;

% ================================================
% Configuration of CIC decoder filter
% ================================================
os = 16;
N = 3; %3
M = 1;

% ================================================
% Sigma delta model (works only for order = 1)
% ================================================
SIGDELORDER = 1;
doDither = false;
ditherFac = 0.01;
lsecs = 1;

% ================================================
% Misc parameters
% ================================================
FFTSIZE = 8192;

% ================================================
% ================================================
% ================================================
% Copy the command line options if present
% ================================================
% ================================================
% ================================================

if(nargin >= 3)
    decodeFir = decodeFirA;
end

if(nargin >= 4)
    ppfilterlength = p1;
    N = p1;
end
if(nargin >= 5)
    FIRDESIGN_FAC = p2;    
    M = p2;
end

if(nargin >= 2)
    fs0 = fsin;
end

if(nargin >= 6)
    lsbFirst = lsbfirstArg;
end
% ================================================
% Derive some parameters
% ================================================
fs = fs0 * os;

% ================================================
% Either produce your own PDM input signal or take the
% input signal. The PDM generator does not work perfectly as a 
% higher order PDM generator is not simple
% ================================================
if(nargin == 0)
    
    f = 80000;
    f = 2000;
    xmin = 1;
    xmax = ceil(fs/f)+1;

    ll = lsecs * fs;

    %if(strcmp(signaltype,
    sig = 0.9 * sin(2*pi*f/fs * [0:ll-1]);

    % ==============================
    % PDM Encoder
    % ==============================
    %out = jvx_dsp.pdm.jvx_sigmadelta_pdm(sig, fs, doDither, ditherFac);
    out = jvx_dsp.pdm.jvx_sigmadelta_pdm_ho(sig, fs, doDither, SIGDELORDER, ditherFac);
    
    if(doplot)
        figure;
        plot(sig(xmin:xmax), 'r');
        hold on;
        plot(out(xmin:xmax), ':');
        ylim([-1.2;1.2]);
        
        specin = fft(sig);
        specout = fft(out);
        ffts = size(specin,2);
        
        specin = specin(1:ffts/2+1);
        specout = specout(1:ffts/2+1);
        
        specx = [0:ffts/2]/(ffts) * fs;
        
        figure;
        plot(specx, 10*log10(specin.*conj(specin)), 'r');
        hold on;
        plot(specx, 10*log10(specout.* conj(specout)), ':');
    end %if(doplot)
    sig_ds = sig(1:os:end);

else
    
    numBits = 16;
    if(strcmp(class(in), 'uint32') || strcmp(class(in), 'int32'))
        numBits = 32;
    end
    
    % The PDM input is given in 16 Bit format. Therefore, we need to 
    % serialize the inout into a stream of +-1
    disp('Serialize input');
    out = serialize(in, numBits, lsbFirst);
    
    %disp('Pack input');
    %inP = pack(out, 16, true);
    
    % We may add some additional signals to study filtering and
    % anti-aliasing
    %out = out + 0.1 * sin(2*pi*100000/fs*[0:length(out)-1]);
    %out = out + 0.1 * sin(2*pi*80000/fs*[0:length(out)-1]);
    
    plotIn = false;
end

% ==============================
% ==============================
% ==============================
% ==============================
% ==============================
% PDM Decoder
% Create low pass reconstruction filter, version 1
% ==============================
% ==============================
% ==============================
% ==============================
% ==============================
% ==============================

if(decodeFir)
    
    % ========================================    
    % PDM Decoder using an FIR filter
    % ========================================
    
    % Design filter IR
    delta_filter = 0.4*1/2/os;

    % Compue effective filter length
    efffilterlength = os*ppfilterlength; 

    % ========================================
    % ========================================
    % The filter designs
    % ========================================
    % ========================================
    if(filterdesign == 1)
        
        om = 1/os - delta_filter/2;
        % om = 2*90000/fs;
        disp(['Design 1: Input parameter: om = ' num2str(om*(fs/2))]);
        [ir] = fircls1(efffilterlength-1, om, 1, 10^(-ATTEN/20));
        dsigntoken = ['_d1_ll' num2str(efffilterlength) '_att' num2str(ATTEN) '_os' num2str(os)];
        
    elseif(filterdesign == 2)
        
        delta_filter = delta_filter * FIRDESIGN_FAC;
        f = [0 1/os - delta_filter/2 1/os 1];
        a = [1  1  0 0];
        disp(['Design 2: Input parameter: om = ' num2str(f*(fs/2))]);     
        dsigntoken = ['_d2_ll' num2str(efffilterlength) '_fac' num2str(FIRDESIGN_FAC) '_os' num2str(os)];
        ir = firls(efffilterlength-1,f,a);
        
    elseif(filterdesign == 3)
        
        os = 16;
        N = 3;
        M = 2;
        ir = calc_ir_cic(os, N, M);
        error('Not complete');
        % Would need to modify this to become a symmetric implementation!
    else
        delta_filter = delta_filter * FIRDESIGN_FAC;
        % Frequency band edges
        %f = [0 1/os - delta_filter/2 1/os + delta_filter/2 1];
        f = [0 1/os - delta_filter/2 1/os 1];
        disp(['Design 4: Input parameter: om = ' num2str(f*(fs/2))]);        
        % Desired amplitudes
        a = [1  1  0 0];
        ir = firpm(efffilterlength-1,f,a);
    end
   
    if(doplot)
        figure;
        freqz(ir, 1, 1024, fs)
    end        

    % Filter serialized input signal (high complexity)
    out_dec = filter(ir,1,out);
    
    if(doplot)
        [specin, ffts] = freqz(out, 1, FFTSIZE, fs);
        [specout, ffts] = freqz(out_dec, 1, FFTSIZE, fs);
                       
        show1 = 10*log10(specin.*conj(specin));
        show2 = 10*log10(specout.* conj(specout));
        
        figure;
        plot(ffts, show1, 'r+');
        hold on;
        plot(ffts, show2, 's:');
    end

    % Sub sample the filtered input
    out_dec_ds = out_dec(1:os:end);
    
    outpp = out;
    outpp(find(out < 0)) = 0;
    
    if(1)
        out_decp = filter(ir,1,out);
        out_decp_ds = out_decp(os:os:end);
    
        if(doCpuFir)
           [hdl] = jvx_dsp.pdm.jvx_pdm_dec_cpu_init_r16(efffilterlength, true, ir, generateCHeader, dsigntoken);
           
            disp('FIR Decoder (byte aligned implementation');
            if(runIn16Bit)
                
                if(input32BitRegisters)
                    %% inxx = pack(outpp, 32, true);
                    %% inxxx = rbit(typecast(in, 'uint32'), 32);
                    %% inxxx = pack(outpp, 32, false);
                    
                    hdl.cfg.bitwidth = 32;
                    hdl.cfg.n_regs_per_value = 2;
                
                    [hdl, out_decp_ds1] =  jvx_dsp.pdm.jvx_pdm_dec_cpu_process_r16_int16le(hdl, typecast(in, 'uint32'), true);

                else
                    inxx = pack(outpp, 16, true);
                    [hdl, out_decp_ds1] =  jvx_dsp.pdm.jvx_pdm_dec_cpu_process_r16_int16le(hdl, inxx);
                end
                % 
                out_decp_ds1 = double(out_decp_ds1)/2^15;
            else
                
                % Organizes the bit layouts internally
                [hdl, out_decp_ds1] =  jvx_dsp.pdm.jvx_pdm_dec_cpu_process_r16(hdl, outpp);
            end
            
            figure; 
            subplot(2,1,1);
            plot(out_decp_ds1(length(ir)/16+1:end)); hold on; plot(out_decp_ds(length(ir)/16+1:end), 'r:');
            subplot(2,1,2);
            plot(out_decp_ds1(length(ir)/16+1:end)-out_decp_ds(length(ir)/16+1:end), 'g:');
        
            % Warning: The CPU implementation deviates from the original filter
            % implementation during the first samples. That is since the memory
            % of the PDM decoder is filled with 0s at the beginning. These
            % "zeros" are equivalent to -1 values in PDM signals. After the
            % memory has been initially filled, however, the output is
            % identical!!
            out_dec_ds = out_decp_ds1;
            
            %out_dec_ds = out_decp_ds;
        else
            out_dec_ds = out_decp_ds;
        end
    end 
else
    
    % ========================================
    % Implementation of CIC filter
    % ========================================
    ir = calc_ir_cic(os, N, M);
    if(doplot)
        figure;
        freqz(ir, 1, 1024, fs)
    end        
    
    disp('CIC Decoder -full');
    [hdl1] = jvx_dsp.pdm.jvx_cic_dec_init(os, M, N, true);
    [hdl1, out_dec_dsv, norm] = jvx_dsp.pdm.jvx_cic_dec_process(hdl1, out, false, true);
    out_dec_dsv = double(out_dec_dsv);
    out_dec_dsv = out_dec_dsv * norm;

    if(doplot)
        [specin, ffts] = freqz(out, 1, FFTSIZE, fs);
        [specout, ffts] = freqz(out_dec_dsv, 1, FFTSIZE, fs);
        
        show1 = 10*log10(specin.*conj(specin));
        show2 = 10*log10(specout.* conj(specout));
               
        figure;
        plot(ffts, show1, 'r+');
        hold on;
        plot(ffts, show2, 's:');
    end
    
    disp('CIC Decoder -subsampled');     
    [hdl2] = jvx_dsp.pdm.jvx_cic_dec_init(os, M, N, false);
    [hdl2, out_dec_ds, norm] = jvx_dsp.pdm.jvx_cic_dec_process(hdl2, out, false, true);
    out_dec_ds = double(out_dec_ds);
    out_dec_ds = out_dec_ds * norm;
end

if(doplot)
    if(plotIn)
        
        [specin, ffts] = freqz(sig_ds, 1, FFTSIZE, fs0);
        [specout, ffts] = freqz(out_dec_ds, 1, FFTSIZE, fs0);
        
        show1 = 10*log10(specin.*conj(specin));
        show2 = 10*log10(specout.* conj(specout));
        
        figure;
        plot(ffts, show1, 'r+');
        hold on;
        plot(ffts, show2, 's:');
            
    else
        
        [specout, ffts] = freqz(out_dec_ds, 1, FFTSIZE, fs0);
        
        show2 = 10*log10(specout.* conj(specout));
        
        
        figure;
        plot(ffts, show2, 's:');
    end   
end

%% ===================================================
%% ===================================================
%% ===================================================
%% ===================================================
%% ===================================================

function [out] = rbit(in, numBits)
    out = in;
    for(ind=1:size(in,2))
        ins = dec2bin(in(ind), numBits);
        outs = fliplr(ins);
        out(ind) = bin2dec(outs);
    end
    
function [out] = serialize(in, numBits, lsbfirst)
lin = size(in, 2);
out = zeros(1, lin*numBits);
if(lsbfirst)
    
    nP = 0.0;
    for(ind=0:lin-1)
        p = ind/lin;
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        oneWord = in(ind+1);
        for(ind2 = 1:numBits)
            if(bitget(oneWord, ind2))
                out(ind*numBits+ind2) = 1;
            else
                out(ind*numBits+ind2) = -1;
            end
        end
    end
else
    nP = 0.0;
    for(ind=0:lin-1)
        p = ind/lin;
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        oneWord = in(ind+1);
        for(ind2 = 1:numBits)
            idxBit = numBits - ind2 + 1;
            if(bitget(oneWord, idxBit))
                out(ind*numBits+ind2) = 1;
            else
                out(ind*numBits+ind2) = -1;
            end
        end
    end
end

function [out] = pack(in, numBits, lsbfirst)
lin = size(in, 2);
out= zeros(1, ceil(lin/numBits));
cnt = 1;
if(numBits == 16)
    tok ='uint16';
    out = uint16(out);
else if (numBits == 32)
        tok ='uint32';
        out = uint32(out);
    else
        error('Unsupported');
    end
end

if(lsbfirst)
    
    nP = 0.0;
    if(numBits == 16)
        reg = uint16(0);
    else
        reg = uint32(0);
    end
    for(ind=1:lin)
        p = ind/lin;
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        
        oneBit = in(ind);
        reg = bitshift(reg, -1);    
        if(oneBit > 0)
            reg = bitset(reg, numBits);
        end
            
        if(mod(ind, numBits) == 0)
            oneWord = reg;
            %if(bitget(oneWord, numBits))
            %    oneWord = bitcmp(oneWord, tok) + 1;
            %    oneWord = -oneWord;
            %end
            out(cnt) = oneWord;
            cnt = cnt + 1;
        end
    end
else
    nP = 0.0;
    if(numBits == 16)
        reg = uint16(0);
    else
        reg = uint32(0);
    end
    for(ind=1:lin)
        p = ind/lin;
        if(p>= nP)
            disp([num2str(nP*100) '%']);
            nP = nP + 0.1;
        end
        
        oneBit = in(ind);
        reg = bitshift(reg, 1);    
        if(oneBit > 0)
            reg = bitset(reg, 1);
        end
            
        if(mod(ind, numBits) == 0)
            oneWord = reg;
            %if(bitget(oneWord, numBits))
            %    oneWord = bitcmp(oneWord, tok) + 1;
            %    oneWord = -oneWord;
            %end
            out(cnt) = oneWord;
            cnt = cnt + 1;
        end
    end
end

function ir = calc_ir_cic(os, N, M)
    num = [1 zeros(1, os*M-2) -1]; 
    den = [ 1 -1]; 
    numu = 1; 
    denu = 1; 
    for(ind=1:N) 
        numu = conv(numu, num); 
        denu = conv(denu, den); 
    end; 
    ir = impz(numu, denu);
    ir = ir / sum(ir);