clc;
% clear all;
pkg load image;

function [gKernelX, gKernelY] = getGKernels(m, n, sigma)
	[h1, h2] = meshgrid( -(m-1)/2:(m-1)/2,
						 -(n-1)/2:(n-1)/2 );
	gKernelX = -2 * h1 / 4 / pi / sigma^4 .* exp (-(h1.*h1 + h2.*h2) / 2 / sigma^2);
	gKernelY = -2 * h2 / 4 / pi / sigma^4 .* exp (-(h1.*h1 + h2.*h2) / 2 / sigma^2);
endfunction

function gradienMag = getGradientMagnitude(xconv, yconv)
	gradienMag = sqrt(xconv.^2 .+ yconv.^2);
endfunction

function supressed = nonMaxSup(in, radius)
	k = (radius - 1)/2;
	patchedIn = patchBorder(in, k);
	supressed = patchedIn;
	for i = k+1:size(in,1)+k
		for j = k+1:size(in,2)+k
			localMax = patchedIn(i,j);
			overtaken = false;
			for a = - k +1 : k
				for b = - k +1 : k
					if localMax < patchedIn(i+a, j+b)
						% disp("i:"), disp(i+a)
						% disp("j:"), disp(j+b)
						% patchedIn(i,j)
						overtaken = true;
						% disp("next")
					endif
				endfor
			endfor
			if overtaken
				supressed(i, j) = 0;
			endif
		endfor
	endfor
endfunction

function borderPatched = patchBorder(in, width)
	borderPatched = zeros(rows(in)+2*width, columns(in)+2*width);
	borderPatched(1+width:rows(in)+width, 1+width:columns(in)+width) = in;
endfunction	

function normalized = getNormalized(in)
	maxVal = max(abs(in(:)));
	normalized = uint8(in / maxVal * 127 + 128);
endfunction