#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <Windows.h>  // 그래픽 출력 보조
#include <math.h>
#include <conio.h>    // _getch() 사용을 위한 헤더

////////////////////////////
/// 전역 변수부
////////////////////////////
int inH, inW, outH, outW;
unsigned char** inImage = NULL;
unsigned char** outImage = NULL;

HWND hwnd;
HDC hdc;

////////////////////////////
/// 함수 선언부
////////////////////////////
unsigned char getMedianValue();
void draw2Image(int h, int w);
void openImage(); void saveImage(); void drawImage(); void printMenu();
unsigned char** mallocMemory(int h, int w); void freeMemory(unsigned char** memory, int h);
float** mallocFloatMemory(int h, int w); void freeFloatMemory(float** memory, int h);

void equalImage(); void addImage(); void bwImage(); void bwImage2(); void andImage();
void zoomOutImage(); void zoomInImage(); void zoomIn2Image();
void rotateImage(); void rotate2Image();
void embossImage(); void endInImage(); void histoEqual();void blurImage();
void shapenImage(); void blur2Image(); void edgeSobelImage(); 
void homoedgeImage(); void diffedgeImage(); void moveImage();

////////////////////////////
/// 메인 코드부
////////////////////////////
void main() {
    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(NULL);

    char key = 0;
    while (key != '9') {
        printMenu();
        key = _getch();
        system("cls");
        switch (key) {
        case '1': openImage(); break;
        case '2': saveImage(); break;
        case 'a': case 'A': equalImage(); break;
        case 'b': case 'B': addImage(); break;
        case 'c': case 'C': bwImage(); break;
        case 'd': case 'D': bwImage2(); break;
        case 'e': case 'E': andImage(); break;
        case 'f': case 'F': zoomOutImage(); break;
        case 'g': case 'G': zoomInImage(); break;
        case 'h': case 'H': zoomIn2Image(); break;
        case 'i': case 'I': rotateImage(); break;
        case 'j': case 'J': rotate2Image(); break;
        case 's': case 'S': moveImage(); break;
        case 'k': case 'K': endInImage(); break;
        case 'l': case 'L': histoEqual(); break;
        case 'm': case 'M': blurImage(); break;
        case 'n': case 'N': shapenImage(); break;
        case 'o': case 'O': blur2Image(); break;
        case 'p': case 'P': edgeSobelImage(); break;
        case 'q': case 'Q': homoedgeImage(); break;
        case 'r': case 'R': diffedgeImage(); break;
        
        }
    }
    puts("안녕히 가세요~ 도비는 자유에요...");
}

////////////////////////////
/// 함수 정의부
////////////////////////////
unsigned char getMedianValue() {
    int hist[256] = { 0, };
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            hist[inImage[i][k]]++;
        }
    }
    int sum = 0;
    int target = (inH * inW) / 2; // 전체 픽셀의 절반 위치
    for (int i = 0; i < 256; i++) {
        sum += hist[i];
        if (sum >= target) return (unsigned char)i;
    }
}




void printMenu() {
    puts("\n*** GrayScale Image Processing (RC 1) ***");
    puts("1.열기 2.저장 9.종료");
    puts("A.동일영상 B.밝게/어둡게 C.흑백 D.흑백(평균)");
    puts("E.and연산 F.축소 G.확대 H.확대(백워딩) I.회전 J.회전(백워딩) S:이동");
    puts("K.End-In탐색 L.Histo평활 M.Blur N.Sharp O.Blur2 P:SobelEdge Q.유사연산자");
    puts("R.차연산자 T.차연산자");
}

unsigned char** mallocMemory(int h, int w) {
    unsigned char** retMemory;
    retMemory = (unsigned char**)malloc(sizeof(unsigned char*) * h);
    for (int i = 0; i < h; i++)
        retMemory[i] = (unsigned char*)malloc(sizeof(unsigned char) * w);
    return retMemory;
}

void freeMemory(unsigned char** memory, int h) {
    if (memory == NULL) return;
    for (int i = 0; i < h; i++)
        free(memory[i]);
    free(memory);
}

float** mallocFloatMemory(int h, int w) {
    float** retMemory;
    retMemory = (float**)malloc(sizeof(float*) * h);
    for (int i = 0; i < h; i++)
        retMemory[i] = (float*)malloc(sizeof(float) * w);
    return retMemory;
}

void freeFloatMemory(float** memory, int h) {
    if (memory == NULL) return;
    for (int i = 0; i < h; i++)
        free(memory[i]);
    free(memory);
}

void openImage() {
    FILE* rfp;
    char fullname[200] = "C:/image/mypic/";
    char filename[30];
    printf("파일명(확장자 제외)-->"); scanf("%s", filename);
    strcat(fullname, filename);
    strcat(fullname, ".raw");

    rfp = fopen(fullname, "rb");
    if (rfp == NULL) {
        puts("파일이 없어요. 경로를 확인하세요.");
        return;
    }

    freeMemory(inImage, inH);
    fseek(rfp, 0L, SEEK_END);
    long long fsize = ftell(rfp);
    fclose(rfp);

    inH = inW = (int)sqrt((double)fsize);
    inImage = mallocMemory(inH, inW);

    rfp = fopen(fullname, "rb");
    for (int i = 0; i < inH; i++)
        fread(inImage[i], sizeof(unsigned char), inW, rfp);
    fclose(rfp);

    equalImage();
}

void saveImage() {
    if (outImage == NULL) return;
    char fullname[200] = "C:/images/Etc_Raw(squre)/";
    char filename[30];
    printf("저장할 파일명-->"); scanf("%s", filename);
    strcat(fullname, filename);
    strcat(fullname, ".raw");

    FILE* wfp = fopen(fullname, "wb");
    for (int i = 0; i < outH; i++)
        fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
    fclose(wfp);
    printf("%s 로 저장됨.\n", fullname);
}

void drawImage() {
    if (outImage == NULL) return;
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            int px = outImage[i][k];
            SetPixel(hdc, k + 100, i + 300, RGB(px, px, px));
        }
    }
}
void draw2Image( int h, int w ) {
    if (outImage == NULL) return;
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            int px = outImage[i][k];
            SetPixel(hdc, k + 100, i + 300, RGB(px, px, px));
        }
    }
}

void equalImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            outImage[i][k] = inImage[i][k];
    drawImage();
}

void addImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    int value;
    printf("밝게/어둡게 할 값-->"); scanf("%d", &value);
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            int px = inImage[i][k] + value;
            if (px < 0) px = 0;
            else if (px > 255) px = 255;
            outImage[i][k] = (unsigned char)px;
        }
    }
    drawImage();
}

void bwImage() { // 임계값 127 기준 흑백
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            if (inImage[i][k] < 127) outImage[i][k] = 0;
            else outImage[i][k] = 255;
        }
    }
    drawImage();
}

void bwImage2() { // 평균값 기준 흑백
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    long long sumValue = 0;
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            sumValue += inImage[i][k];
    float radix = sumValue / (float)(inH * inW);

    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            if (inImage[i][k] < radix) outImage[i][k] = 0;
            else outImage[i][k] = 255;
        }
    }
    drawImage();
}

void andImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            outImage[i][k] = inImage[i][k] & 0xC9;
    drawImage();
}

void zoomOutImage() {
    freeMemory(outImage, outH);
    int scale;
    printf("축소배수-->"); scanf("%d", &scale);
    outH = inH / scale; outW = inW / scale;
    outImage = mallocMemory(outH, outW);
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            outImage[i / scale][k / scale] = inImage[i][k];
    drawImage();
}

void zoomInImage() {
    freeMemory(outImage, outH);
    int scale;
    printf("확대배수-->"); scanf("%d", &scale);
    outH = inH * scale; outW = inW * scale;
    outImage = mallocMemory(outH, outW);
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            outImage[i * scale][k * scale] = inImage[i][k];
    drawImage();
}

void zoomIn2Image() {
    freeMemory(outImage, outH);
    int scale;
    printf("확대배수-->"); scanf("%d", &scale);
    outH = inH * scale; outW = inW * scale;
    outImage = mallocMemory(outH, outW);
    for (int i = 0; i < outH; i++)
        for (int k = 0; k < outW; k++)
            outImage[i][k] = inImage[i / scale][k / scale];
    drawImage();
}

void rotateImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    int angle;
    printf("각도-->"); scanf("%d", &angle);
    double radian = angle * 3.141592 / 180.0;
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            int xd = (int)(cos(radian) * i - sin(radian) * k);
            int yd = (int)(sin(radian) * i + cos(radian) * k);
            if ((0 <= xd && xd < outH) && (0 <= yd && yd < outW))
                outImage[xd][yd] = inImage[i][k];
        }
    }
    drawImage();
}

void rotate2Image() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    int angle;
    printf("각도-->"); scanf("%d", &angle);
    double radian = angle * 3.141592 / 180.0;
    int cx = inH / 2, cy = inW / 2;
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            int xs = (int)(cos(radian) * (i - cx) + sin(radian) * (k - cy)) + cx;
            int ys = (int)(-sin(radian) * (i - cx) + cos(radian) * (k - cy)) + cy;
            if ((0 <= xs && xs < inH) && (0 <= ys && ys < inW))
                outImage[i][k] = inImage[xs][ys];
        }
    }
    drawImage();
}

void embossImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    float MASK[3][3] = { {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0} };
    float** tmpIn = mallocFloatMemory(inH + 2, inW + 2);
    float** tmpOut = mallocFloatMemory(outH, outW);

    for (int i = 0; i < inH + 2; i++)
        for (int k = 0; k < inW + 2; k++) tmpIn[i][k] = 127.0f;
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++) tmpIn[i + 1][k + 1] = (float)inImage[i][k];

    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            float S = 0.0;
            for (int m = 0; m < 3; m++)
                for (int n = 0; n < 3; n++)
                    S += tmpIn[i + m][k + n] * MASK[m][n];
            tmpOut[i][k] = S + 127.0f;
        }
    }

    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            if (tmpOut[i][k] > 255) outImage[i][k] = 255;
            else if (tmpOut[i][k] < 0) outImage[i][k] = 0;
            else outImage[i][k] = (unsigned char)tmpOut[i][k];
        }
    }
    freeFloatMemory(tmpIn, inH + 2);
    freeFloatMemory(tmpOut, outH);
    drawImage();
}

void endInImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    int high = inImage[0][0], low = inImage[0][0];
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            if (high < inImage[i][k]) high = inImage[i][k];
            if (low > inImage[i][k]) low = inImage[i][k];
        }
    }
    int value;
    printf("자르고 싶은 범위:");
    scanf("%d", &value);
    low += value; high -= value;
    if (low >= high) high = low + 1;

    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            int res = (int)(((float)(inImage[i][k] - low) / (high - low)) * 255.0);
            if (res > 255) res = 255;
            else if (res < 0) res = 0;
            outImage[i][k] = (unsigned char)res;
        }
    }
    drawImage();
}

void histoEqual() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    int hist[256] = { 0, };
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            hist[inImage[i][k]]++;

    int sumHist[256] = { 0, };
    sumHist[0] = hist[0];
    for (int i = 1; i < 256; i++)
        sumHist[i] = hist[i] + sumHist[i - 1];

    double normalHist[256] = { 0.0, };
    for (int i = 0; i < 256; i++)
        normalHist[i] = sumHist[i] * (1.0 / (inH * inW)) * 255.0;

    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            outImage[i][k] = (unsigned char)normalHist[inImage[i][k]];
    drawImage();
}
void blurImage() {  // 블러링 알고리즘
    // 메모리 해제
    freeMemory(outImage, outH);
    // 출력 영상의 크기를 결정(중요) --> 알고리즘에 따름.
    outH = inH;
    outW = inW;
    // 출력 영상 메모리 할당
    outImage = mallocMemory(outH, outW);

    // 중요!! 블러링 마스크 
    float MASK[3][3] = { {1.0 / 9,  1.0 / 9, 1.0 / 9},
                         {1.0 / 9,  1.0 / 9, 1.0 / 9},
                         {1.0 / 9,  1.0 / 9, 1.0 / 9} };
    // 임시 메모리 준비
    float** tmpInImage, ** tmpOutImage;
    tmpInImage = mallocFloatMemory(inH + 2, inW + 2);
    tmpOutImage = mallocFloatMemory(outH, outW);

    // 임시입력메모리에 127 채우기.(--> 평균값이 더 나을 것임)
    for (int i = 0; i < inH + 2; i++)
        for (int k = 0; k < inW + 2; k++)
            tmpInImage[i][k] = 127.0;
    // 입력 메모리 --> 임시 입력 메모리의 중앙에 채우기
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            tmpInImage[i + 1][k + 1] = inImage[i][k];

    // 회선 연산 : 마스크로 주욱 긁어가면서 계산하기
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            float S = 0.0;
            for (int m = 0; m < 3; m++) {
                for (int n = 0; n < 3; n++) {
                    S += tmpInImage[i + m][k + n] * MASK[m][n];
                }
            }
            tmpOutImage[i][k] = S;
        }
    }

    // 후처리 : 마스크의 합계가 0이면 128 더하기. 1이면 안 더함.
    //for (int i = 0; i < outH; i++)
    //	for (int k = 0; k < outW; k++)
    //		tmpOutImage[i][k] += 127.0;

    // 임시 출력 메모리 --> 출력 메모리(영상)
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            outImage[i][k] = (unsigned char)tmpOutImage[i][k];
        }
    }
    freeFloatMemory(tmpInImage, inH + 2);
    freeFloatMemory(tmpOutImage, outH);

    drawImage();
}
void shapenImage() {
           // 메모리 해제
        freeMemory(outImage, outH);
        // 출력 영상의 크기를 결정(중요) --> 알고리즘에 따름.
        outH = inH;
        outW = inW;
        // 출력 영상 메모리 할당
        outImage = mallocMemory(outH, outW);
 
        float MASK[3][3] = { {-1.0,  -1.0 , -1.0},
                             {-1.0,   9.0 , -1.0},
                             {-1.0,  -1.0,  -1.0} };
        // 임시 메모리 준비
        float** tmpInImage, ** tmpOutImage;
        tmpInImage = mallocFloatMemory(inH + 2, inW + 2);
        tmpOutImage = mallocFloatMemory(outH, outW);

        // 임시입력메모리에 127 채우기.(--> 평균값이 더 나을 것임)
        for (int i = 0; i < inH + 2; i++)
            for (int k = 0; k < inW + 2; k++)
                tmpInImage[i][k] = 127.0;
        // 입력 메모리 --> 임시 입력 메모리의 중앙에 채우기
        for (int i = 0; i < inH; i++)
            for (int k = 0; k < inW; k++)
                tmpInImage[i + 1][k + 1] = inImage[i][k];

        // 회선 연산 : 마스크로 주욱 긁어가면서 계산하기
        for (int i = 0; i < inH; i++) {
            for (int k = 0; k < inW; k++) {
                float S = 0.0;
                for (int m = 0; m < 3; m++) {
                    for (int n = 0; n < 3; n++) {
                        S += tmpInImage[i + m][k + n] * MASK[m][n];
                    }
                }
                tmpOutImage[i][k] = S;
            }
        }

        // 후처리 : 마스크의 합계가 0이면 128 더하기. 1이면 안 더함.
        //for (int i = 0; i < outH; i++)
        //	for (int k = 0; k < outW; k++)
        //		tmpOutImage[i][k] += 127.0;

        // 임시 출력 메모리 --> 출력 메모리(영상)
        
 
        for (int i = 0; i < outH; i++) {
            for (int k = 0; k < outW; k++) {
                
                if (tmpOutImage[i][k] > 255.0)
                    outImage[i][k] = 255.0;
                else if (tmpOutImage[i][k] < 0.0)
                    outImage[i][k] = 0.0;
                else outImage[i][k] = (unsigned char)tmpOutImage[i][k];
            }
        }
        freeFloatMemory(tmpInImage, inH + 2);
        freeFloatMemory(tmpOutImage, outH);

        drawImage();
    }
void blur2Image() {
    // 메모리 해제
    freeMemory(outImage, outH);
    // 출력 영상의 크기를 결정(중요) --> 알고리즘에 따름.
    outH = inH;
    outW = inW;
    // 출력 영상 메모리 할당
    outImage = mallocMemory(outH, outW);

    // 중요!! 블러링 마스크 5*5
    float MASK[5][5] = { {1.0 / 25,  1.0 / 25, 1.0 / 25,1.0 / 25,1.0 / 25},
                         {1.0 / 25,  1.0 / 25, 1.0 / 25,1.0 / 25,1.0 / 25},
                         {1.0 / 25,  1.0 / 25, 1.0 / 25,1.0 / 25,1.0 / 25} };
    // 임시 메모리 준비
    float** tmpInImage, ** tmpOutImage;
    tmpInImage = mallocFloatMemory(inH + 4, inW + 4);
    tmpOutImage = mallocFloatMemory(outH, outW);
    //중앙값 구하기(함수사용)
    unsigned char medianVal = getMedianValue();
   
    // 임시입력메모리에 127 채우기.(--> 평균값이 더 나을 것임)
    for (int i = 0; i < inH + 4; i++)
        for (int k = 0; k < inW + 4; k++)
            tmpInImage[i][k] = (float)medianVal;
    // 입력 메모리 --> 임시 입력 메모리의 중앙에 채우기
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            tmpInImage[i + 2][k + 2] = inImage[i][k];

    // 회선 연산 : 마스크로 주욱 긁어가면서 계산하기
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            float S = 0.0;
            for (int m = 0; m < 5; m++) {
                for (int n = 0; n < 5; n++) {
                    S += tmpInImage[i + m][k + n] * MASK[m][n];
                }
            }
            tmpOutImage[i][k] = S;
        }


    }
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {

            if (tmpOutImage[i][k] > 255.0)
                outImage[i][k] = 255.0;
            else if (tmpOutImage[i][k] < 0.0)
                outImage[i][k] = 0.0;
            else outImage[i][k] = (unsigned char)tmpOutImage[i][k];
        }
    }
    freeFloatMemory(tmpInImage, inH + 4);
    freeFloatMemory(tmpOutImage, outH);

    drawImage();
}
void edgeSobelImage() {
    freeMemory(outImage, outH);
    // 출력 영상의 크기를 결정(중요) --> 알고리즘에 따름.
    outH = inH;
    outW = inW;
    // 출력 영상 메모리 할당
    outImage = mallocMemory(outH, outW);

    // 소벨연산: 1.Gx,Gy 두번의 3*3 연산수행
    //           2.절대값합 
    float Gx[3][3] = { {-1.0,  -2.0 , -1.0},
                         {0.0,  0.0 , 0.0},
                         {1.0,  2.0,  1.0} };
    float Gy[3][3] = { {1.0,  0.0 , -1.0},
                        {2.0,  0.0 , -2.0},
                        {1.0,  0.0, -1.0} };
    
          
    // 임시 메모리 준비
    float** tmpInImage, ** tmpOutImage;
    tmpInImage = mallocFloatMemory(inH + 2, inW + 2);
    tmpOutImage = mallocFloatMemory(outH, outW);
    //중앙값 구하기(함수사용)
    unsigned char medianVal = getMedianValue();
        
    for (int i = 0; i < inH + 2; i++)
        for (int k = 0; k < inW + 2; k++)
            tmpInImage[i][k] = (float)medianVal;
    // 입력 메모리 --> 임시 입력 메모리의 중앙에 채우기
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            tmpInImage[i + 1][k + 1] = inImage[i][k];

    // 회선 연산 : 마스크로 주욱 긁어가면서 계산하기
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            float sumX = 0.0;
            float sumY = 0.0;
            for (int m = 0; m < 3; m++) {
                for (int n = 0; n < 3; n++) {
                    sumX += tmpInImage[i + m][k + n] * Gx[m][n];
                    sumY += tmpInImage[i + m][k + n] * Gy[m][n];
                }
            }
            //두방향의 기울기 강도계산
            float magnitude = fabs(sumX) + fabs(sumY);
            tmpOutImage[i][k] = magnitude;
        }


    }
    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {

            if (tmpOutImage[i][k] > 255.0)
                outImage[i][k] = 255.0;
            else if (tmpOutImage[i][k] < 0.0)
                outImage[i][k] = 0.0;
            else outImage[i][k] = (unsigned char)tmpOutImage[i][k];
        }
    }
    freeFloatMemory(tmpInImage, inH + 2);
    freeFloatMemory(tmpOutImage, outH);

    drawImage();

}
void homoedgeImage() {
        //유사연산자-->중심과 주변의 차이(뺄셈) 8번 + 최대값 찾기
        if (outImage != NULL) freeMemory(outImage, outH);
        outH = inH; outW = inW;
        outImage = mallocMemory(outH, outW);

        // 2. 패딩 처리 (3x3 영역을 조사하므로 상하좌우 1폭씩 추가)
        float** tmpIn = mallocFloatMemory(inH + 2, inW + 2);
        float** tmpOut = mallocFloatMemory(outH, outW);

        // 경계면은 중앙값으로 채우기
        unsigned char medianVal = getMedianValue();
        for (int i = 0; i < inH + 2; i++)
            for (int k = 0; k < inW + 2; k++)
                tmpIn[i][k] = (float)medianVal;

        // 실제 데이터 복사
        for (int i = 0; i < inH; i++)
            for (int k = 0; k < inW; k++)
                tmpIn[i + 1][k + 1] = (float)inImage[i][k];

        // 3. 유사 연산자 로직 적용
        for (int i = 0; i < inH; i++) {
            for (int k = 0; k < inW; k++) {
                float maxDiff = 0.0f; // 주변과의 차이 중 최대값을 저장할 변수
                float center = tmpIn[i + 1][k + 1]; // 현재 중심 픽셀 값

             
                for (int m = 0; m < 3; m++) {
                    for (int n = 0; n < 3; n++) {
                        // 자기 자신(중심)은 건너뜀
                        if (m == 1 && n == 1) continue;

                        // 중심값과 주변값의 차이의 절댓값 계산
                        float diff = fabsf(center - tmpIn[i + m][k + n]);

                        // 그중에서 가장 큰 차이값을 선택
                        if (diff > maxDiff) {
                            maxDiff = diff;
                        }
                    }
                }
                tmpOut[i][k] = maxDiff;
            }
        }

        
        for (int i = 0; i < outH; i++) {
            for (int k = 0; k < outW; k++) {
                if (tmpOut[i][k] > 255.0f) outImage[i][k] = 255;
                else if (tmpOut[i][k] < 0.0f) outImage[i][k] = 0;
                else outImage[i][k] = (unsigned char)tmpOut[i][k];
            }
        }

        freeFloatMemory(tmpIn, inH + 2);
        freeFloatMemory(tmpOut, outH);
        drawImage();
    }
void diffedgeImage() {
    //차연산자
    // 1. 3x3 영역에서 중심을 가로지르는 4가지 방향의 직선을 긋기
    //   2. 4방향으로 서로 빼기 
    
    if (outImage != NULL) freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);

    // 2. 패딩 처리 (3x3 영역을 조사하므로 상하좌우 1폭씩 추가)
    float** tmpIn = mallocFloatMemory(inH + 2, inW + 2);
    float** tmpOut = mallocFloatMemory(outH, outW);

    // 경계면은 중앙값으로 채우기
    unsigned char medianVal = getMedianValue();
    for (int i = 0; i < inH + 2; i++)
        for (int k = 0; k < inW + 2; k++)
            tmpIn[i][k] = (float)medianVal;

    // 실제 데이터 복사
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
            tmpIn[i + 1][k + 1] = (float)inImage[i][k];

    // 3. 차연산자 로직 적용
    
    for (int i = 0; i < inH; i++)
        for (int k = 0; k < inW; k++)
        {
            float d1 = fabsf(tmpIn[i][k] - tmpIn[i + 2][k + 2]); // 대각선 1
            float d2 = fabsf(tmpIn[i][k + 1] - tmpIn[i + 2][k + 1]); // 세로
            float d3 = fabsf(tmpIn[i][k + 2] - tmpIn[i + 2][k]);   // 대각선 2
            float d4 = fabsf(tmpIn[i + 1][k] - tmpIn[i + 1][k + 2]); // 가로

            float maxVal = d1;
            if (d2 > maxVal) maxVal = d2;
            if (d3 > maxVal) maxVal = d3;
            if (d4 > maxVal) maxVal = d4;

            tmpOut[i][k] = maxVal;
        }

    for (int i = 0; i < outH; i++) {
        for (int k = 0; k < outW; k++) {
            if (tmpOut[i][k] > 255.0f) outImage[i][k] = 255;
            else if (tmpOut[i][k] < 0.0f) outImage[i][k] = 0;
            else outImage[i][k] = (unsigned char)tmpOut[i][k];
        }
    }

    freeFloatMemory(tmpIn, inH + 2);
    freeFloatMemory(tmpOut, outH);
    drawImage();


}
void moveImage() {
    freeMemory(outImage, outH);
    outH = inH; outW = inW;
    outImage = mallocMemory(outH, outW);
    int hShift, wShift;
    printf("수직이동값:"); scanf("%d", &hShift);
    printf("좌우이동값:"); scanf("%d", &wShift);
    //printf("왼쪽으로이동값:"); scanf("%d", &right);
    //printf("오른쪽이동값:"); scanf("%d", &left);
    //이동후 중앙값 계산


    //배경을 0으로 초기화
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            outImage[i][k] = 0;
        }
    }
    //백워드 적용
    /*for (int i = 0; i < outH;i++) {
        for (int k = 0; k < outW;k++) {

            int oldI = i - hShift;
            int oldK = k - wShift;

            if (oldI >= 0 && oldI < inH && oldK >= 0 && oldK < inW)
                outImage[i][k] = inImage[oldI][oldK];
        }
    }*/
    for (int i = 0; i < inH; i++) {
        for (int k = 0; k < inW; k++) {
            // [직산] 원본 (i, k)가 결과 영상의 어디로 가야 하는가?
            // 수식: 결과_좌표 = 원본_좌표 + 이동량
            int newI = i + hShift;
            int newK = k + wShift;

            // 3. 범위 검사 (결과 좌표가 화면 안인지 확인)
            if (newI >= 0 && newI < outH && newK >= 0 && newK < outW) {
                // 원본의 값을 결과 위치에 직접 대입
                outImage[newI][newK] = inImage[i][k];
            }
        }
    }

    
    
    
    drawImage();
}
    
    
    
    
    
    
   