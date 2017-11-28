
#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>
#include <time.h>
#include "imageprocessing.h"
#include <FreeImage.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
imagem abrir_imagem(char *nome_do_arquivo);
void salvar_imagem(char *nome_do_arquivo);
void liberar_imagem(imagem *i);
 */

imagem abrir_imagem(char *nome_do_arquivo) {
  FIBITMAP *bitmapIn;
  int x, y;
  RGBQUAD color;
  imagem I;

  bitmapIn = FreeImage_Load(FIF_JPEG, nome_do_arquivo, 0);

  if (bitmapIn == 0) {
    printf("Erro! Nao achei arquivo - %s\n", nome_do_arquivo);
  } else {
    printf("Arquivo lido corretamente!\n");
   }

  x = FreeImage_GetWidth(bitmapIn);
  y = FreeImage_GetHeight(bitmapIn);

  I.width = x;
  I.height = y;
  I.r = malloc(sizeof(float) * x * y);
  I.g = malloc(sizeof(float) * x * y);
  I.b = malloc(sizeof(float) * x * y);

   for (int i=0; i<x; i++) {
     for (int j=0; j <y; j++) {
      int idx;
      FreeImage_GetPixelColor(bitmapIn, i, j, &color);

      idx = i + (j*x);

      I.r[idx] = color.rgbRed;
      I.g[idx] = color.rgbGreen;
      I.b[idx] = color.rgbBlue;
    }
   }
  return I;

}

void liberar_imagem(imagem *I) {
  free(I->r);
  free(I->g);
  free(I->b);
}

void salvar_imagem(char *nome_do_arquivo, imagem *I) {
  FIBITMAP *bitmapOut;
  RGBQUAD color;

  printf("Salvando imagem %d por %d...\n", I->width, I->height);
  bitmapOut = FreeImage_Allocate(I->width, I->height, 24, 0, 0, 0);

   for (int i=0; i<I->width; i++) {
     for (int j=0; j<I->height; j++) {
      int idx;

      idx = i + (j*I->width);
      color.rgbRed = I->r[idx];
      color.rgbGreen = I->g[idx];
      color.rgbBlue = I->b[idx];

      FreeImage_SetPixelColor(bitmapOut, i, j, &color);
    }
  }

  FreeImage_Save(FIF_JPEG, bitmapOut, nome_do_arquivo, JPEG_DEFAULT);
}

void multi_imagem_linha(imagem *I, float num) {
   struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
   gettimeofday(&rt0, NULL);
   
   for (int i=0; i<I->height; i++) {
     for (int j=0; j<I->width; j++) {
      int idx;

      idx = i + (j*I->height);
      I->r[idx]= I->r[idx]*num;
      if(I->r[idx] > 255) I->r[idx]=255.0;

      I->g[idx]= I->g[idx]*num;
      if(I->g[idx] > 255) I->g[idx]=255.0;

      I->b[idx]= I->b[idx]*num;
      if(I->b[idx] > 255) I->b[idx]=255.0;
    } 
   }
   gettimeofday(&rt1, NULL);
   timersub(&rt1, &rt0, &drt);
   printf("LINHA:Tempo real : %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
}

void multi_imagem_coluna(imagem *I, float num) {
   struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
   gettimeofday(&rt0, NULL);
   
   for (int i=0; i<I->width; i++) {
     for (int j=0; j<I->height; j++) {
      int idx;

      idx = i + (j*I->width);
      I->r[idx]= I->r[idx]*num;
      if(I->r[idx] > 255) I->r[idx]=255.0;

      I->g[idx]= I->g[idx]*num;
      if(I->g[idx] > 255) I->g[idx]=255.0;

      I->b[idx]= I->b[idx]*num;
      if(I->b[idx] > 255) I->b[idx]=255.0;
    } 
   }
   gettimeofday(&rt1, NULL);
   timersub(&rt1, &rt0, &drt);
   printf("COLUNA:Tempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
}

void multi_imagem_thread(imagem *I, float num) {
   struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
   gettimeofday(&rt0, NULL);
   pthread_t threadao[4];

   void* multh(void* argu){
	
	int* qualquer = (int*)argu;
   	for (int i = (*qualquer); i<I->height; i = i+4) {
   	  for (int j=0; j<I->width; j++) {
   	   int idx;
	
   	   idx = i + (j*I->height);
   	   I->r[idx]= I->r[idx]*num;
   	   if(I->r[idx] > 255) I->r[idx]=255.0;
	
   	   I->g[idx]= I->g[idx]*num;
   	   if(I->g[idx] > 255) I->g[idx]=255.0;
	
   	   I->b[idx]= I->b[idx]*num;
   	   if(I->b[idx] > 255) I->b[idx]=255.0;
   	 } 
   	}
   }
   int vetor[4] = {0,1,2,3};
   for (int bolinha = 0; bolinha<=3;bolinha++){
	   pthread_create (&threadao[bolinha], NULL, multh, &vetor[bolinha]);
   }
   for (int bolinha = 0; bolinha<=3;bolinha++){
   	pthread_join(threadao[bolinha],NULL);
   }
   gettimeofday(&rt1, NULL);
   timersub(&rt1, &rt0, &drt);
   printf("THREAD:Tempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
}

void multi_imagem_multproc(imagem *I, float num) {



   void multbrilho(int argu){
	
   	for (int i = (argu); i<I->height; i = i++) {
   	  for (int j=0; j<I->width; j++) {
   	   int idx;
	
   	   idx = i + (j*I->height);
   	   I->r[idx]= I->r[idx]*num;
   	   if(I->r[idx] > 255) I->r[idx]=255.0;
	
   	   I->g[idx]= I->g[idx]*num;
   	   if(I->g[idx] > 255) I->g[idx]=255.0;
	
   	   I->b[idx]= I->b[idx]*num;
   	   if(I->b[idx] > 255) I->b[idx]=255.0;
   	 } 
   	}
   }

   pid_t processoso[1];

   
   for (int bolinha = 0; bolinha<=1;bolinha++){
	processoso[bolinha] = fork();
	if (processoso ==0){
		multbrilho(bolinha);
		wait(NULL);
	}
   }
}



void divi_imagem(imagem *I, float num) {

   for (int i=0; i<I->width; i++) {
     for (int j=0; j<I->height; j++) {
      int idx;
    ///Caso num==0, punir usuário
    if(num==0){
      printf("Não divida por ZERO, cazzo!! CIAO!\n");
     }
    else{
      idx = i + (j*I->width);
      I->r[idx]= I->r[idx]/num;
      if(I->r[idx] > 255) I->r[idx]=255.0;

      I->g[idx]= I->g[idx]/num;
      if(I->g[idx] > 255) I->g[idx]=255.0;

      I->b[idx]= I->b[idx]/num;
      if(I->b[idx] > 255) I->b[idx]=255.0;
     }

    } 
   }
}

void maximo_imagem(imagem *I) {
   
   int max=0;

   for (int i=0; i<I->width; i++) {
     for (int j=0; j<I->height; j++) {
      int idx,temp=0;

      idx = i + (j*I->width);
      temp = I->r[idx] + I->b[idx] + I->g[idx];

      if(max <= temp) {
        max=temp;
      }

    } 
   }
   printf("O valor máximo é %d \n", max/3);
}
