#include <stdio.h>
#include <mex.h>
#include <math.h>

int getValue(unsigned char * data, long z){
	long charNum                = z / 8;
	unsigned char bitNum        = z % 8;
	unsigned char dat           = data[charNum];
    //printf("value %ld = %d\n", z, (int)((dat >> bitNum) & 1));
	return (int)((dat >> bitNum) & 1);	
}


bool valEq(unsigned char * dat, long s1, long f1, long s2){
	for (long k = s1; k <= f1; k++)
		if (getValue(dat, k) != getValue(dat, s2 + k - s1))
			return false;
	return true;
}


void zeroOut(unsigned char * dat, long startIdx, long endIdx){

	long charNum1 	= startIdx/8;
	long bitNum1 	= startIdx % 8;
	long charNum2 	= endIdx/8;
	long bitNum2 	= endIdx % 8;
	long m;
	for (long k = charNum1; k <= charNum2; k++){
		if (charNum1 == charNum2){
			for (m = bitNum1; m <= bitNum2; m++)
				dat[charNum1] = (255 - (1 << m)) & dat[charNum1];
			return;
		}
		
		if (k == charNum1)
			for (m = bitNum1; m < 8; m++)
				dat[k] = (255 - (1 << m)) & dat[k];  
		else if (k == charNum2)
			for (m = 0; m <= bitNum2; m++)
				dat[k] = (255 - (1 << m)) & dat[k]; 
		else
			dat[k] = 0;
	}
}

unsigned long decode32(unsigned char * int32){
    
    /*for (int k = 0; k < 4; k++){
     printf("%d\n", (int) int32[k]);    
    }*/
    unsigned long out = 0;
    
    out += ((unsigned long) int32[0]) << 0;
    out += ((unsigned long) int32[1]) << 8;
    out += ((unsigned long) int32[2]) << 16;
    out += ((unsigned long) int32[3]) << 24;
    return out;
}

void encode32(long coord, long app, char * cPart){    
    long aCoord = coord * app;
    cPart[0] = (aCoord >> 24) & 255;
    cPart[1] = (aCoord >> 16) & 255;
    cPart[2] = (aCoord >> 8) & 255;
    cPart[3] = (aCoord) & 255;
    
    
}

void encodePoly32(long * coords, long app, char * cCoords){
    char cPart[4];
    for (int k = 0; k < 10; k++){
        encode32(coords[k], app, cPart);
        for (int m = 0; m < 4; m++){
            cCoords[k*4 + m] = cPart[m];
        }
    }
    
    
}

void parseBMMData(unsigned char * data, long sr, long sc, long app, char * fname){
    
    // Define ambles:
    int gdspreamble[103]    = {0, 6, 0, 2, 0, 7, 0, 28, 1, 2, 230, 43, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
								230, 43, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 10, 2, 6, 110, 111, 110, 97,
								109, 101, 0, 20, 3, 5, 61, 104, 219, 139, 172, 113, 12, 180, 56, 109,
								243, 127, 103, 94, 246, 236, 0, 28, 5, 2, 0, 114, 0, 4, 0, 17, 0, 13, 
								0, 22, 0, 56, 0, 114, 0, 4, 0, 17, 0, 13, 0, 22, 0, 56, 0, 11, 6, 6, 99, 110, 
								111, 110, 97, 109, 101};
    int gdspostamble[8]     = {0, 4, 7, 0, 0, 4, 4, 0};           
    int polypreamble[16]    = {0, 4, 8, 0, 0, 6, 13, 2, 0, 1, 0, 6, 14, 2, 0, 0};
	int polypostamble[4]    = {0, 4, 17, 0};
    
    int polyBlockFormat[4]  = {0, 44, 16, 3};
    
    // cast to char:
    unsigned char gdsPre[103];
    unsigned char gdsPost[8];
    unsigned char polyPre[16];
    unsigned char polyPost[4];
    unsigned char polyForm[4];
    
    for (int k = 0; k < 103; k++)
        gdsPre[k] = (unsigned char) gdspreamble[k];
    for (int k = 0; k < 8; k++)
        gdsPost[k] = (unsigned char) gdspostamble[k];
    for (int k = 0; k < 16; k++)
        polyPre[k] = (unsigned char) polypreamble[k];
    for (int k = 0; k < 4; k++)
        polyPost[k] = (unsigned char) polypostamble[k];
    for (int k = 0; k < 4; k++)
        polyForm[k] = (unsigned char) polyBlockFormat[k];
    
    
    long polyCt = 0;
    
    char cCoords[40];
    
    FILE * fp;
    if((fp=fopen(fname, "wb"))==NULL) {
        printf("Cannot open file.\n");
        return;
    }
    
    
    // Write GDS preamble:
    fwrite(gdsPre, sizeof(char), 103, fp);
    
    
	for (long z = 0; z < sc*sr; z++){
		if (getValue(data, z) == 0)
			continue;
		
		long zz = z;
		while (zz % sr < sr && getValue(data, zz + 1) == 1)
			zz++;
	
		long yy = z;
		long yz = zz-yy;
		while (yy/sr < sc - 1 && valEq(data, yy, yy+yz, yy+sr))
			yy += sr;
		
        
		long m   = z / sr;
		long k   = z % sr;
		long kk  = k + yz;
		long mm  = m + ((yy - z)/sr);
		
        //printf("[yy: %d, z: %d, mm: %d]\n", (int)yy,(int)z, (int)mm);
        
		long x1 = m;
		long x2 = mm+1;
		long y1 = sr-kk-1;
		long y2 = sr-k;
        
        //printf("[x1: %d, x2: %d, y1: %d, y2: %d]\n", (int)x1,(int)x2,(int)y1,(int)y2 );
		/***************/
		// Encode polygon block format:
        
        //thisPolyBlock += polypreamble;
        fwrite(polyPre, sizeof(char), 16, fp);
		
		//thisPolyBlock += String.fromCharCode(0, 44, 16, 3); // num bytes and number format identifier
        fwrite(polyForm, sizeof(char), 4, fp);
        
        
        //thisPolyBlock += encodePoly32(coords, app, cCoords);
        long coords[10] = {x1, y1, x2, y1, x2, y2, x1, y2, x1, y1};
		encodePoly32(coords, app, cCoords);
        fwrite(cCoords, sizeof(char), 40, fp);
		
        //thisPolyBlock += polypostamble;
        fwrite(polyPost, sizeof(char), 4, fp);
		
	
		for (int q = m; q <= mm; q++)
			zeroOut(data, k + q*sr, kk + q*sr);
        
        polyCt++;
	}
    printf("Polygon Count: %ld\n", polyCt);
    // Write GDS postamble:
    fwrite(gdsPost, sizeof(char), 8, fp);
    
    fclose(fp);
}



// Gateway function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{

    char *gdsName;

    double* dat = mxGetPr(prhs[0]);  // Pointer to the matrix data    gdsName = mxArrayToString(prhs[1]);
    gdsName = mxArrayToString(prhs[1]);

    long sr = (long)dat[0];
    long sc = (long)dat[1];
    long app = (long)dat[2];

    long fSize = sr * sc;

    unsigned char data[fSize];

    for (long k = 0; k < fSize; k++)
        data[k] = (unsigned char)dat[k + 3];


    printf("sr = %ld, sc = %ld, fsize = %ld, app = %ld\n", sr, sc, fSize, app);

    parseBMMData(data, sr, sc, app, gdsName);
    
    
}