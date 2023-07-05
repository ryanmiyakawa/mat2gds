
% Uses: https://github.com/ryanmiyakawa/coords2gds
%
% Executes a compiled binary built from above repo

function csv2gds(csvFile, outputGDSFile)

system(sprintf('./bin/text2gds %s %s', csvFile, outputGDSFile));


