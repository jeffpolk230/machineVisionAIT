clc; clear all;

x = -10:1:10;
y = 17 * x;
y = rand(1, 21)*10;
data = [x(:) y(:)];

%% functionname: function description
function [outputs] = solving(arg)
	outputs = [];
	for i = 1:size(arg)-1
		a = (arg(i, 2)-arg(i+1, 2)) / (arg(i, 1)-arg(i+1, 1));
		b = a * arg(i,1) - arg(i,2);
		outputs = [outputs;a, b];
	endfor
endfunction