function mat2gds(A, umPerPixel, gdsName)

% Create bmm array:

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
            
bmm = [sr, sc, angstromPerPixel, byteAr];


% parse bmm mat to gds:
mbmmMat2gds(bmm, gdsName);
