function img2gds(imageFile, umPerPixel, gdsFileName)
    % Read the image file
    image = imread(imageFile);

    % Convert the image to a binary matrix of 0s and 1s
    binaryMatrix = imbinarize(rgb2gray(image));

    % Pass the binary matrix to the mat2gds function
    mat2gds(binaryMatrix, umPerPixel, gdsFileName);
end

