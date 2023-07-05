% Writes matrix to CXRO binary monochrome matrix in compact (.BMM) format.  This is
% used by the ZPDEV website, which can easily convert .BMM to .GDS.  The
% corresponding GDS will be defined on a pixel grid specified by the size
% of the input matrix and the size conversion factor defined by the user.
%
% function writeBMM(filename, A, umPerPixel), copyright CXRO 2014
%
% FILENAME specifies output file: example.bmm
% A is the input matrix.  Values of A must be either 0 or 1
% UMPERPIXEL specifies the dimensions of a pixel in microns
%
% BMM format:
%
% [CXRO-BMM] (8 bytes), [angstroms/pix] (4 bytes), [sr, sc] (8 bytes)
% [data] (num elements/8 bytes)
%
% Data is written by looping through row dimension first, similar to A(:)
% Headers are written in little-endian 32-bit unsigned integers

function writeBMM(filename, A, umPerPixel)

angstromPerPixel = round(umPerPixel*10000);
[sr, sc] = size(A);

% Check if input is a binary matrix:
if (~all(A == 1 | A == 0))
    error('Matrix values must be 1 or 0');
end

% create one long column
lm      = A(:);

% Append zeros if length is not a multiple of 8
lm      = [lm; zeros(mod(-sr*sc,8),1)];

%convert each 8 bits in array to a byte represented by a uint8
byteAr  = sum(reshape(lm, 8, length(lm)/8).* ...
                ((2.^(0:7)')*ones(1, length(lm)/8)));
            
% Write header:
fid     = fopen(filename, 'w', 'ieee-le');

fwrite(fid, uint8('CXRO-BMM')); % Write ID header
fwrite(fid, angstromPerPixel, 'uint32');
fwrite(fid, sr*sc, 'uint32');
fwrite(fid, sr, 'uint32');
fwrite(fid, sc, 'uint32');


% Write data:
fwrite(fid, byteAr, 'uint8');

fclose(fid);