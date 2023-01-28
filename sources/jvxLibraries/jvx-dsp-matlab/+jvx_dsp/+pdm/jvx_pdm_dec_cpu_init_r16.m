%% Function to resample a PDM signal in a CPU - that is: compute filter output based on 8 bit PDM inputs
% This function assumes that PDM signals come in frames of 16 bit
% collections in which the 16 bit fields are written from left to right
function [handle] = jvx_pdm_dec_cpu_init_r16(lir, isforpdm, ir, generateCHeader, dsigntoken, outfile)

    %addpath('../../../../base/sources/jvxLibraries/jvx-dsp-base-matlab/')

    % Fixed value for register width assumption: 16 bit per register
    handle.cfg.bitwidth = 16;

    % In order to save memory, we separate the 16 bit fields into two 8 bit
    % fields
    handle.cfg.n_fragments_per_reg = 2;

    % Number of iterations per word (16 bit -> 1; 32bit -> 2)
    handle.cfg.n_regs_per_value = 1;
    
    % Length of impulse response, should be multiple of 16!
    handle.cfg.lir_os = lir;

    % Yes, this should be 8 bit
    handle.derived.bitwidth_fragment = handle.cfg.bitwidth/handle.cfg.n_fragments_per_reg;

    % Derive number of 8-bit sequences in convolution
    handle.derived.lir_fragments = handle.cfg.lir_os/(handle.cfg.bitwidth/handle.cfg.n_fragments_per_reg);

    % Filter design parameter
    delta_filter = 0.4*1/2/handle.cfg.bitwidth;

    % Create required lookup tables
    handle.derived.number_ltables = handle.derived.lir_fragments/2;
    handle.derived.number_entries_table = 2^handle.derived.bitwidth_fragment;
    handle.lookuptables = zeros(handle.derived.number_ltables, handle.derived.number_entries_table);

    % For FIR filter Frequency band edges
    f = [0 1/handle.cfg.bitwidth - delta_filter/2 1/handle.cfg.bitwidth + delta_filter/2 1];

    % For FIR Filter Desired amplitudes
    a = [1  1  0 0];

    % Compute filter (length is param 1 + 1)
    if ~exist('ir','var')
        % firpm is not available in octave for now
        handle.ir = firpm(handle.cfg.lir_os-1,f,a);
    else
        handle.ir = ir;
    end
    % Temp buffer for input 'simulation'
    oneIn = zeros(1,handle.derived.bitwidth_fragment);

    % Decompose the ir into (symmetric) segments
    handle.segs = reshape(handle.ir', handle.derived.bitwidth_fragment, handle.derived.lir_fragments)';

    zeroVal = 0;
    if(nargin >= 2)
        if(isforpdm)
            zeroVal = -1;
        end
    end

    % Create lookup table
    for(ind = 0:handle.derived.number_ltables-1)
        oneseg = handle.ir(ind*handle.derived.bitwidth_fragment+1:(ind+1)*handle.derived.bitwidth_fragment);
        for(jnx = 0:handle.derived.number_entries_table-1)
            for(knx = 0:handle.derived.bitwidth_fragment-1)
                if(bitand(jnx, 2^knx))
                    oneIn(knx+1) = 1;
                else

                    oneIn(knx+1) = zeroVal; % Turn this int a '-1' for PDM
                end
            end

            % Create lookup table entries
            handle.lookuptables(ind+1,jnx+1) = fliplr(oneIn) * oneseg';
        end
    end

    % Display lookup table size
    disp(['Size lookup tables: ' num2str(prod(size(handle.lookuptables))) ' elements']);

    % Hold a second variant for 16 bit operation
    handle.lookuptables_16 = int16(handle.lookuptables * 2^15);

    % Create table for bit reversal addressing
    handle.lookupbitrevtable = zeros(1, handle.derived.number_entries_table);
    for(knx = 0:handle.derived.number_entries_table-1)
        knxrev = bin2dec(fliplr(dec2bin(knx,handle.derived.bitwidth_fragment)));
        handle.lookupbitrevtable(knx+1) = knxrev;
    end

    % Some internal state variables
    handle.pcnt = 0;
    handle.reg = 0;
    handle.ppcnt = 0;
    handle.mem.accu = zeros(1, handle.derived.lir_fragments);
    handle.mem.p = zeros(1,handle.derived.lir_fragments);

    cnt = 1;

    for(ind=1:handle.derived.lir_fragments)
        reg = 0;
        for(knx = 0:handle.derived.bitwidth_fragment-1)
            reg = bitshift(reg, 1);
            if(mod(cnt,2))
                reg = bitor(reg, 1);
            end
            cnt = cnt + 1;
        end
        handle.mem.p(ind) = reg;
    end

    if(generateCHeader)
        fname = ['jvx_lut_config' dsigntoken '.h'];
        if(nargin >= 6)
            fname = outfile;
        end
        fname
        [fhdl] = jvx_dsp_base.code_gen.cheader_begin(fname, 'jvx_pdm_run.m');
        allEntriesNumber = handle.derived.number_ltables * handle.derived.number_entries_table;
        allInOneTable = zeros(1, allEntriesNumber);
        for(ind=1:handle.derived.number_ltables)
            allInOneTable((ind-1)*handle.derived.number_entries_table+1:ind*handle.derived.number_entries_table) = ...
                handle.lookuptables(ind,:);
        end

        jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIR_PDM_DEC_NUM_ENTRIES' dsigntoken], ...
                                             int16(handle.derived.number_entries_table), '', false, 'Number entries per table');

        jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

        jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIR_PDM_DEC_NUM_TABLES' dsigntoken], ...
                                             int16(handle.derived.number_ltables), '', false, 'Number tables');

        jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

        %pragmas{1} = '#ifdef NRMOTOR_TARGET_OMAPL137';
        %pragmas{2} = '#pragma DATA_SECTION(".userDefinedDataTC") ';
        %pragmas{3} = '#endif';
        pragmas = {};

        jvx_dsp_base.code_gen.cheader_var(fhdl, 'static jvxInt16', ['JVX_FIR_PDM_DEC_TABLES' dsigntoken], ...
                                          '%i', int16(handle.lookuptables * 2^15), '', 'Tables', pragmas);

        jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

        jvx_dsp_base.code_gen.cheader_var(fhdl, 'static jvxUInt16', ['JVX_FIR_PDM_DEC_BITREVERSE' dsigntoken], ...
                                          '%x', handle.lookupbitrevtable, '', 'Bit reversal lookup', {}, '0x');

        jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

        jvx_dsp_base.code_gen.cheader_end(fhdl);
    end
