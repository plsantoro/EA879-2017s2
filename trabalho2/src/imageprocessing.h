
#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H


typedef struct {
  unsigned int width, height;
  float *r, *g, *b;
} imagem;

imagem abrir_imagem(char *nome_do_arquivo);
void salvar_imagem(char *nome_do_arquivo, imagem *I);
void liberar_imagem(imagem *i);
void divi_imagem(imagem *I, float num);

void multi_imagem_linha(imagem *I, float num);
void multi_imagem_coluna(imagem *I, float num);
void multi_imagem_thread(imagem *I, float num);
void multi_imagem_multproc(imagem *I, float num);

void maximo_imagem(imagem *I);
void medir_tempo(void *(*funcao)(void *args), void *args);

#endif
