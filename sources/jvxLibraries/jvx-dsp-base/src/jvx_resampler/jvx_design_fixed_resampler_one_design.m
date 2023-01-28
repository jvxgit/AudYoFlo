function [] = jvx_design_fixed_resampler_one_design(bsizein, bsizeout,lfilt,stopbandatten, socket, TUNING_FACTOR, ofile)

[str] = jvx_design_fixed_resampler_fir(bsizein, bsizeout,lfilt,stopbandatten, socket, TUNING_FACTOR)

disp(['[jvx_fixed_resampler] Buffersize in: ' num2str(str.buffersizeIn)]);
disp(['[jvx_fixed_resampler] Buffersize out: ' num2str(str.buffersizeOut)]);
disp(['[jvx_fixed_resampler] Oversampling: ' num2str(str.oversamplingFactor)]);
disp(['[jvx_fixed_resampler] Downsampling: ' num2str(str.downsamplingFactor)]);
disp(['[jvx_fixed_resampler] Filter length: ' num2str(str.lFilter)]);

[fhdl] = jvx_dsp_base.code_gen.cheader_begin(ofile , 'jvx_design_fixed_resampler.m');

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_BSIZE_IN' str.designtoken], ...
	int16(str.buffersizeIn), '', false, 'Input buffersize');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_BSIZE_OUT' str.designtoken], ...
	int16(str.buffersizeOut), '', false, 'Input buffersize');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_OVERSAMPLING' str.designtoken], ...
	int16(str.oversamplingFactor), '', false, 'Oversampling');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_DOWNSAMPLING' str.designtoken], ...
	int16(str.downsamplingFactor), '', false, 'Downsampling');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_LFILTER' str.designtoken], ...
	int16(str.lFilter), '', false, 'Filter length');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_STATES_BUF_LENGTH' str.designtoken], ...
	int16(str.lStates), '', false, 'Filter length');

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_define(fhdl, ['JVX_FIXED_RESAMPLER_IMPRESP_BUF_LENGTH' str.designtoken], ...
	int16(length(str.impResp)), '', false, 'Filter length');

	%pragmas{1} = '#ifdef NRMOTOR_TARGET_OMAPL137';
    %pragmas{2} = '#pragma DATA_SECTION(".userDefinedDataTC") ';
    %pragmas{3} = '#endif';
	pragmas = {};

jvx_dsp_base.code_gen.cheader_var(fhdl, 'static jvxData', ['JVX_FIXED_RESAMPLER_IMPRESP' str.designtoken], ...
        '%f', str.impResp, false, 'Impulse Response', pragmas);

jvx_dsp_base.code_gen.cheader_emptyline(fhdl);

jvx_dsp_base.code_gen.cheader_end(fhdl);
