function [hdl] = terminate(hdl)

	if (~isempty(hdl.prv))
		hdl.prv = [];
	end
end