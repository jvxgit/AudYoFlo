classdef jvxMatrixReference < handle
  properties
    data
  end
  methods
    function h = jvxMatrixReference(N, M)
      h.data = zeros(N,M);
    end
    
    function setRow(obj, idx, dat)
        obj.data(idx, :) = dat;
    end
    
    function m = matrix(obj)
        m = obj.data;
    end
    
    function r = row(obj, idx)
        r = obj.data(idx,:);
    end
  end
end