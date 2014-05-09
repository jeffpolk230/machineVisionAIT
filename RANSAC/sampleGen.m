% Generating random sample point with k% of outlier 
% Prep step for RANSAC testing
clc; clear all;

x = -20:.1:20;
y = 17*x + 29;
% should take-in model ?

%% noisySamples: generate noisy samples with 1/k of outliers and t as threshold for inliers
function [noisySamples] = getNoisySamples(k, t, model)
	numberOfpoints = size(model)(2);
	avg = sum(model)/numberOfpoints;
	outliers = (mod(floor(rand(1,numberOfpoints)*100),ceil(100/k))==0).*avg;
	%how far should outlier go?
	noisySamples = model .+ (2*t*rand(1,numberOfpoints)-t).+outliers;
endfunction

samples = getNoisySamples(30, 20 , y);
% subplot(2,1,1);
% plot(y);
% subplot(2,1,2);
% plot(samples,'r');
