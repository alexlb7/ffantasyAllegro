#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NAVEGACAO 0 
#define BATALHA 1
#define FIM 2
#define TAM_mochileiro 42 // lado do triangulo nave
#define raioAsteroide randInt(TAM_mochileiro/3, TAM_mochileiro)
#define PASSO ((TAM_mochileiro*sqrt(3))/3)/5
#define distM TAM_mochileiro*sqrt(3)/3
#define TAM_OBJ 50
#define MAX_DIST distM + ast.raio
#define ESPECIAL 1
#define ATACAR 2 
#define FUGIR 3
#define CIMA 0
#define BAIXO 1
#define DIR 2
#define ESQ 3
#define DANO_ATAQUE 50
#define DANO_ATAQUE_ESP randInt(35,95)
#define DANO_INI1 20
#define DANO_INI2 25
#define DANO_INI3 30
#define MONSTRO 5

const float FPS = 20;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

int velocidade=0;
int X_OPT;
int Y_OPT;
int X_kek1;
int Y_kek1;
int X_kek2;
int Y_kek2;
int X_MB;
int Y_MB;
int X_PIRATA;
int Y_PIRATA;
ALLEGRO_FONT *FONTE;
ALLEGRO_BITMAP *navegacao = NULL;
ALLEGRO_BITMAP *mochila = NULL;
ALLEGRO_BITMAP *batalha = NULL;
ALLEGRO_BITMAP *pirat = NULL;
ALLEGRO_SAMPLE *theme =NULL;

int contador;

int Y_CIRCULO;

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
	float vidaMax;
	//0 = cima, 1 = baixo, 2 = dir, 3 = esq
	int direcao;

	//batalha
	int acao;
	int executar;
	int pontuacao;
	int recordeCod;


} mochileiro;


typedef struct pirata {
	
	int x, y;
	ALLEGRO_COLOR cor;
	ALLEGRO_COLOR cor_asteroide;
	int raio;
	int vida;
	float vidaMax;

	//batalha
	int executar;
	int lvl;
	int dano;

} pirata;


void initmochileiro(mochileiro *m) {

	m->x = (TAM_mochileiro*sqrt(3))/6;
	m->y = SCREEN_H - TAM_mochileiro/2;
	m->cor = al_map_rgb(14, 181, 103);
	m->vida = 142;
	m->vidaMax = 142.0;

	m->direcao = ESQ;


	m->acao = 0;
	m->executar = 0;
	m->pontuacao = 0;

}

void initpirata(pirata *p) {

	float r = raioAsteroide;

	p->x = randInt(r,SCREEN_W-r);
	p->y = randInt(r,SCREEN_H-r);
	p->cor_asteroide = al_map_rgb(64, 64, 64);
	p->raio = r;
	p->executar = 0;

	p->lvl = randInt(1,3);
	if(p->lvl == 1){
		p->vida = 188;
		p->vidaMax = 188.0;
		p->dano = DANO_INI3;
	}
	else if(p->lvl == 2){
		p->vida = 142;
		p->vidaMax = 142.0;
		p->dano = DANO_INI2;
	} 
	else if(p->lvl == 3){
		p->vida = 102;
		p->vidaMax = 102.0;
		p->dano = DANO_INI1;
	} 

}

void initGlobais(){
	X_OPT = 3*SCREEN_W/4;
	Y_OPT = 3*SCREEN_H/4;
	X_MB = 600;
	Y_MB = 110;
	X_PIRATA = 50;
	Y_PIRATA = Y_MB;

	Y_CIRCULO = (3*SCREEN_H/4) + 28;

	X_kek1 = X_MB;
	Y_kek1 = Y_MB + 20;

	X_kek2 = X_PIRATA + 70;
	Y_kek2 = Y_PIRATA + 142;

	contador = 1;

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

void desenhaPont(mochileiro *m){
	char my_text[20];
	sprintf(my_text, "Pontuacao: %d", m->pontuacao);
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 10, 10, 0, my_text);
}
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

	al_draw_bitmap(navegacao, 0, 0, 0);
	al_draw_filled_rectangle(SCREEN_W - TAM_OBJ, 0, SCREEN_W, TAM_OBJ, al_map_rgb(51, 51, 255));

}

void desenhaAsteroide(pirata p){

	al_draw_filled_circle(p.x, p.y, p.raio, p.cor_asteroide);

}

int processaTeclaNaveg(mochileiro *m, int tecla){
	
		switch(tecla) {

			case ALLEGRO_KEY_UP:
				if(m->y - distM >= 0) 
					m->y -= 3*PASSO/2;
				m->direcao = CIMA;
				break;
			case ALLEGRO_KEY_DOWN:
				if(m->y + distM <= SCREEN_H) 
					m->y += 3*PASSO/2;
				m->direcao = BAIXO;
				break;
			case ALLEGRO_KEY_LEFT:
				if(m->x - distM >= 0)
					m->x -= 3*PASSO/2;
				m->direcao = DIR;
				break;
			case ALLEGRO_KEY_RIGHT:
				if(m->x + distM <= SCREEN_W) 
					m->x += 3*PASSO/2;
				m->direcao = ESQ;
				break;
			case ALLEGRO_KEY_W:
				if(m->y - distM >= 0) 
					m->y -= 3*PASSO/2;
				m->direcao = CIMA;
				break;
			case ALLEGRO_KEY_S:
				if(m->y + distM <= SCREEN_H) 
					m->y += 3*PASSO/2;
				m->direcao = BAIXO;
				break;
			case ALLEGRO_KEY_A:
				if(m->x - distM >= 0)
					m->x -= 3*PASSO/2;
				m->direcao = DIR;
				break;
			case ALLEGRO_KEY_D:
				if(m->x + distM <= SCREEN_W) 
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

int colidiuAsteroides(pirata ast1, pirata ast2) {
	int xi = TAM_mochileiro, yi = SCREEN_H - TAM_mochileiro;
	int xo = SCREEN_W - TAM_OBJ/2, yo = TAM_OBJ/2;
	if(dist(ast1.x,ast1.y,ast2.x,ast2.y)<= ast1.raio + ast2.raio || dist(ast2.x, ast2.y, xi, yi) <= ast2.raio + TAM_mochileiro*sqrt(2) 
		|| dist(ast2.x, ast2.y, xo, yo) <= ast2.raio + TAM_OBJ*sqrt(2)/2 )
		return 1;
	return 0;
}

int colisaoAsteroides(pirata a, pirata asts[], int n){

	int i = 0;
	for(i=0; i < n; i++){
		if(colidiuAsteroides(asts[i], a))
			return 1;
	}
	return 0;
}

//------------------------------------------BATALHA-----------------------------------------------------------------

void desenhaCenarioBatalha(){

	al_draw_bitmap(batalha, 0, 0, 0);
	al_draw_filled_rectangle(X_OPT, Y_OPT, SCREEN_W, SCREEN_H, al_map_rgb(0,0,0));
	al_draw_bitmap(mochila, X_MB, Y_MB, 0);
	al_draw_bitmap(pirat, X_PIRATA, Y_PIRATA, 0);

	al_draw_text(FONTE, al_map_rgb(181,14,14), X_OPT+ 30, Y_OPT + 10, 0, "ESPECIAL");
	al_draw_text(FONTE, al_map_rgb(181,14,14), X_OPT+ 30, Y_OPT + 50, 0, "ATACAR");
	al_draw_text(FONTE, al_map_rgb(181,14,14), X_OPT+ 30, Y_OPT + 90, 0, "FUGIR");


	al_draw_filled_circle(X_OPT + 15, Y_CIRCULO, 7, al_map_rgb(92, 11, 132));

	al_draw_filled_rectangle(120, 80, 224, 95, al_map_rgb(255, 255, 255));
 	al_draw_filled_rectangle(650, 80, 754, 95, al_map_rgb(255, 255, 255));

}

int detectouAsteroide(mochileiro m, pirata ast) {
	if(dist(m.x,m.y,ast.x,ast.y)<= distM + raioAsteroide)
		return 1;
	return 0;
}

void desenhaHP(pirata p[], int i, mochileiro m, int lvl){

	al_draw_filled_rectangle(652, 82, 652 + 100.0*(m.vida/m.vidaMax),93, al_map_rgb(255, 0, 0));
 switch(lvl){
 	case 1:
	 al_draw_filled_rectangle(122, 82, 122 + 100.0*(p[i].vida/p[i].vidaMax), 93, al_map_rgb(255, 123, 0));
	 break;
	case 2:
	 al_draw_filled_rectangle(122, 82, 122 + 100.0*(p[i].vida/p[i].vidaMax), 93, al_map_rgb(255, 0, 255));
	 break;
	case 3:
	 al_draw_filled_rectangle(122, 82, 122 + 100.0*(p[i].vida/p[i].vidaMax), 93, al_map_rgb(0,255,255));
	 break;
 }
}

void processaTeclaBatalha(mochileiro *m, int tecla){

	if(velocidade == 0){
		switch(tecla){
			
			case ALLEGRO_KEY_UP:
				if(Y_CIRCULO > Y_OPT + 28){
				contador--;
				Y_CIRCULO -= 40;
				}
				break;

			case ALLEGRO_KEY_DOWN:
				if(Y_CIRCULO < Y_OPT + 108){
					contador++;
					Y_CIRCULO += 40;
				}
				break;

			case ALLEGRO_KEY_ENTER:
				m->executar = 1;

				if(contador==ESPECIAL)
					m->acao = ESPECIAL;
				
				else if(contador==ATACAR)
					m->acao = ATACAR;

				else if(contador==FUGIR)
					m->acao = FUGIR;

				break;
		}
	}
}
//--------------------------------------------------------------------------FIM-----------------------------------------

void desenhaCenarioMorreu(mochileiro m){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 2*(SCREEN_W/5), 1*(SCREEN_H/3), 0, "GAME OVER");
	al_draw_textf(FONTE, al_map_rgb(255, 255, 255), 1*(SCREEN_W/5), (SCREEN_H/2), 0, "RECORDE: %d", m.recordeCod);
	al_draw_textf(FONTE, al_map_rgb(255, 255, 255), 3*(SCREEN_W/5), (SCREEN_H/2), 0, "PONTUACAO: %d", m.pontuacao);

}

void desenhaCenarioGanhou(mochileiro m){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(FONTE, al_map_rgb(255, 255, 255), 2*(SCREEN_W/5), 1*(SCREEN_H/3), 0, "PARABÉNS!");
	al_draw_textf(FONTE, al_map_rgb(255, 255, 255), 1*(SCREEN_W/5), (SCREEN_H/2), 0, "RECORDE: %d", m.recordeCod);
	al_draw_textf(FONTE, al_map_rgb(255, 255, 255), 3*(SCREEN_W/5), (SCREEN_H/2), 0, "PONTUACAO: %d", m.pontuacao);
}

int main(int argc, char **argv){
	
	srand(time(NULL));

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

	// Inicializa o audio 
  if (!al_install_audio()){
    fprintf(stderr, "Falha ao inicializar áudio.\n");
    return false;
  }

  if (!al_init_acodec_addon()){
    fprintf(stderr, "Falha ao inicializar codecs de áudio.\n");
    return false;
  }

  if (!al_reserve_samples(1)){
    fprintf(stderr, "Falha ao alocar canais de áudio.\n");
    return false;
  }
	// fim do ngc do audio

	//carregar a figura .jpg na variavel bird
    navegacao = al_load_bitmap("galaxyPixel.jpg");
    mochila = al_load_bitmap("jorj2.png");
    batalha = al_load_bitmap("fundo2.1.jpg");
    pirat = al_load_bitmap("inimigo2.png");

    //se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!navegacao) {
      fprintf(stderr, "nao encontrou a imagem do fundo!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
     //se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!mochila) {
      fprintf(stderr, "nao encontrou a imagem do jorj!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
    //se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!batalha) {
      fprintf(stderr, "nao encontrou a imagem do fundo2!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
    //se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!pirat) {
      fprintf(stderr, "nao encontrou a imagem do inimigo2!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
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

	//inicializa o modulo que permite carregar imagens no jogo
	al_init_image_addon();

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

	//musica
	al_reserve_samples(1);
	theme = al_load_sample("Sounds\\Musica.ogg");
	al_play_sample(theme, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL); // Tema principal
	
	int playing = 1;
	int modo_jogo = NAVEGACAO;
	int movementKey = 0;
	int movementFlag = 0;
	int tecla;
	
	initGlobais();

	mochileiro jorj;
	initmochileiro(&jorj);
	int MAX_ASTEROIDES = randInt(18,27);

	//pirata a;
	pirata asts[MAX_ASTEROIDES];

	int i=0;
	int indicador_asteroide = 0;

	for(i=0; i < MAX_ASTEROIDES; i++){
		
		initpirata(&asts[i]);

		if(colisaoAsteroides(asts[i], asts, i))
			i--;

	}

	FILE* recordeArq = fopen("recorde.txt", "r");
	fscanf(recordeArq, "%d", &jorj.recordeCod);
	fclose(recordeArq);
	
	
	//initpirata(&a);

	while(playing) {

		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {


			if(modo_jogo == NAVEGACAO){

				desenhaCenarioNaveg();
				desenhaPont(&jorj);
				desenhamochileiroNaveg(jorj);
				
				
				for(i=0; i < MAX_ASTEROIDES; i++)
				{
					//desenhaAsteroide(asts[i]);
				}
				
				//desenhaAsteroide(a);
				

				for(i=0; i < MAX_ASTEROIDES; i++)
				{	
					
					if(detectouAsteroide(jorj, asts[i]) && asts[i].vida>0)
					{
						modo_jogo = BATALHA;
						indicador_asteroide=i;
					}
					
					
					/*
					if(detectouAsteroide(jorj, a))
						modo_jogo = BATALHA;
					*/

				}

				if(chegouObjetivo(jorj))
					modo_jogo = FIM;

			}
			else if(modo_jogo == BATALHA){
				desenhaCenarioBatalha();
				desenhaHP(asts,indicador_asteroide,jorj,asts[indicador_asteroide].lvl);

					if(jorj.acao == FUGIR && jorj.executar == 1){
						if(randInt(1,10) >= 5){

							if(jorj.direcao==CIMA)
								jorj.y += distM;
							if(jorj.direcao==BAIXO)
								jorj.y -= distM;
							if(jorj.direcao=DIR)
								jorj.x -= distM;
							if(jorj.direcao==CIMA)
								jorj.x += distM;

							modo_jogo = NAVEGACAO;
							velocidade = 0;
							jorj.acao = 0;
						}else{	
							jorj.acao = MONSTRO;
							jorj.executar = 0;
						}
					}

					if(jorj.acao == ATACAR && jorj.executar == 1){
						al_draw_filled_circle(X_kek1 - velocidade, Y_kek1, 15, al_map_rgb(252,15,192));
						velocidade += 15;

						if(X_kek1 - velocidade < X_kek2 + 68){
							velocidade = 0;
							jorj.acao = MONSTRO;
							jorj.executar = 0;
							asts[indicador_asteroide].vida -= DANO_ATAQUE;
						}
					}

					if(jorj.acao == ESPECIAL && jorj.executar == 1){
						al_draw_filled_circle(X_kek1 - velocidade, Y_kek1, 40, al_map_rgb(252,15,192));
						velocidade += 10;

						if(X_kek1 - velocidade < X_kek2 + 68){
							velocidade = 0;
							jorj.acao = MONSTRO;
							jorj.executar = 0;
							asts[indicador_asteroide].vida -= DANO_ATAQUE_ESP;
						}
					}

					if(jorj.acao == MONSTRO && jorj.executar == 0){
						al_draw_filled_circle(X_kek2 +200+ velocidade, Y_kek2, 20, al_map_rgb(255, 255, 255));
						velocidade += 15;
						
						if(X_kek2 +200+ velocidade > X_kek1 +100){
							velocidade = 0;
							jorj.acao = 0;
							jorj.vida -= asts[indicador_asteroide].dano;
							
							if(jorj.vida <= 0)
								modo_jogo = FIM;
						}
						if(asts[indicador_asteroide].vida <= 0){
								if(asts[indicador_asteroide].lvl == 1)
							 		jorj.pontuacao += 50;
								else if(asts[indicador_asteroide].lvl == 2)
							 		jorj.pontuacao += 100;
								else if(asts[indicador_asteroide].lvl == 3)
							 		jorj.pontuacao += 150;

							 	velocidade = 0;
							 	jorj.acao = 0;	
					 			modo_jogo = NAVEGACAO;
							}
					}
			}	
			else if(modo_jogo == FIM){
				al_clear_to_color(al_map_rgb(0,0,0));
				if(jorj.pontuacao > jorj.recordeCod){
					recordeArq = fopen("recorde.txt", "w");
					fprintf(recordeArq, "%d", jorj.pontuacao);
					jorj.recordeCod = jorj.pontuacao;
					fclose(recordeArq);
				}
					desenhaCenarioGanhou(jorj);
					
				if (jorj.vida <= 0){
					desenhaCenarioMorreu(jorj);
				}
			}

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			movementFlag = 1;
			movementKey = ev.keyboard.keycode;

		} 
		else if(ev.type == ALLEGRO_EVENT_KEY_UP){
			movementFlag = 0;
		}

		if(movementFlag){
			if(modo_jogo == NAVEGACAO)
				playing = processaTeclaNaveg(&jorj, movementKey);
		} 
		else{
			if(modo_jogo == BATALHA)
				processaTeclaBatalha(&jorj, ev.keyboard.keycode);
		}

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
 
	return 0;
}