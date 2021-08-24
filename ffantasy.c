#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NAVEGACAO 0 
#define BATALHA 1
#define TAM_mochileiro 42 // lado do triangulo nave
#define raioAsteroide randInt(TAM_mochileiro/2, TAM_mochileiro*2)
#define PASSO ((TAM_mochileiro*sqrt(3))/3)/10
#define distM TAM_mochileiro*sqrt(3)/3
#define TAM_OBJ 50
#define MAX_DIST distM + ast.raio
#define ATACAR 0 
#define ESPECIAL 1
#define FUGIR 2
#define CIMA 0
#define BAIXO 1
#define DIR 2
#define ESQ 3


const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

int CEU_H;
int X_OPT;
int Y_OPT;
ALLEGRO_FONT *FONTE;

//aleatorio entre 0 e n - 1
int random(int n){
	return rand() % n;
}

//aleatorio entre min e max (inclusive)
int randInt(int min, int max){
	return min + random(max - min + 1);
}

float dist(int x1, int y1, int x2, int y2){

	return sqrt( pow(x1-x2,2) + pow(y1-y2,2));
}

typedef struct mochileiro {
	
	int x, y;
	ALLEGRO_COLOR cor;
	int vida;
	//0 = cima, 1 = baixo, 2 = dir, 3 = esq
	int direcao;

	//batalha
	int acao;
	int executar;
	int x_old, y_old;


} mochileiro;


typedef struct pirata {
	
	int x, y;
	ALLEGRO_COLOR cor;
	ALLEGRO_COLOR cor_asteroide;
	int raio;
	int vida;

	//batalha
	int ataca;
	int executar;

} pirata;


void initmochileiro(mochileiro *m) {

	m->x = (TAM_mochileiro*sqrt(3))/6;
	m->y = SCREEN_H - TAM_mochileiro/2;
	m->cor = al_map_rgb(14, 181, 103);
	m->vida = 100;
	m->direcao = ESQ;


	m->acao = FUGIR;
	m->executar = 0;
	m->x_old = m->x;
	m->y_old = m->y;

}

void initpirata(pirata *p) {

	p->x = randInt(raioAsteroide,SCREEN_W-raioAsteroide);
	p->y = randInt(raioAsteroide,SCREEN_H-raioAsteroide);
	p->cor = al_map_rgb(14, 181, 103);
	p->cor_asteroide = al_map_rgb(64, 64, 64);
	p->vida = 100;
	p->raio = raioAsteroide;

	p->ataca = 1;
	p->executar = 0;

}

void initGlobais(){
	CEU_H = SCREEN_H/4;
	X_OPT = 3*SCREEN_W/4;
	Y_OPT = 3*SCREEN_H/4;

	int tam_fonte = 32;
	if(SCREEN_W < 300)
		tam_fonte = 20;

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("arial.ttf", tam_fonte, 1);   
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
}

//------------------------------------------NAVEGACAO-----------------------------------------------------------------

void desenhamochileiroNaveg(mochileiro m){

	if(m.direcao == CIMA){
		al_draw_filled_triangle(m.x, m.y - (TAM_mochileiro*sqrt(3))/3, m.x - TAM_mochileiro/2, m.y + (TAM_mochileiro*sqrt(3))/6, m.x + TAM_mochileiro/2, m.y + (TAM_mochileiro*sqrt(3))/6, m.cor);
	} 
	else if(m.direcao == BAIXO){
		al_draw_filled_triangle(m.x, m.y + (TAM_mochileiro*sqrt(3))/3, m.x - TAM_mochileiro/2, m.y - (TAM_mochileiro*sqrt(3))/6, m.x + TAM_mochileiro/2, m.y - (TAM_mochileiro*sqrt(3))/6, m.cor); 
	} 
	else if(m.direcao == DIR){
		al_draw_filled_triangle(m.x - (TAM_mochileiro*sqrt(3))/3, m.y, m.x + (TAM_mochileiro*sqrt(3))/6, m.y - TAM_mochileiro/2, m.x + (TAM_mochileiro*sqrt(3))/6, m.y + TAM_mochileiro/2, m.cor);
	}
	else{
		al_draw_filled_triangle(m.x + (TAM_mochileiro*sqrt(3))/3, m.y, m.x - (TAM_mochileiro*sqrt(3))/6, m.y - TAM_mochileiro/2, m.x - (TAM_mochileiro*sqrt(3))/6, m.y + TAM_mochileiro/2, m.cor);
	}
}

void desenhaCenarioNaveg(){

	al_clear_to_color(al_map_rgb(92, 11, 132));
	al_draw_filled_rectangle(SCREEN_W - TAM_OBJ, 0, SCREEN_W, TAM_OBJ, al_map_rgb(51, 51, 255));
}

void desenhaAsteroide(pirata p){

	al_draw_filled_circle(p.x, p.y, p.raio, p.cor_asteroide);

}

int processaTeclaNaveg(mochileiro *m, int tecla){
	
		switch(tecla) {

			case ALLEGRO_KEY_UP:
				if(m->y - PASSO >= 0) 
					m->y -= 3*PASSO/2;
				m->direcao = CIMA;
				break;
			case ALLEGRO_KEY_DOWN:
				if(m->y + PASSO <= SCREEN_H) 
					m->y += 3*PASSO/2;
				m->direcao = BAIXO;
				break;
			case ALLEGRO_KEY_LEFT:
				if(m->x - PASSO >= 0)
					m->x -= 3*PASSO/2;
				m->direcao = DIR;
				break;
			case ALLEGRO_KEY_RIGHT:
				if(m->x + PASSO <= SCREEN_W) 
					m->x += 3*PASSO/2;
				m->direcao = ESQ;
				break;
			case ALLEGRO_KEY_W:
				if(m->y - PASSO >= 0) 
					m->y -= 3*PASSO/2;
				m->direcao = CIMA;
				break;
			case ALLEGRO_KEY_S:
				if(m->y + PASSO <= SCREEN_H) 
					m->y += 3*PASSO/2;
				m->direcao = BAIXO;
				break;
			case ALLEGRO_KEY_A:
				if(m->x - PASSO >= 0)
					m->x -= 3*PASSO/2;
				m->direcao = DIR;
				break;
			case ALLEGRO_KEY_D:
				if(m->x + PASSO <= SCREEN_W) 
					m->x += 3*PASSO/2;
				m->direcao = ESQ;
				break;
			case ALLEGRO_KEY_ESCAPE:
				return 0;

		}
	return 1;
}

int chegouObjetivo(mochileiro m){

	if(m.x >= SCREEN_W - TAM_OBJ && m.y <= TAM_OBJ)
		return 1;
	return 0;
}

//------------------------------------------BATALHA-----------------------------------------------------------------

void desenhaCenarioBatalha(){

	al_clear_to_color(al_map_rgb(64, 64, 64));
	al_draw_filled_rectangle(0, 0, SCREEN_W, CEU_H, al_map_rgb(92, 11, 132));
	al_draw_filled_rectangle(X_OPT, Y_OPT, SCREEN_W, SCREEN_H, al_map_rgb(0,0,0));
	al_draw_text(FONTE, al_map_rgb(181,14,14), X_OPT + 15, Y_OPT + 5, 0, "Fugir");
}

int detectouAsteroide(mochileiro m, pirata ast) {
	if(dist(m.x + distM, m.y + distM, ast.x + ast.raio, ast.y + ast.raio) <= MAX_DIST) // A NAVE TA ENTRANDO DENTRO DO ASTEROIDE
		return 1;
	return 0;
}

void processaTeclaBatalha(mochileiro *m, int tecla){

	switch(tecla) {

		case ALLEGRO_KEY_UP:
			m->acao = FUGIR;
			break;
		case ALLEGRO_KEY_DOWN:
			m->acao = FUGIR;
			break;
		case ALLEGRO_KEY_ENTER:
			m->executar = 1;
			break;
	}

}

int processaAcaomochileiro(mochileiro *m){

	if(m->executar){

		m->executar = 0;

		if(m->acao == FUGIR){

			m->x += 1*PASSO;
			m->y += 1*PASSO;

			return NAVEGACAO;
		}
	}
	return BATALHA;
}

int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	


 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
   


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());  	


	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	int modo_jogo = NAVEGACAO;
	int movementKey = 0;
	int movementFlag = 0;
	int tecla;
	
	initGlobais();

	mochileiro jorj;
	initmochileiro(&jorj);

	pirata kek1;
	initpirata(&kek1);

	pirata kek2;
	initpirata(&kek2);

	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {


			if(modo_jogo == NAVEGACAO){

				desenhaCenarioNaveg();
				desenhamochileiroNaveg(jorj);
				desenhaAsteroide(kek1);
				desenhaAsteroide(kek2);

				if(detectouAsteroide(jorj, kek1)){
					modo_jogo = BATALHA;
				}else if(detectouAsteroide(jorj, kek2)){
					modo_jogo = BATALHA;
				}

				if(chegouObjetivo(jorj))
					playing = 0;

			}
			else{

				desenhaCenarioBatalha();

				modo_jogo = processaAcaomochileiro(&jorj);

			}

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			movementFlag = 1;
			movementKey = ev.keyboard.keycode;

		} 
		else if(ev.type == ALLEGRO_EVENT_KEY_UP){
			movementFlag = 0;
		}

		if(movementFlag){
			if(modo_jogo == NAVEGACAO){
				playing = processaTeclaNaveg(&jorj, movementKey);
			}
			else{
				processaTeclaBatalha(&jorj, movementKey);

			}
		}

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}