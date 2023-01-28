function y = init_firQMF_Filterbank( h0 )
% y = init_firQMF_Filterbank( h0 )
% gibt struct y zurueck mit Parametern und States fuer die Prozessierung
% einer FIR QMF-Bank mittles der Funktionen firQMFAnalysisFilterbank.m und
% firQMFSynthesisFilterbank.m
%
% created      : 19.09.2012
% authors      : Andreas Friedrich and Heiner Loellmann (IND, RWTH Aachen)
% modified     : 
%
% Referenze
% [1] J. D. Johnston, “A filter family designed for use in quadrature mirror
%     filter banks,” in Proc. ICASSP-1980, May 1980, pp. 291-294.

if nargin == 0
    load('johnston32TapD.mat')
    h0 = johnston32TapD;
end

% QMF Filter Tiefpasskanal
y.filters.h0 = h0;
y.filters.g0 = 2*y.filters.h0;

% QMF Filter Hochpasskanal
W = (-1).^(0:length(y.filters.h0)-1); % Modulation
y.filters.h1 = y.filters.h0.*W.';
y.filters.g1 = -2*y.filters.h1;

% Filterlänge
L = length(y.filters.h0);

y.states.h0 = zeros(L-1,1);
y.states.h1 = zeros(L-1,1);
y.states.g0 = zeros(L-1,1);
y.states.g1 = zeros(L-1,1);

% state of up- and downsampler
% (start with sampling the first sample of inputSignal)
y.states.Rd = 0; 
y.states.Ru = 0;
